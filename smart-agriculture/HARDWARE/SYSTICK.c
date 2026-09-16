

#include "user.h"

//1配置内核定时器 - 
void my_systcik_config()
{
	//1.选择时钟源
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	
	//2.初始值填写
	//168000
	if(SysTick_Config(SystemCoreClock/1000))
	{	
		//内核定时器专用-调试灯
		TurnOn_Led(LED3);
	}
}

//延时函数--- 利用内核定时器编写一个延时函数 
//基本定时是1ms
//循环1000次 
//1秒钟；
unsigned int timedelay = 0;
//中断函数 - 体统注册函数--不用声明；；
void SysTick_Handler()
{
	//让全局变量减少1;
	if(timedelay != 0 )
		timedelay--;
}
//自定义
void my_delay_ms(unsigned int num)
{	
	timedelay = num;
	while(timedelay != 0 );
}