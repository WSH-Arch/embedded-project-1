

#ifndef __USER
#define __USER

#include "stm32f4xx.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "delay.h"

//后续所有函数的声明  都在此文件当中
//led 函数声明
#define LED1 1
#define LED2 2
#define LED3 3
void Config_Led();
void TurnOn_Led(unsigned int num);
void TurnOff_Led(unsigned int num);
void Flash_Led();
void Flow_Led();
void mydelay(unsigned int time);

//BEEP
void Config_Beep();
void TurnOn_Beep();
void TurnOff_Beep();
void Alarm_Beep();
//KEY
void Config_Key();
void Scan_Key();

//systick
void my_delay_ms(unsigned int num);
void my_systcik_config();

//中断
void INTERRUPT_KEY3_INIT();

//定时器
void TIMER4_Config(u16 psc,u16 arr);

//pwm
void TIM2_PWM_CH3_BEEP_CONFIG();
void BEEP_PWM_DRIVER();
//代码移植-提高效率 - 前提看懂；
void TIM1_PWM_CH1_LED_CONFIG();
void TIM4_PWM_CH1_MOTOR_CONFIG();
void ledtestpwm();
void MOTO_LEFT();
void MOTO_RIGHT();
//usart
void my_usart1_config();
void uart2init();

//DHT11
void DHT11_Config();
void DHT11_INMODE();
void DHT11_OUTMODE();
u8 DHT11_ReadData();
u8 DHT11_ReadDataByte();

//iic 的驱动函数
void iic_init();
void iic_start();
void iic_stop();
void iic_send_byte_data(unsigned char data);
u8 iic_getack();
u8 iic_get_byte_data();
u8 iic_send_ack(unsigned char ack);
#define ACK 0
#define NACK 1
//at24c02设备函数
u8 AT24C02_INIT();
u8 AT24C02_WRITE_BYTE(u8 wordaddr,u8 data);
unsigned char AT24C02_READ_BYTE(unsigned char word_addr,char * readbuf);
//oled驱动函数
void WriteOLedCmd(uint8_t cmd);
void WriteOLedData(uint8_t data);
//清屏函数
void OLed_Fill(unsigned char bmp_data);
void OLed_SetPos(unsigned char x, unsigned char y);
void InitOLed(void);
void Oled_print(uint8_t x, uint8_t y,char *str);
void OLed_ShowChina(uint8_t x,uint8_t y,uint8_t *buf);
void SSD1306_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);
extern u8 paint[];

//SPI函数 + w25q64 - 128 ;// 
void SPI1_Init();
u8 SPI1_ReadWriteByte(u8 data);
void w25q64_init();
u16 w25q64_readid();
u8 w25q64_eraser_chip_all();
void w25q64_write_page(unsigned char *pbuffer,unsigned int writeaddr,unsigned short int numbyte);
unsigned int w25q64_read_data(uint32_t addr,uint8_t buff[],uint8_t len);

//传感器
void ADC1_LIGHT_SENSOR_Conifg();
u16 ADC1_get_light_value();
u16 ADC1_get_light_Average_value();

//DMA设置 
void MYDMA_CONFIG(DMA_Stream_TypeDef *DMA_Streamx,u32 channel,u32 des,u32 sedbuf,u16 size);
void myDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 size);
#define SEND_BUFF_SIZE 8200   //8K



#endif 
