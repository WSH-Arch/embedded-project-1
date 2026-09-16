
#include "user.h"

//1 蜂鸣器配置
//PB10
void Config_Beep()
{
	//1.配置时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	//2.配置引脚
	GPIO_InitTypeDef beepstruct;
	beepstruct.GPIO_Pin = GPIO_Pin_10;
	//输出模式
	beepstruct.GPIO_Mode = GPIO_Mode_OUT;
	//推挽模式 输出 0  1 ；
	beepstruct.GPIO_OType = GPIO_OType_PP;
	//上拉电阻
	beepstruct.GPIO_PuPd = GPIO_PuPd_UP;
	//低速响应
	beepstruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOB,&beepstruct);
	//3.初始化引脚 - 给相反的电平
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}

//2 打开蜂鸣器
void TurnOn_Beep()
{	
	//向某个组中的某个引脚写不同的电平
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_SET);

}
//3 关闭蜂鸣器
void TurnOff_Beep()
{ 
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_RESET);
}

//4 蜂鸣器警报音
void Alarm_Beep()
{
	//1.打开蜂鸣器
	//2 延时
	//3 关闭蜂鸣器
	//4 延时
	
	GPIO_ToggleBits(GPIOB,GPIO_Pin_10);
	delay_ms(50);
	GPIO_ToggleBits(GPIOB,GPIO_Pin_10);
	delay_ms(50);
}


