#include "user.h"

//PB2 CS  // 低电平有效  输出低电平就行 
//PB3 CLK 
//PB4 MISO 
//PB5 MOSI GPIO 复用模式

//1 SPI设备接口初始化函数
void SPI1_Init()
{
	//1、时钟配置 - SPI电路电路
	// 84mhz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	//GPIO 电路也不能落下
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	//2 gpio 引脚
	//通过填写结构体的方式 间接配置寄存器；；；
	GPIO_InitTypeDef spistruct;
	//led灯 链接三个  引脚
	spistruct.GPIO_Pin = GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5 ;	
	//配置引脚模式 - 输入 输出 模拟模式 复用模式
	spistruct.GPIO_Mode = GPIO_Mode_AF;
	//输出类型  推挽模式（ 0 1 ）  开漏（0 - 高组态）-高电平靠外部上拉电阻
	//00 推挽模式;;; 
	spistruct.GPIO_OType = GPIO_OType_PP;
	//上拉电阻 - 引脚默认电平
	spistruct.GPIO_PuPd = GPIO_PuPd_UP;
	//响应速度
	spistruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&spistruct);
	
	//3 具体指出 io口复用成了什么电路功能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);
	
	//4. 配置SPI电路
	SPI_InitTypeDef spiaaatruct;
	//全双工 - 
	spiaaatruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	//stm32 - 主机立场
	spiaaatruct.SPI_Mode = SPI_Mode_Master;
	//数据传输 一次8位
	spiaaatruct.SPI_DataSize = SPI_DataSize_8b;
	//时钟极性 + 时钟相位
	//空闲电平低电平 采样位置 第一个跳变
	spiaaatruct.SPI_CPOL = SPI_CPOL_Low;
	spiaaatruct.SPI_CPHA = SPI_CPHA_1Edge;
	//传输序列 - 先传输数据的高位
	spiaaatruct.SPI_FirstBit = SPI_FirstBit_MSB;
	//软件片选
	spiaaatruct.SPI_NSS = SPI_NSS_Soft;
	//波特率问题 - 2分之一 42mmhz
	spiaaatruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	//数据校验 - 保证数据传输的准确性
	 /* Initialize the SPI_CRCPolynomial member */
	spiaaatruct.SPI_CRCPolynomial = 7;
	//SPI电路的初始化
	SPI_Init(SPI1,&spiaaatruct);

	//SPI使能
	SPI_Cmd(SPI1,ENABLE);

}

//2 SPI设备读写函数 - 电路 - 移位寄存器
//参数的要写的数据
//返回是读的数据
u8 SPI1_ReadWriteByte(u8 data)
{
	//发送数据寄存器
	//返送完了 flag = 1 ，没发送完 flag = 0；
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)== RESET);  
		SPI_I2S_SendData(SPI1,data);
	
	//接收数据寄存器
	//接收完了 flag =1 ，没接收完 flag = 0 ；
	//
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
		return SPI_I2S_ReceiveData(SPI1);
}