#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
			  	    													  
void SPI1_Init(void);			 //初始化SPI1口
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI1速度   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1总线读写一个字节

//软件SPI
#define SPI_SCK			PAout(7)
#define SPI_CS			PAout(5)
//#define SPI_MOSI		
#define SPI_MISO		PAin(0)

void SPI3_Init(void);
u8 SPI3_Read_Byte(void);

//u8 SPI3_ReadWriteByte(u8 TxData);

#endif

