

#include "user.h"

//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

//1. 6个串口配置方法一样 只有 引脚不一样
//配置串口1
void my_usart1_config()
{
	//1.时钟问题
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	//2.GPIOA 9 10 ;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//3.GPIO引脚的复用配置  AF模式
	GPIO_InitTypeDef gpio_uart_struct;
	gpio_uart_struct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 ;	
	//复用模式
	gpio_uart_struct.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_struct.GPIO_OType = GPIO_OType_PP;
	gpio_uart_struct.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_uart_struct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA,&gpio_uart_struct);
	//4.GPIOA组 9 10 复用成 串口功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	//5.串口初始化
	USART_InitTypeDef usart1struct;
	//波特率
	usart1struct.USART_BaudRate = 115200;
	//不需要硬件配置
	usart1struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//输入输出
	usart1struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//不用校验
	usart1struct.USART_Parity = USART_Parity_No;
	//1位停止位
	usart1struct.USART_StopBits = USART_StopBits_1;
	//数据位
	usart1struct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&usart1struct);
	//6.中断控制的配置
	NVIC_InitTypeDef nvicstruct;
	//串口1中断
	nvicstruct.NVIC_IRQChannel = USART1_IRQn;
	nvicstruct.NVIC_IRQChannelCmd  = ENABLE;
	nvicstruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicstruct.NVIC_IRQChannelSubPriority = 0 ;
	NVIC_Init(&nvicstruct);
	//7.cmd 打开
	USART_Cmd(USART1,ENABLE);
	//8.中断使能函数
	//接收数据寄存器 数据不为空 发生中断
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}
//准备一个 缓冲区来存储上位机发送过的数据
char res[32] = "";
char i = 0 ;
//串口中断函数
void USART1_IRQHandler()
{
//	u8 res = USART_ReceiveData(USART1);
//	//用一种设备举例  然后可以 拓展到其他设备中去
//	if(res == 1)
//	{
//		//操作设备
//		TurnOn_Beep();
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
//	}
//	//s 关闭蜂鸣器
//	if(res == 's')
//	{
//		//操作设备
//		TurnOff_Beep();
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
//	}

	//
	//如果返回数值是1 代表 中断已经发生，flag = 1；
	//2次判断
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{
		//循环接受上位机发送过来的数据 
		res[i] = USART_ReceiveData(USART1);
		i++;
		if(i > 16)
			i = 0; 
		delay_us(50);
	}
	//比对字符串前n个字符
	//字符串形式 打开一个设备
	if(!strncmp(res,"openbeep",strlen("openbeep")))
	{
		TurnOn_Beep();
		i = 0 ;
		//把缓冲区的 从res开始 长度是 sizeof res 都填写成 0 ； 
		memset(res,0,sizeof(res));
		delay_us(10);
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
	//字符串形式 打开一个设备 又来一遍 
	if(!strncmp(res,"closebeep",strlen("closebeep")))
	{
		TurnOff_Beep();
		i = 0 ;
		//把缓冲区的 从res开始 长度是 sizeof res 都填写成 0 ； 
		memset(res,0,sizeof(res));
		delay_us(10);
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}


//1 串口2的配置
//配置部分；
void uart2init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//1 串口时钟配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//2 D组时钟配置
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
	//3引脚功能配置
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);
	
	//4.GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//输出类型不要配置
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//5.//串口初始化
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
	
	//6串口1使能
	USART_Cmd(USART2,ENABLE);
	//7串口中断使能
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	//8中断配置
	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);		
} 
//2 串口2函数已经调试通过 
uint8_t  Uart2RecvBuf[128] = {0};
uint8_t  Uart2RecvBufLen = 0 ; 
//串口2的中断函数
//中断服务函数
//串行是一个字符中断一次。
void USART2_IRQHandler(void)
{
	unsigned char  res;
	//判断是否发生中断
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{	
		res=USART_ReceiveData(USART2);
		Uart2RecvBuf[Uart2RecvBufLen] = res;
		Uart2RecvBufLen++;
		if(res == 0x01)
		{
			TurnOn_Led(LED2);
		}
		if(res == 0x02)
		{
			TurnOff_Led(LED2);
		}
	}
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
}

//3 串口2 发送多个字符
void Uart2SendBuf(char *buf, int slen)
{
	int i = 0;
	for(i = 0; i < slen; i++)
	{
		//循环发送,直到发送完毕
		//循环发送一定成功，因为在不断的检测发送状态位
		//发送完成 flag = 1 ；
		while((USART2->SR&0X40)==0);   
		USART_SendData(USART2,buf[i]);
	}
}

//4 串口2 
//给ESP8266发送AT指令
int SendWifiATCmd_OK(char* atcmd)
{
	//指令长度
	int atcmdlen = strlen((char*)atcmd);
	//清空接收缓冲区
	memset(Uart2RecvBuf, 0,128);
	Uart2RecvBufLen = 0 ;
	//提示发送的数据是:%s\r\n
	//printf("atcmd:%s len:%d\r\n",atcmd,atcmdlen);
	//发送命令
	Uart2SendBuf(atcmd,atcmdlen);
	delay_ms(100);
	while(1)
	{
		//返回OK 是对的
		if(strstr((char *)Uart2RecvBuf,"OK") != NULL)
		{
			printf("Recv AtCmd Ack:%s\r\n", Uart2RecvBuf);
			//返回OK字符串，表示执行AT命令成功
			return 0;
		}
		//返回 ERROR要立即返回 错误提示信息 ;
		//有error返回指针
		//没有error返回null
		//没有ok也没有error - 是对的。
		else if(strstr((char *)Uart2RecvBuf,"ERROR") != NULL)
		{
			printf("error:%s\r\n", Uart2RecvBuf);
			return -1;
		}
		//返回不是 ERROR也是对的
		else if(strstr((char *)Uart2RecvBuf,"busy") != NULL)
		{
			printf("busy:%s\r\n", Uart2RecvBuf);
			//返回OK字符串，表示执行AT命令成功
			return -2;
		}
	}
	//最后也要清空 
	memset(Uart2RecvBuf, 0,128);
	Uart2RecvBufLen = 0 ;
	return 0;

}
//检测什么呢？
int SendWifiATCmd_NO_OK(char* atcmd)
{
	//指令长度
	int atcmdlen = strlen((char*)atcmd);
	//清空接收缓冲区
	memset(Uart2RecvBuf, 0,128);
	Uart2RecvBufLen = 0 ;
	int timeout = 0 ;
	//提示发送的数据是:%s\r\n
	//printf("atcmd:%s len:%d\r\n",atcmd,atcmdlen);
	//发送命令
	Uart2SendBuf(atcmd,atcmdlen);
	delay_ms(1000);
	while(1)
	{	
		//返回OK 是对的
		if(strstr((char *)Uart2RecvBuf,"CONNECT") != NULL)
		{
			//返回OK字符串，表示执行AT命令成功
			return 0;
		}
		else
		{
			return -1;
		}
	} 
}

//指针数组 - 每一个成员都是一个指针
char *ESP8266_STA_Client[] = {
    "AT\r\n", // 1 检测ESP8266模块是否正常工作
    "ATE0\r\n",// 2 关闭回显功能
    "AT+CWMODE=1\r\n", // 3 设置为STA模式
    "AT+RST\r\n", // 4 重启ESP8266模块
	
    "ATE0\r\n", // 5 关闭回显功能
    "AT+CWLAP\r\n", // 6 查询可以连接的WIFI
    "AT+CWJAP=\"oneoneone\",\"12345678\"\r\n", // 7 连接指定的WIFI
    "AT+CIPMUX=0\r\n", // 8 设置为单连接模式 
    "AT+CIPMODE=1\r\n", //9 设置为透传模式
    "AT+CIPSTART=\"TCP\",\"192.168.137.1\",8088\r\n", //10 建立TCP连接
    "AT+CIPSEND\r\n",// 11 双方都可以发送数据 - 全双工模式  ;;; 
};

//通过串口2 发送指令给 ESP8266
int InitEsp8266()
{
	//1 发送 AT指令
	// 检测ESP8266模块是否正常工作
	if(SendWifiATCmd_OK(ESP8266_STA_Client[0])<0)
	{
		printf("AT ERROR\r\n");
		return -1; 
	}else
		printf("1 AT OK\r\n"); 
	delay_ms(100);
	//2// 关闭回显功能
	if(SendWifiATCmd_OK(ESP8266_STA_Client[1])<0)
	{
		printf("ATE0 ERROR\r\n");
		return -1; 
	}else
		printf("2 ATE0 OK\r\n"); 
	delay_ms(100);
	//3 // 设置为STA模式
	if(SendWifiATCmd_OK(ESP8266_STA_Client[2])<0)
	{
		printf("AT+CWMODE=1 ERROR\r\n");
		return -1; 
	}else
		printf("3 AT+CWMODE=1 OK\r\n"); 
	delay_ms(100);
	//4 // 重启ESP8266模块
	if(SendWifiATCmd_OK(ESP8266_STA_Client[3])<0)
	{
		printf("AT+RST ERROR\r\n");
		return -1; 
	}else
		printf("4 AT+RST OK\r\n"); 
	delay_ms(300);
	//5  关闭回显功能
	if(SendWifiATCmd_OK(ESP8266_STA_Client[4])<0)
	{
		printf("ATE0 ERROR\r\n");
		return -1; 
	}else
		printf("5 ATE0 OK\r\n");
	delay_ms(100);
	//6 查询可以连接的WIFI
	//这里只是查询一下 可以链接的wifi  所以没有使用它的返回值。
	SendWifiATCmd_NO_OK(ESP8266_STA_Client[5]);
	delay_ms(100);
	//7 连接指定的WIFI 
	//只有链接这一步需要使用返回数值。
	while(SendWifiATCmd_NO_OK(ESP8266_STA_Client[6])<0)
	{
		printf("7 connect wifi ERROR\r\n");
		delay_ms(1000);
	}
	printf("7 connect wifi OK\r\n"); 
	delay_ms(500);
	//8 设置为单连接模式  
	while(SendWifiATCmd_OK(ESP8266_STA_Client[7])<0)
	{
		printf("8 AT+CIPMUX=0 单链模式 ERROR\r\n");
		delay_ms(1000);
	}
	printf("8 AT+CIPMUX=0 单链模式 OK\r\n"); 
	delay_ms(500);
	//9 设置为透传模式
	while(SendWifiATCmd_OK(ESP8266_STA_Client[8])<0)
	{
		printf("9 AT+CIPMODE=1 透传模式 ERROR\r\n");
		delay_ms(500);
	}
		printf("9 AT+CIPMODE=1 透传模式 OK\r\n"); 
	delay_ms(300);
	//10 建立TCP连接 - 建立TCP连接之前，先把 上位机服务器打开 
	//这个是有error返回的，也行
	while(SendWifiATCmd_OK(ESP8266_STA_Client[9])<0)
	{
		printf("TCP链接  ERROR\r\n");
		delay_ms(300);
	}
		printf("10 TCP链接 OK\r\n"); 
	delay_ms(300);
	//11 互相传递数据 
	SendWifiATCmd_OK(ESP8266_STA_Client[10]);
	
	return 0 ;
}


