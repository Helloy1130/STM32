#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
	
 	    													  
void SPI1_Init(void);			 //初始化SPI1口
void SPI1_Write_cmd(u8 Cmd);	 //写命令
void SPI1_Write_data(u32 Data);	 //写数据
		 
#endif

