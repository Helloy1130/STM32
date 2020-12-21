#ifndef _BMP280_H
#define _BMP280_H

#include "main.h"

#define BMP280_SLAVE_ADDR		(0x77 << 1)

#define BMP280_CHIPID_REG		0xD0//chip id 寄存器地址
#define BMP280_CHIPID 			0x58

#define BMP280_CTRL_REG		  0xF4//控制寄存器地址
#define BMP280_CTRL         0x23//温度使能，精度0.005℃，压强不使能，普通工作模式

#define BMP280_TEMP_REG		  0xFA//温度读取寄存器地址

#define BMP280_CALIB_T1_REG 0x88//校准数据的寄存器地址
#define BMP280_CALIB_T2_REG 0x8A
#define BMP280_CALIB_T3_REG 0x8C

void bmp280_init(void);
float bmp280_get_temp(void);

#endif
