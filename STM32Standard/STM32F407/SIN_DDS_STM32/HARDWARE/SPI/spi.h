#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
	
 	    													  
void SPI1_Init(void);			 //��ʼ��SPI1��
void SPI1_Write_cmd(u8 Cmd);	 //д����
void SPI1_Write_data(u32 Data);	 //д����
		 
#endif

