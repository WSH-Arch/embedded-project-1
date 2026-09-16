#include "user.h"

//查询表格 来的指令 
//常用指令数值
#define W25X_ManufactDeviceID	0x90 
//擦除片选命令
#define W25X_ChipErase			0x60   //或者h都可以
//读状态寄存器
#define W25X_ReadStatusReg		0x05
//读数据
#define W25X_ReadData			0x03
//写使能
#define W25X_WriteEnable		0x06
//写非使能
#define W25X_WriteDisable		0x04 
//页编程 就是写数据
#define W25X_PageProgram		0x02
//块擦除
#define W25X_BlockErase			0xD8 
//扇区擦除  
#define W25X_SectorErase		0x20

//1.设备初始化
void w25q64_init()
{
	//1pb2进行单独初始化
	//1.配置时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef w25struct;
	w25struct.GPIO_Pin = GPIO_Pin_2;
	w25struct.GPIO_Mode = GPIO_Mode_OUT;
	w25struct.GPIO_OType = GPIO_OType_PP;
	w25struct.GPIO_PuPd = GPIO_PuPd_UP;
	w25struct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOB,&w25struct);
	//片选低电平有效 --  给高电平
	GPIO_SetBits(GPIOB,GPIO_Pin_2);
	
	//2.1条  1主多从  关闭lora
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	GPIO_InitTypeDef LORAstruct;
	LORAstruct.GPIO_Pin = GPIO_Pin_1 ;	
	LORAstruct.GPIO_Mode = GPIO_Mode_OUT;
	LORAstruct.GPIO_OType = GPIO_OType_PP;
	LORAstruct.GPIO_PuPd = GPIO_PuPd_UP;
	LORAstruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOE,&LORAstruct);
	//初始化--- 当灯是低电平点亮的时候  给高电平
	GPIO_SetBits(GPIOE,GPIO_Pin_1);
	
}

//2.读出芯片的id
u16 w25q64_readid()
{
	u16 id = 0 ;
	//1 片选
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);
	
	//2 发送指令
	SPI1_ReadWriteByte(W25X_ManufactDeviceID);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	
	//3.返回4个字节
	id |= SPI1_ReadWriteByte(0x00);
	id <<= 8 ;
	id |= SPI1_ReadWriteByte(0x00);
	
	//4.取消片选
	GPIO_SetBits(GPIOB,GPIO_Pin_2);
	return id;
}

//3 写使能 
void w25q64_write_enable()
{
	//1.片选
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);
	//2.发送指令
	SPI1_ReadWriteByte(W25X_WriteEnable);
	//3.取消片选
	GPIO_SetBits(GPIOB,GPIO_Pin_2);
}

//4.检查是否写完 checkbusy
u8 w25q64_check_busy()
{
	//如果stat = 1 ，那就是忙 还没执行完上面的动作。
	u8 stat = 0 ;
	do{
			//1.片选
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			//2 指令 - 要读取对面的 状态寄存器-stm32 -存储器
			SPI1_ReadWriteByte(W25X_ReadStatusReg);
			//发送00 返回状态
			stat = SPI1_ReadWriteByte(0x00);
			//3 关闭片选
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
	}while(stat &0x01);
	//如果stat是 1那就继续循环
	return 0;
}

//5 擦除数据--ff
u8 w25q64_eraser_chip_all()
{
	//补充：
	w25q64_write_enable();
	w25q64_check_busy();
	//1.片选
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);
	//2.发送指令
	SPI1_ReadWriteByte(W25X_ChipErase);
	//3.关闭片选
	GPIO_SetBits(GPIOB,GPIO_Pin_2);
	
	//4.擦除数据 是很长时间。
	//检查是否busy
	w25q64_check_busy();
	return 0;
}

//6 写数据
//6,写函数
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pbuffer:要写入的数据指针
//writeaddr:开始写入的地址 24bit
//numbyte：最大字节数 256字节
void w25q64_write_page(unsigned char *pbuffer,unsigned int writeaddr,unsigned short int numbyte)
{
	unsigned int i = 0 ;
	//1.写使能
	w25q64_write_enable();
	//2.片选
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);
	//3.发送写指令
	SPI1_ReadWriteByte(W25X_PageProgram);
	//4.发送24bit地址
	SPI1_ReadWriteByte((u8)((writeaddr) >> 16));
	SPI1_ReadWriteByte((u8)((writeaddr) >> 8));
	SPI1_ReadWriteByte((u8)(writeaddr));
	//5.循环写数据
	for(i=0;i<numbyte;i++)SPI1_ReadWriteByte(pbuffer[i]);
	//6.取消片选
	GPIO_SetBits(GPIOB,GPIO_Pin_2);
	//7.等待写完
	w25q64_check_busy();	
}
//8 读数据
//地址 + 空间 + 长度。
unsigned int w25q64_read_data(uint32_t addr,uint8_t buff[],uint8_t len)
{
			//1.片选
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
	
			//2.发送读数据指令
			SPI1_ReadWriteByte(W25X_ReadData);  //一共发送 4个字节。24个字节能寻址所有的空间吗？8M
			//后面跟着的是地址  //取出大小部分。
			SPI1_ReadWriteByte((addr >> 16) & 0xff); 
			SPI1_ReadWriteByte((addr >> 8) & 0xff);
			SPI1_ReadWriteByte((addr) & 0xff);
	
			//3.读出数据
			uint8_t i = 0 ;
			for(i = 0 ; i < len ; i++)
			{
					//读也用这个函数来做。
					buff[i] = SPI1_ReadWriteByte(0);
				
			}
			//4.取消片选
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			//等待结束。
			w25q64_check_busy();
			//返回实际读到的字节数
			return i ;
}


























