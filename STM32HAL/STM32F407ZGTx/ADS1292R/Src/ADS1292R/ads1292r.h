#ifndef __ADS1292R_H
#define __ADS1292R_H

#include "main.h"

////////////////////////////////////////////////////////////////////////////////
//引脚宏定义
#define ADS1292R_DRDY		    HAL_GPIO_ReadPin(ADS1292R_DRDY_GPIO_Port,ADS1292R_DRDY_Pin)
#define ADS1292R_CS_HIGH		HAL_GPIO_WritePin(ADS1292R_CS_GPIO_Port, ADS1292R_CS_Pin,GPIO_PIN_SET)
#define ADS1292R_CS_LOW			HAL_GPIO_WritePin(ADS1292R_CS_GPIO_Port, ADS1292R_CS_Pin,GPIO_PIN_RESET)
#define ADS1292R_PWDN_HIGH		HAL_GPIO_WritePin(ADS1292R_PWDN_GPIO_Port, ADS1292R_PWDN_Pin,GPIO_PIN_SET)
#define ADS1292R_PWDN_LOW		HAL_GPIO_WritePin(ADS1292R_PWDN_GPIO_Port, ADS1292R_PWDN_Pin,GPIO_PIN_RESET)
#define ADS1292R_START_HIGH		HAL_GPIO_WritePin(ADS1292R_START_GPIO_Port, ADS1292R_START_Pin,GPIO_PIN_SET)
#define ADS1292R_START_LOW		HAL_GPIO_WritePin(ADS1292R_START_GPIO_Port, ADS1292R_START_Pin,GPIO_PIN_RESET)
/////////////////////////////////////////////////////////////////////////////////////////////
//声明变量
extern uint8_t ads1292_data_buf[9];	//ads1292数据缓存
extern uint8_t ads1292_recive_flag;	//数据读取完成标志

#define CONFIG_SPI_MASTER_DUMMY   0xFF

// Register Read Commands
#define RREG        0x20		//Read n nnnn registers starting at address r rrrr
//first byte 001r rrrr (2xh)(2) - second byte 000n nnnn(2)
#define WREG        0x40		//Write n nnnn registers starting at address r rrrr
//first byte 010r rrrr (2xh)(2) - second byte 000n nnnn(2)

#define START		0x08		//Start/restart (synchronize) conversions
#define STOP		0x0A		//Stop conversion
#define RDATAC      0x10		//Enable Read Data Continuous mode.

//This mode is the default mode at power-up.
#define SDATAC		0x11		//Stop Read Data Continuously mode
#define RDATA		0x12		//Read data by command; supports multiple read back.

//register address
#define ADS1292_REG_ID			0x00
#define ADS1292_REG_CONFIG1		0x01
#define ADS1292_REG_CONFIG2		0x02
#define ADS1292_REG_LOFF		0x03
#define ADS1292_REG_CH1SET		0x04
#define ADS1292_REG_CH2SET		0x05
#define ADS1292_REG_RLDSENS		0x06
#define ADS1292_REG_LOFFSENS    0x07
#define ADS1292_REG_LOFFSTAT    0x08
#define ADS1292_REG_RESP1	    0x09
#define ADS1292_REG_RESP2	    0x0A

void ads1292_Init(void);

#endif
