
#include "user.h"

//1 iic电路的配置函数
// //PB6  时钟 SCKL
//pb7 :  数据  SDA
void iic_init()
{

	//1 时钟问题配置好
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	//2 接口问题配置好
	GPIO_InitTypeDef iicstruct;
	//引脚做好
	iicstruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	//都要输出
	iicstruct.GPIO_Mode = GPIO_Mode_OUT;
	//输出类型是 开漏
	iicstruct.GPIO_OType = GPIO_OType_OD;
	//芯片内部 不用配置上拉电阻
	//芯片外部已经有上拉电阻了
	iicstruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//相应速度
	iicstruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&iicstruct);
	//3 初始化 - 数据线是 高电平
//	GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_SET);
//	//有效电平 - 时钟线位低电平的时候 数据线上的数据 不会被采集
//	GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_RESET);
}
//>> iic输出模式函数
void iic_outputmode()
{
		GPIO_InitTypeDef iicstruct;
		iicstruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		//输出模式
		iicstruct.GPIO_Mode = GPIO_Mode_OUT;
		//推挽输出
		iicstruct.GPIO_OType = GPIO_OType_PP;
		//低速输出
		iicstruct.GPIO_Speed = GPIO_Speed_100MHz;
		iicstruct.GPIO_PuPd = GPIO_PuPd_UP;
		//引脚初始化。
		GPIO_Init(GPIOB,&iicstruct);
}



//2 起始信号
void iic_start()
{
	//1 时钟线为高电平
	GPIO_SetBits(GPIOB,GPIO_Pin_6);
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	delay_us(2);
	
	//2.数据线跳变
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	delay_us(2);
	//3 从设备接到这个信号以后就回认为  传输开始
	// 函数结束之前，把时钟线 调成低电平  数据线上的数据 无效
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
}
//3 终止信号
void iic_stop()
{	
	//1 两个都是低电平
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	delay_us(2);
	
	//2.时钟线高电平
	GPIO_SetBits(GPIOB,GPIO_Pin_6);
	delay_us(2);
	//3 数据线  跳到高电平
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	delay_us(2);
	//4 end以后： 双高；空闲信号。
}

//4.iic协议发送一个字节、
void iic_send_byte_data(unsigned char data)
{
	
	//1.发送一位的方法 循环8次
	unsigned char i = 0 ; 
	for(i = 0 ; i < 8 ; i++)
	{
		//时钟线为低电平  数据线上的数据可以变化
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		delay_us(2);
		//数据位数
		//先传输高位 - 是时序要求 ；；；
		if((data >> (7 - i))& 0x01)  //要发送的数据最低位是1 就发送高电平
			GPIO_SetBits(GPIOB,GPIO_Pin_7);
		else			//否则就发送低电平
			GPIO_ResetBits(GPIOB,GPIO_Pin_7);
		//时钟线变成高电平 数据线就会被识别到
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
		delay_us(2);
	}
	
	//2 全部完成之后  时钟线为低电平 无效时钟电平
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	delay_us(1);
}
//5 第九个时钟周期 主设备 希望从数据线读到 低电平  = ACK ;
//返回数值 0
//返回数值 1
u8 iic_getack()
{
	//1.时钟线为低电平 - 数据线数据可以变化 - 无效时钟
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	delay_us(1);
	
	//2 目标是从数据线读到电平  先给高电平
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	delay_us(1);
	
	//3.时钟线为低电平 数据线上的数据 无效电平
	//时钟线为高电平  数据线上的数据是可信任的。
	GPIO_SetBits(GPIOB,GPIO_Pin_6);
	delay_us(1);
	
	//4 读数据线上的数据
	if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7))
	{
		//函数开始执行的时候  时钟线 低电平 
		//函数即将结束的时候  时钟线还是低电平
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		delay_us(1);
		//返回0代表读到的电平是 低电平
		return 0;
	}
	else
		//返回1代表读到的电平电平是高电平
		return 1;
}

//6.对面发送数据。我们写的cpu的代码。
//读数据
u8 iic_get_byte_data()
{
	u8 resdata = 0 ; 
    // 1000 0000 	
	//1从发送数据过来 一个字节
	//时钟线拉低
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	delay_us(1);
	
#if 1
	//2.数据线拉高
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	delay_us(1);
#endif
	
	//3.读一个字节 循环8次读位数据过来
	char i = 0 ; 
	for(i = 0 ; i < 8;i++)
	{
		//向左边移动一位
		//为什么是先移动而不是最后移动
		resdata <<= 1;
		//iic读数据的原则是 时钟线为高电平才读
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
		delay_us(1);
		
		//判断电平的方式
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7))
			resdata |=  0x01;  //置1
		else
			resdata &= 0xfe;   //清0
		delay_us(1);
		//时钟线拉低
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		delay_us(4);
	}
	//返回 读成功的数据
	return resdata;
}

//7 发送一个数据位过去
u8 iic_send_ack(unsigned char ack)
{
	//1 时钟线为低电平
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	delay_us(1);
	//2.判定要发送的电平是 高低
	if(ack == NACK)
		GPIO_SetBits(GPIOB,GPIO_Pin_7);
	else
		GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	//3. 本主机 把时钟线拉高 对面就会读这个电平
	GPIO_SetBits(GPIOB,GPIO_Pin_6);
	delay_us(2);
	
	//4.函数结束的时候 规则一致
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	delay_us(1);
}
