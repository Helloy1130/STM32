#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

#define SPI_SCLK       	PEout(2)
#define SPI_CS         	PEout(3)
#define SPI_MOSI       	PEout(4)
#define SPI_MISO		PEout(5)

void SPI1_Init(void);			 //初始化SPI1口
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI1速度
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1总线读写一个字节

//软件SPI
void MySPI_Init(void);//初始化
u8 MySPI_ReadByte(void);//读数据 返回值若为开始标志0x11,则开始读数据
void MySPI_SendByte(u8 data);//写数据


#endif

