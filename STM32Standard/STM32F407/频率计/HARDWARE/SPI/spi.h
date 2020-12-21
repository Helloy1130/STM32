#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
			  	    													  
void SPI1_Init(void);			 //��ʼ��SPI1��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI1�ٶ�   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1���߶�дһ���ֽ�

//���SPI
#define SPI_SCK			PAout(7)
#define SPI_CS			PAout(5)
//#define SPI_MOSI		
#define SPI_MISO		PAin(0)

void SPI3_Init(void);
u8 SPI3_Read_Byte(void);

//u8 SPI3_ReadWriteByte(u8 TxData);

#endif

