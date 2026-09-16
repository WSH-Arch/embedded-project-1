
#include "user.h"

static u16 chipid = 0 ;
static u8 buff[128] = "";
extern char wendu[2];
extern char shidu[2];
#define READ_DHT11_OK 0
#define READ_DHT11_NOT_OK 1
char readbuf = 0 ;
//拓展 内存  缓存  8k 
extern u8 sendbuf[SEND_BUFF_SIZE];
//ESP8266 
int InitEsp8266();
void Uart2SendBuf(char *buf, int slen);

int main()
{
	//0.内核时钟初始化
	delay_init(168);
	//在main中进行分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//10 串口测试
	my_usart1_config();
	//1 LED初始化
	Config_Led(); 
	//2 BEEP初始化 
	Config_Beep();  
	//3 KEY初始化
	Config_Key();
	//4 自定义内核定时器的初始化	
	//my_systcik_config();  
	//5 中断配置  
	INTERRUPT_KEY3_INIT();  
	//6 预分频系数  初始参数  
	//TIMER4_Config(8400-1,5000-1);  
	//7 pwm-beep警报音
	//TIM2_PWM_CH3_BEEP_CONFIG();
	//8.单独测试某个设备的使用
	//TIM1_PWM_CH1_LED_CONFIG();
	//不断的修改 比较捕获寄存器的数值；
	//9.舵机测试 - 联合调试的时候再打开
	//TIM4_PWM_CH1_MOTOR_CONFIG();
	
	//11 esp8266
	uart2init();
	if(InitEsp8266() < 0)
	{
		printf("InitEsp8266 error");
	}
	else
		printf("InitEsp8266 successful");
	delay_ms(100);
	//串口2 发送的数据就是发送给ESP8266的；；；
	Uart2SendBuf("28256",strlen("28256"));
	
#if 0 
	//12 dht11初始化  
	DHT11_Config();  
	if(DHT11_ReadData()== READ_DHT11_OK)  
	{
		printf("温度是:%d .%d\r\n",wendu[0],wendu[1]);  
		printf("湿度是:%d .%d\r\n",shidu[0],shidu[1]); 
	} 
	else
		printf("read error\r\n");
	
	//打印菜单 
	//showmenu();
	//有线通信 ;
	//无线通信 ;
	printf("智慧农业系统\r\n");
	printf("|   1.功能1     |\r\n");
	printf("|   2.功能2    	|\r\n");
	printf("|   3.功能3     |\r\n");
	printf("|   4.功能4     |\r\n");
	printf("|   5.功能5     |\r\n");
	printf("|   6.功能6     |\r\n");
	printf("|   7.功能7     |\r\n");
	printf("|   8.功能8     |\r\n");
	
	//13 at24c02 test 
	//while(AT24C02_INIT()); 
	//写测试 
	char num = 0x97; 
	AT24C02_WRITE_BYTE(0x02,num); 
	
	AT24C02_READ_BYTE(0x02,&readbuf);   

	//14 oled显示  
	iic_init();  
	InitOLed();  
	//显示英文 
	OLed_Fill(0x00);//描点清屏函数； 
	//横坐标 纵坐标  内容 
	//Oled_print(1,1,"this fnesday"); 
	//显示图片
	SSD1306_DrawBMP(0,0,128,8,paint); 

	//15 spi-w25q64； 
	SPI1_Init();  
	w25q64_init(); 
	chipid = w25q64_readid(); 
	//擦除芯片 - 全部变成ff 
	//w25q64_eraser_chip_all(); 
	//读 1 地址 2指针 3长度 
	w25q64_read_data(1,buff,1); 

	//测试 写一个字符串  
	//hello -  
	u8 cbuff[128] = "hello" ;
	//写数据
	//w25q64_write_page(cbuff,2,strlen((char*)cbuff));

	//读数据
	u8 vbuff[128] = "";
	w25q64_read_data(2,vbuff,6);

	//每个模块单独测试通过以后 - 联合测试的时候 重新写
	//驱动层 + 逻辑层
	//16 adc-light sensor
	ADC1_LIGHT_SENSOR_Conifg();
	
	u16 lithvalue = ADC1_get_light_Average_value();
	//显示成字符串可以
	//再通过串口打印出来
	printf("light value = %d\r\n",lithvalue);
 
	//17 DMA控制器的使用
	MYDMA_CONFIG(DMA2_Stream7,DMA_Channel_4,(u32)(&USART1->DR),(u32)sendbuf,SEND_BUFF_SIZE);
	
	//准备数据
	int i = 0 ;
	for(i = 0 ; i < SEND_BUFF_SIZE;i++)
	{
		//字节是一个空间概念
		//字符是一个数据概念
		sendbuf[i] = 0xDD;
	}
	printf("DMA开始传输:\r\n");
	//允许传送-使能
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	//真正传送
	myDMA_Enable(DMA2_Stream7,SEND_BUFF_SIZE);
	delay_ms(500);
	//获取状态位
	//传输完毕 flag = 1 ， 传输没有完毕 flag = 0 ；
	while(!DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7));
	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7); 
	printf("DMA传输完成\r\n"); 
	//蜂鸣器 警报一声 
	Alarm_Beep();  
	 
	//声明  
	void Fan_Timer12_CH2_Config();  
	void Fan_Control(unsigned int cvalue);  
	//风扇配置函数  
	Fan_Timer12_CH2_Config(); 

	//风扇控制函数  
	Fan_Control(6000);   
	
#endif  

	extern uint8_t Uart2RecvBuf[128];
	memset(Uart2RecvBuf, 0,128);
	while(1) 
	{   //闪烁灯光
		
		Flash_Led();	
//		printf("串口2发送过来的数据是:%s\r\n",(char*)Uart2RecvBuf);
//		delay_ms(1000);
	} 
} 
//快捷编译方法  F7 
