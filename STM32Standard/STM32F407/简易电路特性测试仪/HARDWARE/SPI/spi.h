#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

#define SPI_SCLK       	PEout(2)
#define SPI_CS         	PEout(3)
#define SPI_MOSI       	PEout(4)
#define SPI_MISO		PEout(5)

void SPI1_Init(void);			 //��ʼ��SPI1��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI1�ٶ�
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1���߶�дһ���ֽ�

//���SPI
void MySPI_Init(void);//��ʼ��
u8 MySPI_ReadByte(void);//������ ����ֵ��Ϊ��ʼ��־0x11,��ʼ������
void MySPI_SendByte(u8 data);//д����


#endif

