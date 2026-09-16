

#include "user.h"

//at24c04 存储设备
//底层依赖iic接口
#define AT24C02_WRITE_ADDR 0xA0 //1010 0000 
#define AT24C02_READ_ADDR 0xA1 //1010 0001 

//1.设备初始化
//检测设备是否存在
//返回 0 代表设备存在
//返回 1 代表设备不存在
u8 AT24C02_INIT()
{
	//iic接口初始化
	iic_init();
	//起始信号
	iic_start();
	//发送设备地址
	iic_send_byte_data(AT24C02_WRITE_ADDR);
	//获取ack
	if(iic_getack())
	{
		iic_stop();
		return 1 ;
	}
	else
		return 0;
}

//2. 随机写
//参数要求：
//1 设备地址
//2 字节地址
//3 数据本身//一个字节的数据
u8 AT24C02_WRITE_BYTE(u8 wordaddr,u8 data)
{
	//1.起始信号
	iic_start();
	//2.发送设备地址 + w
	iic_send_byte_data(AT24C02_WRITE_ADDR);
	//3.获取ack信号 - 0 
	if(iic_getack())
	{
		iic_stop();
		return 1 ;
	}
	//4.发送字节地址
	iic_send_byte_data(wordaddr);
	//5.获取ack信号
	if(iic_getack())
	{
		iic_stop();
		return 1 ;
	}
	//6.发送数据本身
	iic_send_byte_data(data);
	//7.获取ack信号
	if(iic_getack())
	{
		iic_stop();
		return 1 ;
	}
	//8.停止信号
	iic_stop();
	return 0;
}
//3. 随机读
//读取成功返回 0
//读取失败 返回 1
//读到的数据怎么办
unsigned char AT24C02_READ_BYTE(unsigned char word_addr,char * readbuf)
{
	u8 res = 0 ;
	//1 起始信号
	iic_start();
	//2.发送设备地址 - 写地址 ；；
	iic_send_byte_data(AT24C02_WRITE_ADDR);
	//3.获取ack信号
	if(iic_getack())
	{
		iic_stop();
		return 1 ;
	}
	//4 发送字节地址 
	iic_send_byte_data(word_addr);
	if(iic_getack())
	{
		iic_stop();
		return 1 ;
	}
	
	//5 起始信号
	iic_start();
	//6 发送设备地址
	iic_send_byte_data(AT24C02_READ_ADDR);
	if(iic_getack())
	{
		iic_stop();
		return 1 ;
	}
	
	//7.读取对方的数据
	*readbuf = iic_get_byte_data();
	
	//5.发送 NACK信号
	iic_send_ack(NACK);
	//6.停止信号
	iic_stop();

	return 0;
}





















