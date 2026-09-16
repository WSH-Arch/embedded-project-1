
#include "user.h"


//DHT11设备
// PA3 引脚；
void DHT11_Config()
{
	//时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//引脚初始化
	GPIO_InitTypeDef dht11struct;
	//A3
	dht11struct.GPIO_Pin = GPIO_Pin_3;
	//输出模式
	dht11struct.GPIO_Mode = GPIO_Mode_OUT;
	//类型 - 开漏输出
	dht11struct.GPIO_OType = GPIO_OType_OD;
	//电阻
	dht11struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//响应速度
	dht11struct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA,&dht11struct);
	
	//输出高电平
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
}
// 输入模式函数
void DHT11_INMODE()
{
	//引脚初始化
	GPIO_InitTypeDef dht11struct;
	//A3
	dht11struct.GPIO_Pin = GPIO_Pin_3;
	//输入模式
	dht11struct.GPIO_Mode = GPIO_Mode_IN;
	//电阻
	dht11struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//响应速度
	dht11struct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA,&dht11struct);

}
// 输出模式函数
void DHT11_OUTMODE()
{
	//引脚初始化
	GPIO_InitTypeDef dht11struct;
	//A3
	dht11struct.GPIO_Pin = GPIO_Pin_3;
	//输出模式
	dht11struct.GPIO_Mode = GPIO_Mode_OUT;
	//类型 - 开漏输出
	dht11struct.GPIO_OType = GPIO_OType_OD;
	//电阻
	dht11struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//响应速度
	dht11struct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA,&dht11struct);
	//输出高电平
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
}
char buff[5] = "";
u8 DHT11_ReadDataByte();
char wendu[2] ="";
char shidu[2] ="";
//返回0  - 代表读到了数据
//返回1 - 代表执行失败 
// DHT11读数据函数
u8 DHT11_ReadData()
{
	//输出模式
	DHT11_OUTMODE();
	//1.低电平20ms + 高电平30us
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	delay_ms(20);
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	delay_us(30);

	//上拉电阻 - 等待高电平；
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3));
	
	//2.输入模式
	DHT11_INMODE();
	//等待低电平结束
	//从设备发送过来的低电平 也就是主设备读到的低电平
	while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3));
	//等待高电平结束
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3));		
	
	//数据问题 - 40bit
	//连续40个组合
	char i = 0 ; 
	//5个字节
	for( i = 0 ; i < 5 ; i++)
	{
		buff[i] = DHT11_ReadDataByte();
	}
	DHT11_OUTMODE();
	//校验
	if(buff[4] == buff[0] + buff[1] + buff[2] + buff[3])
	{
		//校验通过
		shidu[0] = buff[0];
		shidu[1] = buff[1];
		
		wendu[0] = buff[2];
		wendu[1] = buff[3];
		return 0;	
	}
	else
		return 1;
}
// DHT11读位函数-一次读8位 - 一个字节
u8 DHT11_ReadDataByte()
{
	u8 readbit = 0 ;
	DHT11_INMODE();
	//循环读8次
	char i = 0 ;
	for(i = 0 ; i < 8 ; i ++)
	{
		//放到前面 不影响，放到后面 高位会移动消失 ；；；
		readbit = readbit << 1; 
		
		//上拉电阻 - 等待高电平；
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3));
		//低电平过来 - 不需要低电平
		 while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3));
		
		//判断高电平时间。
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3))
			delay_us(30);
		
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3))
			readbit |= 0x01;
		else
			readbit &= 0xfe; //1111 1110
	}
	DHT11_OUTMODE();
	return readbit;
}