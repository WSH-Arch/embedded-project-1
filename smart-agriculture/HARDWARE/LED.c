
#include "user.h"
//每一个硬件设备 - 写一个文件 - 每个文件中 包含  若干个函数  
//1.LED配置函数
//多个单词的组合  函数名字体现功能 第一个字母大写 用下划线连
void Config_Led()
{
	//1.配置时钟 - 函数 ；；；
	//GPIOE ;;;;  时钟怎么查看?查表？
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
		
	//2.配置引脚
	//通过填写结构体的方式 间接配置寄存器；；；
	GPIO_InitTypeDef ledstruct;
	//led灯 链接三个  引脚
	ledstruct.GPIO_Pin = GPIO_Pin_8 |GPIO_Pin_9 | GPIO_Pin_10 ;	
	//配置引脚模式 - 输入 输出 模拟模式 复用模式
	ledstruct.GPIO_Mode = GPIO_Mode_OUT;
	//输出类型  推挽模式（ 0 1 ）  开漏（0 - 高组态）-高电平靠外部上拉电阻
	ledstruct.GPIO_OType = GPIO_OType_PP;
	//上拉电阻 - 引脚默认电平
	ledstruct.GPIO_PuPd = GPIO_PuPd_UP;
	//响应速度
	ledstruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOE,&ledstruct);
	
	//初始化--- 当灯是低电平点亮的时候  给高电平
	GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
}

//2.点亮LED函数
void TurnOn_Led(unsigned int num)
{
	switch(num)
	{
		case LED1: GPIO_ResetBits(GPIOE,GPIO_Pin_8);break;
		case LED2: GPIO_ResetBits(GPIOE,GPIO_Pin_9);break;
		case LED3: GPIO_ResetBits(GPIOE,GPIO_Pin_10);break;
		default:break;
	}
}
//3 熄灭LED函数
void TurnOff_Led(unsigned int num)
{
	switch(num)
	{
		case LED1: GPIO_SetBits(GPIOE,GPIO_Pin_8);break;
		case LED2: GPIO_SetBits(GPIOE,GPIO_Pin_9);break;
		case LED3: GPIO_SetBits(GPIOE,GPIO_Pin_10);break;
		default:break;
	}
}
//延时函数
//us级别----1000000
void mydelay(unsigned int time)
{
	unsigned int z = time * 10000;
	while(z--);
} 

//4 LED闪烁函数
void Flash_Led()
{
	//1 点亮LED  
	TurnOn_Led(LED1);  
	//2 延时一下  
	mydelay(800);     
	//3 关闭LED  
	TurnOff_Led(LED1);  
	//4 延时一下   
	mydelay(800);  
} 

//5 LED流水函数 
void Flow_Led()
{


}