# embedded-project-1

STM32 嵌入式实践项目集（个人课程与实践项目），包含两个独立工程。

## Projects

| 项目 | 平台 / 工具链 | 核心内容 |
|---|---|---|
| [smart-agriculture](./smart-agriculture) | STM32F407VE · 标准库 · Keil MDK5 | 温湿度与光照采集、OLED 显示、SPI Flash 存储、ESP8266 WiFi 上传 |
| [quadruped-robot-dog](./quadruped-robot-dog) | STM32U575 · HAL 库 · STM32CubeMX | 4 路 PWM 舵机驱动、动作序列编排与按键触发 |

## Hardware & Toolchain

- MCU：STM32F407VE（Cortex-M4）/ STM32U575RITx（Cortex-M33）
- IDE：Keil MDK5、STM32CubeMX
- 调试：ST-Link / J-Link、串口调试助手、示波器、万用表
- 外设：DHT11、光敏电阻、SSD1306 OLED、AT24C02、W25Q64、ESP8266、舵机

## Repository Structure

    embedded-project-1/
    ├── smart-agriculture/       # 智慧农业监控系统
    │   ├── CORE/   FWLIB/   HARDWARE/   SYSTEM/   USER/
    │   └── README.md
    └── quadruped-robot-dog/     # 四足机器狗舵机控制
        ├── Core/   Drivers/   MDK-ARM/
        └── README.md

## Notes

- 编译产物（Objects / Listings / *.o / *.axf / *.map 等）与本地配置文件（*.uvguix、JLinkLog.txt）已通过 `.gitignore` 排除，仓库仅包含源码与工程配置。
- `FWLIB/`、`Drivers/` 下的 ST 官方外设库与 HAL 库版权归 STMicroelectronics 所有，保留原始版权声明。
- 工程内的 WiFi SSID / 密码等凭据已替换为占位符，实际使用时请自行填入。
