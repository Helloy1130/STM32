//
// Created by Y on 2020/10/27.
//

#ifndef WUHUFLY_BMP280_H
#define WUHUFLY_BMP280_H

#include "main.h"

#define BMP280_SLAVE_ADDR		(0x76 << 1)

#define BMP280_CHIPID_REG		0xD0//chip id 寄存器地址
#define BMP280_CHIPID 			0x58

#define BMP280_CTRL_REG		    0xF4//控制寄存器地址
#define BMP280_CTRL             0x3F//温度精度0.005℃，压强精度0.16Pa，普通工作模式

#define BMP280_TEMP_REG		    0xFA//温度读取寄存器地址
#define BMP280_PRESS_REG		0xF8//压强读取寄存器地址

#define BMP280_CALIB_T1_REG     0x88//校准温度数据的寄存器地址
#define BMP280_CALIB_T2_REG     0x8A
#define BMP280_CALIB_T3_REG     0x8C
#define BMP280_CALIB_P1_REG     0x8E//校准压强数据的寄存器地址
#define BMP280_CALIB_P2_REG     0x90
#define BMP280_CALIB_P3_REG     0x92
#define BMP280_CALIB_P4_REG     0x94
#define BMP280_CALIB_P5_REG     0x96
#define BMP280_CALIB_P6_REG     0x98
#define BMP280_CALIB_P7_REG     0x9A
#define BMP280_CALIB_P8_REG     0x9C
#define BMP280_CALIB_P9_REG     0x9E

void bmp280_init(void);
float readTemperature(void);
float readPressure(void);

#endif //WUHUFLY_BMP280_H
