

#include "user.h"

//1.中断配置
//GPIOE-6;
void INTERRUPT_KEY3_INIT()
{
	//1.时钟问题 - 中断控制器 打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	//2 用到io口了 - GPIOE-6;
	GPIO_InitTypeDef key_inter_struct;
	key_inter_struct.GPIO_Pin = GPIO_Pin_6 ;	
	//配置引脚模式 - 输入 输出 模拟模式 复用模式
	key_inter_struct.GPIO_Mode = GPIO_Mode_IN;
	//上拉电阻 - 引脚默认电平
	key_inter_struct.GPIO_PuPd = GPIO_PuPd_UP;
	//响应速度
	key_inter_struct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOE,&key_inter_struct);
	
	//3.把引脚配置成中断功能 - 引脚复用问题
	//6号引脚就有了中断功能了。
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource6);
	
	//4 中断选项问题
	EXTI_InitTypeDef extistruct;
	//使能
	extistruct.EXTI_LineCmd = ENABLE;
	//中断模式
	extistruct.EXTI_Mode = EXTI_Mode_Interrupt;
	//看懂 - 能改 - 记住
	extistruct.EXTI_Trigger = EXTI_Trigger_Falling;
	//中断线
	//GPIOA B C E
	//A2 B2 C2 D2 - 2号线；
	//A6 B6 C6 D6 E6 - 6号线
	//每一组的相同引脚都是一个线	
	extistruct.EXTI_Line = EXTI_Line6;
	EXTI_Init(&extistruct);
	
	//5 中断初始化问题- 中断控制器的配置
	NVIC_InitTypeDef nvicstruct;
	//中断类型 - 外部中断类型；
	//stm32f4xx.h  6号引脚在此处范围内
	nvicstruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	//
	nvicstruct.NVIC_IRQChannelCmd  = ENABLE;
	//抢占优先级和相应优先级
	nvicstruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicstruct.NVIC_IRQChannelSubPriority = 0 ;
	NVIC_Init(&nvicstruct);
	//优先级分组
	
}
//2.中断函数 - 中断执行函数
//系统注册-名字不能写别的----  EXTI3XXXX_IRQHandler()
//找到注册名字的选项，拿过来用;;;
//只要定义 不要声明 不要调用
void EXTI9_5_IRQHandler()
{
	//自定义代码
	TurnOn_Beep();
		
	//清除中断标志位
	EXTI_ClearITPendingBit(EXTI_Line6);
}




