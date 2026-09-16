

#include "user.h"


//无符号数组
u8 sendbuf[SEND_BUFF_SIZE];
//1.从内存中 输出大量的数据 到 串口1 使用DMA功能
// DMA 2个电路  每个电路 8个通道 每个通道 8个流
//1.数据流  2号流   4号通道  2号电路 
//2.通道 8个通道
//3.外设的地址。目标地址  数据要去哪？
//4.源头地址  数据哪里来？
//5 数据大小。 
void MYDMA_CONFIG(DMA_Stream_TypeDef *DMA_Streamx,u32 channel,u32 des,u32 sedbuf,u16 size)
{
	//1 配置2号DMA的电路
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); 
	//2 重新启动DMA电路 刷新2号流
	DMA_DeInit(DMA_Streamx);
	//3 等待DMA流 状态 可用的时候才能重新配置
	//flag 为 1 的时候才能向下运行。
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}
	
	//4.配置DMA所有参数
	DMA_InitTypeDef dmastruct;
	//几号通道 
	dmastruct.DMA_Channel  = channel ;
	//外设地址 数据要去哪？
	dmastruct.DMA_PeripheralBaseAddr = des;
	//数据从哪来？
	dmastruct.DMA_Memory0BaseAddr = sedbuf;
	//数据传输模式：内-外  外-内  内-内 外-外
	dmastruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	//数据传输的大小
	dmastruct.DMA_BufferSize = size;
	//外设非增量模式
	dmastruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//存储器的增量模式
	dmastruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//外部设备的数据长度  8位 
	dmastruct.DMA_PeripheralDataSize = 	DMA_PeripheralDataSize_Byte;
	//存储器的数据长度
	dmastruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		
	//传输模式  单次模式 循环模式
	dmastruct.DMA_Mode = DMA_Mode_Normal;
	//中等优先级
	dmastruct.DMA_Priority = DMA_Priority_Medium;
	//FIFO模型
	dmastruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dmastruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	//存储器突发单次传输
	dmastruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	//外部设备突发单次传输
	dmastruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA_Streamx,&dmastruct);
}

//DMA开启传输函数
void myDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 size)
{
	//1.关闭传输
	DMA_Cmd(DMA_Streamx,DISABLE);
	//2.确保状态位可以被设置
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}
	//3.数据传输量的配置
	DMA_SetCurrDataCounter(DMA_Streamx,size);
	//4.开启DMA传输
	DMA_Cmd(DMA_Streamx,ENABLE);
}


























