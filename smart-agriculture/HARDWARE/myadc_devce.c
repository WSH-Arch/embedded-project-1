


#include "user.h"

//1 光敏电阻 PA0
//2 模拟电压
//3 气体传感器
//4 酒精传感器
//5 人体红外传感器

//1 .adc的配置 PA0
void ADC1_LIGHT_SENSOR_Conifg()
{	
	//1.时钟配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//2.配置引脚
	GPIO_InitTypeDef lightstruct;
	lightstruct.GPIO_Pin = GPIO_Pin_0;	
	//模式复用 - 输入 输出 模拟 复用
	lightstruct.GPIO_Mode = GPIO_Mode_AN;
	lightstruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	lightstruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA,&lightstruct);
	//3.ADC1参数配置
	ADC_CommonInitTypeDef adc1struct; 
	//独立模式
	adc1struct.ADC_Mode = ADC_Mode_Independent;
	//间隔时间，第一次采集完毕之后 等待一会 
	adc1struct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	//DMA功能：当我们采集完毕之后，直接把数据从记过寄存器中拷贝出来放到 缓冲区当中。
	adc1struct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	//adc的采样频率 不能超过36mhz
	//adc电路 apb2 = 84 / 4 = 21mhz 
	adc1struct.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInit(&adc1struct); 
	
	//4 .ADC1参数配置
	ADC_InitTypeDef adc2struct; 
	
	//是否扫描 - 16个通道 - 不扫描多个通道
	adc2struct.ADC_ScanConvMode = DISABLE;
	//采集精度:
	adc2struct.ADC_Resolution = ADC_Resolution_12b;
	//连续转换 - 一次一结束
	adc2struct.ADC_ContinuousConvMode = DISABLE;
	//出发检测 - 软件触发，调佣函数 -才开始检测 - 
	//外部电平变化引起 开始检测 还是内部调用函数
	adc2struct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//采集完的数据 存储到16位的寄存器当中并且   右对齐
	adc2struct.ADC_DataAlign = ADC_DataAlign_Right;
	//规则组 while 还是注入组 interupt 1个序列;
	adc2struct.ADC_NbrOfConversion = 1 ;
	ADC_Init(ADC1,&adc2struct); 
	//5 开启使能
	ADC_Cmd(ADC1,ENABLE);
}

//2.开始采集函数
u16 ADC1_get_light_value()
{
	//1.设置规则通道组 - ADC1,1个序列 - 采样周期 - 
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_480Cycles);
	//2.开始采集的开关
	ADC_SoftwareStartConv(ADC1);
	//3.采样时间到 flag = 1 ;  没有采集完成 flag = 0 ;
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	//4.转换结束
	return ADC_GetConversionValue(ADC1);
}
//3.取平均数值函数
u16 ADC1_get_light_Average_value()
{
	//累积5次取平均数值
	u16 average = 0 ,light_sum  = 0 ;
	u16 times = 5;
	char i = 0 ;
	for(i = 0 ; i < times ; i++)
	{
		light_sum += ADC1_get_light_value();
		delay_ms(5);
	}
	return average  = light_sum / times ;
}









