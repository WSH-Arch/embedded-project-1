
#include "user.h"

//
//1.定时方法 - 定时自然时间（在内部计数） + 计数-引脚脉冲个数 - 
//2.输出pwm波形
//片上定时器 14个；
//定时器 1 + 8 ： 高级定时器
//基本定时器 6 7 ：DAC:数字信号转换成模拟信号输出
//通用定时器：通用定时器 足够用了----
// 2 3 4 5 9 10 11 12 14 ；
//编程方法全一样。
// 位数不一样。16位定时器   32位； 
//执行中断函数 定时器 也有中断函数
//1 预分频系数 - 分成多少份 成为真正的时钟  
//2 自动装载数值：决定定时多少个机器周期
//TIMER4_Config(8400 - 1,50000 - 1);
void TIMER4_Config(u16 psc,u16 arr)
{
	//1.总线 时钟  
	//APB1>>>>42MHZ
	//定时器在此处 特殊 还是  84mhz  //  8400份
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	//2份 ---  42 4份----21mhz；
	//1 真正的时钟：总线时钟/分频系数 = 真正的时钟
	//1/真正的时钟 = 机器周期
	//机器周期 * 自动装载数值 = 定时时间
	
	//2. 配置时钟单元
	TIM_TimeBaseInitTypeDef time4struct;
	//定时器的配置 - 填写结构体的方式进行
	//自动装载数值 50000;
	time4struct.TIM_Period = arr;
	//预分频系数 8400
	time4struct.TIM_Prescaler = psc;
	//时钟分割;
	time4struct.TIM_ClockDivision = TIM_CKD_DIV1;
	//向上计数模式
	time4struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&time4struct);
	
	//3.允许定时器 执行中断函数
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
		
	//4.定时器使能
	TIM_Cmd(TIM4,ENABLE);
	//5.配置中断的等级步骤
	NVIC_InitTypeDef nvicstruct;
	//定时器中断类型 - stm32f4xx.h中进行查询  
	nvicstruct.NVIC_IRQChannel = TIM4_IRQn;
	nvicstruct.NVIC_IRQChannelCmd = ENABLE;
	// 0 1 2 3 
	nvicstruct.NVIC_IRQChannelPreemptionPriority = 0 ; 
	nvicstruct.NVIC_IRQChannelSubPriority = 0 ;
	NVIC_Init(&nvicstruct);
}

//中断函数的名字固定
//0.5s执行一次
//奇数次执行的现象
//偶数次数执行的现象相反
u8 flg = 0 ;
void TIM4_IRQHandler()
{
	//1 蜂鸣器代替
	if(flg % 2 == 0)
	{
		TurnOn_Beep();
		flg++;
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	}else
	if(flg % 2 == 1)
	{
		TurnOff_Beep();
		flg++;
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	}
	if(flg > 10000)
		flg = 0 ;
}

