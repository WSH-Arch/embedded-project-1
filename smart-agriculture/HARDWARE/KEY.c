

#include "user.h"

//不是所有的代码都是从 0 写
//看懂例题程序 - 一个模块 厂家赠送 驱动资料  驱动代码 --  
//能不能看懂 - 能不能修改 - 能不能移植 - 能不能把功能调通 - 能不能理解原理。
//加快开发效率


//1.按键配置函数
//GPIOE -时钟 ahb1总线  
void Config_Key()
{
	//1 配置时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	//2  gpio 上拉电阻 + 输入模式 
	GPIO_InitTypeDef keystruct;
	//3 号引脚
	keystruct.GPIO_Pin = GPIO_Pin_4;
	//4  输入模式 -读引脚电平
	keystruct.GPIO_Mode = GPIO_Mode_IN;
	//5 电阻问题
	keystruct.GPIO_PuPd = GPIO_PuPd_UP;
	//6 响应速度 上拉电阻
	keystruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE,&keystruct);
}
#define KEY_UP 1
#define KEY_DOWN 0
//自由控制板载资源
//2 按键检测函数
void Scan_Key()
{
	//返回值是 读到的那一组的那一个引脚的电平
	//读到高电平就返回1 读到低电平就返回 0
	u8 stat = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4);
	if(stat ==  KEY_DOWN )
	{	//20个基本单位
		mydelay(2);         //消除抖动
		if(stat ==  KEY_DOWN )
		{
			//松手判断
			while(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4));
			//操作其他硬件
			TurnOn_Beep();
		}
	}

}