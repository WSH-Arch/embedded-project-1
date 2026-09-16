
#include "user.h"


//1.定时器-pwm配置函数
//beep警报音 - 设备链接到了哪个引脚？
//moto模块 - 设备链接到了哪个引脚？
//定时器2的通道3驱动蜂鸣器
void TIM2_PWM_CH3_BEEP_CONFIG()
{
	//1.时钟问题
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//GPIOB 组引脚
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	//2.配置引脚 复用模式
	GPIO_InitTypeDef beeppwmstruct;
	beeppwmstruct.GPIO_Pin = GPIO_Pin_10;
	//pwm是复用模式
	beeppwmstruct.GPIO_Mode = GPIO_Mode_AF;
	//推挽模式 输出 0  1 ；
	beeppwmstruct.GPIO_OType = GPIO_OType_PP;
	//上拉电阻
	beeppwmstruct.GPIO_PuPd = GPIO_PuPd_UP;
	//低速响应
	beeppwmstruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOB,&beeppwmstruct);
	
	//》》》》 pb10 - 复用成 定时器2的功能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2);
	
	//2.定时器基本配置
	TIM_TimeBaseInitTypeDef timstruct;
	//用例程的 数值 有现象
	timstruct.TIM_Prescaler = 84 - 1;
	//装载数值
	timstruct.TIM_Period = 1000 - 1;
	//向上计数方式
	timstruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&timstruct);
	
	//3.pwm 配置通道
	TIM_OCInitTypeDef oc3struct;
	//模式
	oc3struct.TIM_OCMode = TIM_OCMode_PWM1;
	//比较使能
	oc3struct.TIM_OutputState = TIM_OutputState_Enable;
	//极性 - 有效电平
	oc3struct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM2,&oc3struct);
	 
	//4. 定时器使能 
	TIM_Cmd(TIM2,ENABLE); 
	//5.比较捕获寄存器使能 
	TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable); 
	//6.自动装载数值使能 
	TIM_ARRPreloadConfig(TIM2,ENABLE); 
}

//beep pwm驱动函数
void BEEP_PWM_DRIVER()
{
	unsigned int value = 0 ,flg = 1 ;
	//value 0 - 60 - 0 
	int num = 100;
	while(num--)
	{
		if(flg == 1)
			value++;
		else
			value--;
		
		//判断范围
		if(value > 600)
			flg = 0 ; 
		if(value == 0 )
			flg = 1;
		//修改捕获比较寄存器的数值
		TIM_SetCompare3(TIM2,value);
		delay_ms(10);
	}
}

//2.舵机模块
//定时周期在20ms>>> 总线时钟/预分频  + 壮哉数值---准备一个合适数值
//高电平时间     0.5ms- 1ms - 1.5m - 2ms - 2.5ms之间
//舵机转动角度是 0度 - 45---90---135-- 180度；
void TIM4_PWM_CH1_MOTOR_CONFIG()
{
	//1.配置时钟 - timer + GPIO 
	//84MHZ  GPIOD 12 ; 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
	//2.GPIOD - 12 ; 
	GPIO_InitTypeDef motostruct;
	//引脚
	motostruct.GPIO_Pin = GPIO_Pin_12;
	//复用模式
	motostruct.GPIO_Mode = GPIO_Mode_AF;
	//上拉电阻
	motostruct.GPIO_PuPd = GPIO_PuPd_UP;
	//低速
	motostruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOD,&motostruct);
	
	//3.引脚复用映射  PD12 - TIM2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
	
	//4.配置定时器
	TIM_TimeBaseInitTypeDef timstruct;
	//自动装载数值 - 定时周期确定
	timstruct.TIM_Period = 20000 - 1; //定时周期20ms
	//分频系数
	//84M / 84 = 1 M
	//1000000hz  周期/秒
	//一个周期: 1us
	//定时时间 20ms = 20000us / 1 = 20000 个数值。
	timstruct.TIM_Prescaler = 84 -1 ; 
	timstruct.TIM_CounterMode = TIM_CounterMode_Up;
	timstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4,&timstruct);
	
	//5 配置通道
	TIM_OCInitTypeDef oc1struct;
	//cnt小于 ccr3 - 是有效电平
	oc1struct.TIM_OCMode = TIM_OCMode_PWM1;
	//高电平是有效电平
	oc1struct.TIM_OCPolarity = TIM_OCPolarity_High;
	//使能
	oc1struct.TIM_OutputState = TIM_OutputState_Enable;
	//初始捕获比较寄存器的数值
	oc1struct.TIM_Pulse = 0 ; 
	TIM_OC1Init(TIM4,&oc1struct);

	//6 比较捕获寄存器使能
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	//7 自动装载数值寄存器的使能
	TIM_ARRPreloadConfig(TIM4,ENABLE);
	
	//8 定时器本身的使能
	TIM_Cmd(TIM4,ENABLE);
}

//
void MOTO_LEFT()
{
	//500 * 1us = 0.5ms
	TIM_SetCompare1(TIM4,500);
	delay_ms(1000);
}

void MOTO_RIGHT()
{
	TIM_SetCompare1(TIM4,2500);
	delay_ms(1000);
}
	
//3.直流电机模块
void TIM12_PWM_CH2_FAN_CONFIG()
{
	
	

}
//4.LED模块；
void TIM1_PWM_CH1_LED_CONFIG()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	//1TIM1时钟配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE); 
	//2E组时钟配置
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 
	
	//3重启动定时器1
	TIM_DeInit(TIM1);
	
	//4E组第9个引脚复用为定时器1
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_TIM1); 
	
	//5配置E组引脚配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   
	//复用功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  
	//速度100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
	//推挽复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	//5初始化引脚
	GPIO_Init(GPIOE,&GPIO_InitStructure);  
	
	//6高级定时器初始化
	TIM_TimeBaseStructure.TIM_Prescaler=400;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=100;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 1; 
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
		
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 	        
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//7根据T指定的参数初始化外设TIM1 4OC1
//	TIM_OCInitStructure.TIM_Pulse = 100;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure); 
	//8.使能定时器
	TIM_Cmd(TIM1, ENABLE);  	
	//9控制输出模式使能
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
//呼吸灯
void ledtestpwm()
{
	u16 led0pwmval=0;    
	u8 dir=1; 
	while(1) 
	{
 		delay_ms(10);
		//dir==1 led0pwmval递增
		if(dir)
			led0pwmval++;
		else     //dir==0 led0pwmval递减 
			led0pwmval--;
		//led0pwmval到达300后，方向为递减
 		if(led0pwmval>100)
			dir=0;
		//led0pwmval递减到0后，方向改为递增
		if(led0pwmval==0)
			dir=1;	
		//修改比较值，修改占空比	
		TIM_SetCompare1(TIM1,led0pwmval);	
	}
}

//1.风扇配置函数 
//TIM12_CH2 
void Fan_Timer12_CH2_Config() 
{
	//1 PB15
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	//2 
	GPIO_InitTypeDef GPIOB151struct;
	//引脚
	GPIOB151struct.GPIO_Pin = GPIO_Pin_15;
	//复用模式
	GPIOB151struct.GPIO_Mode = GPIO_Mode_AF;
	//上拉电阻
	GPIOB151struct.GPIO_PuPd = GPIO_PuPd_UP;
	//低速
	GPIOB151struct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOB,&GPIOB151struct);
	
	//3.
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM12);
	
	//4.
	TIM_TimeBaseInitTypeDef timstruct;
	//自动装载数值
	timstruct.TIM_Period = 20000 - 1;
	//分频系数
	timstruct.TIM_Prescaler = 84 -1 ; 
	timstruct.TIM_CounterMode = TIM_CounterMode_Up;
	timstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM12,&timstruct);
	
	//5 配置通道
	TIM_OCInitTypeDef oc2struct;
	//cnt小于 ccr3 - 是有效电平
	oc2struct.TIM_OCMode = TIM_OCMode_PWM1;
	//高电平是有效电平  
	//当前数值小于比较捕获寄存器的数值的时候输出高电平。
	oc2struct.TIM_OCPolarity = TIM_OCPolarity_High;
	//使能
	oc2struct.TIM_OutputState = TIM_OutputState_Enable;
	//初始捕获比较寄存器的数值
	oc2struct.TIM_Pulse = 0 ; 
	TIM_OC2Init(TIM12,&oc2struct);
	
	//6 比较捕获寄存器使能
	TIM_OC2PreloadConfig(TIM12,TIM_OCPreload_Enable);

	//7 自动装载数值寄存器的使能
	TIM_ARRPreloadConfig(TIM12,ENABLE);
	//8 定时器本身的使能、
	TIM_Cmd(TIM12,ENABLE);
}
//风扇控制函数
void Fan_Control(unsigned int cvalue)
{	
	TIM_SetCompare2(TIM12,cvalue);
}

























