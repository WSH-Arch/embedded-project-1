# 智慧农业监控系统（STM32F407VE）

基于 STM32F407 的农业环境监测系统：采集温湿度与光照强度，本地显示、掉电存储，并通过 WiFi 上传到上位机。

## 功能概览

```
DHT11（温湿度，PA3）  ─┐
                       ├─→ STM32F407VE ─┬─→ SSD1306 OLED 显示
光敏电阻（ADC）       ─┘                ├─→ W25Q64 Flash 存储
                                        ├─→ 蜂鸣器 / LED 报警（超阈值）
                                        └─→ USART2 → ESP8266 → WiFi → PC（TCP）
```

## 硬件清单

| 模块 | 型号 / 说明 | 接口 |
|---|---|---|
| 主控 | STM32F407VE（Cortex-M4，168MHz） | — |
| 温湿度传感器 | DHT11 | 单总线（PA3） |
| 光照传感器 | 光敏电阻 + 分压电路 | ADC1 |
| 显示屏 | SSD1306 OLED 128×64 | 软件模拟 I2C |
| 存储器 1 | AT24C02 EEPROM | 软件模拟 I2C |
| 存储器 2 | W25Q64 SPI Flash（8MB） | SPI1 |
| 无线模块 | ESP8266（AT 指令） | USART2 |
| 人机交互 | 按键、LED、蜂鸣器 | EXTI / GPIO / TIM PWM |
| 执行器 | 直流电机 / 风扇 / 舵机 | TIM PWM |

## 引脚分配

| 功能 | 引脚 | 说明 |
|---|---|---|
| DHT11 数据线 | PA3 | 开漏输出 + 输入模式动态切换 |
| I2C SCL | PB6 | 软件模拟，开漏，需外部上拉电阻 |
| I2C SDA | PB7 | 软件模拟，开漏，需外部上拉电阻 |
| W25Q64 片选 | PB2 | 低电平有效 |
| 另一从机片选 | PE1 | 拉高以关闭 LoRa 模块，避免 SPI 总线冲突 |
| 按键（中断） | PE6 | 下降沿触发，EXTI 线 6，NVIC `EXTI9_5_IRQn` |
| 蜂鸣器 | TIM2_CH3 | PSC=84-1，ARR=1000-1 → 1kHz 报警音 |
| 电机 / 舵机 | TIM4_CH1 | PSC=84-1，ARR=20000-1 → 20ms（50Hz） |
| 风扇 | TIM12_CH2 | PSC=84-1，ARR=20000-1，占空比调速 |
| 调试串口 | USART1 | printf 重定向（`fputc`）+ 接收中断 |
| 无线串口 | USART2 | 接 ESP8266，AT 指令交互 |
| DMA | DMA2_Stream7 / Channel4 | 内存 → `USART1->DR`，单次 8200 字节 |

## 目录结构

    smart-agriculture/
    ├── CORE/        Cortex-M4 内核与启动文件
    ├── FWLIB/       STM32F4xx 标准外设库
    ├── HARDWARE/    外设驱动（本项目核心代码）
    │   ├── DTH11.c/.h            DHT11 单总线时序驱动
    │   ├── iic.c/.h              软件模拟 I2C
    │   ├── iic_ssd1306_oled.c/.h OLED 驱动（字模 / 图片取模）
    │   ├── iic_at24c02.c/.h      EEPROM 读写
    │   ├── SPI.c / SPI_W25Q64.c  SPI 与 Flash 指令封装
    │   ├── myadc_devce.c         ADC 采集与均值滤波
    │   ├── pwm.c                 PWM：蜂鸣器 / 电机 / 风扇
    │   ├── my_usart.c            串口、printf 重定向、ESP8266 AT 指令
    │   ├── dma.c                 DMA 配置与启动
    │   └── interrupt.c           EXTI 外部中断
    ├── SYSTEM/      延时、位操作等基础函数
    └── USER/        主程序（user.c）、工程文件 Smart_AGR.uvprojx

## 关键实现说明

**DHT11 单总线时序**：主机拉低 ≥18ms 发起，随后释放总线；DHT11 回应后发送 40bit（5 字节）数据，每位以 50μs 低电平起始，高电平 26–28μs 表示 0、70μs 表示 1。驱动通过 GPIO 输入/输出模式动态切换与微秒级延时实现，读取 5 字节后做校验和判断（前 4 字节之和 == 第 5 字节）。

**开漏输出 + 外部上拉**：DHT11 与 I2C 均为单线双向通信，采用开漏输出避免双方同时驱动总线造成冲突，高电平由外部上拉电阻提供。

**SPI Flash 操作**：按指令集操作 W25Q64 —— 0x90 读 ID、0x06 写使能、0x02 页写（每次 ≤256 字节，含 24 位地址）、0x20 扇区擦除、0x05 读状态寄存器判断忙状态。Flash 只能将 1 改写为 0，因此修改数据前必须先擦除。

**ESP8266 联网流程**：串口发送 AT 指令序列完成联网与数据上传 ——

    AT                                  # 握手
    AT+CWMODE=1                         # STA 模式
    AT+RST                              # 重启生效
    AT+CWJAP="<SSID>","<PASSWORD>"      # 连接路由器
    AT+CIPMUX=0                         # 单连接
    AT+CIPMODE=1                        # 透传模式
    AT+CIPSTART="TCP","<SERVER_IP>",<PORT>   # 连接上位机 TCP 服务器
    AT+CIPSEND                          # 进入发送

    说明：仓库中的 SSID / 密码 / 服务器地址已替换为占位符，请填入自己的配置。

**DMA 传输**：使用 DMA2 的 7 号数据流、通道 4，将 8200 字节缓冲区搬送到 USART1 数据寄存器，内存地址递增、外设地址固定，传输结束后查询 `DMA_FLAG_TCIF7` 标志确认完成，避免 CPU 逐字节发送。

## 编译与运行

1. 使用 Keil MDK5 打开 `USER/Smart_AGR.uvprojx`
2. 编译前确认已安装 STM32F4xx 器件支持包
3. 连接 ST-Link / J-Link 下载程序
4. USART1 接 USB-TTL 可在串口助手中查看调试输出（printf 重定向）
## 代码导航

| 目录 / 文件 | 来源 | 说明 |
|---|---|---|
| `HARDWARE/` | **本人编写** | DHT11 单总线、软件 I2C、OLED、AT24C02、W25Q64、ADC、PWM、串口/DMA、EXTI 中断 |
| `USER/user.c` | **本人编写** | 主程序与各模块联调流程 |
| `SYSTEM/` | 通用基础库 | 微秒/毫秒延时与位操作 |
| `FWLIB/` `CORE/` `USER/stm32f4xx.h` | ST 官方 | STM32F4xx 标准外设库与内核文件，未做修改 |
| `USER/Smart_AGR.uvprojx` | Keil 工程 | 双击可打开编译 |

## 调试记录

- DHT11 读取偶发失败：定位为微秒级延时精度不足与中断打断时序，改用 GPIO 直读 + `delay_us` 并缩短关键段代码
- SPI 总线冲突：总线上同时挂载 Flash 与 LoRa 模块，除 Flash 片选外需将另一从机片选拉高
- 大块数据串口发送阻塞：改用 DMA 搬运，CPU 占用明显下降

## 已知局限

- 传感器精度与供电稳定性为实验级别，未做标定与外壳设计
- 未加入看门狗与断线重连机制，长时间运行可靠性有限
- 无 RTOS，功能以裸机轮询 + 中断实现
