#ifndef _BMP280_H
#define _BMP280_H

#include "main.h"

#define BMP280_SLAVE_ADDR		(0x77 << 1)

#define BMP280_CHIPID_REG		0xD0//chip id �Ĵ�����ַ
#define BMP280_CHIPID 			0x58

#define BMP280_CTRL_REG		  0xF4//���ƼĴ�����ַ
#define BMP280_CTRL         0x23//�¶�ʹ�ܣ�����0.005�棬ѹǿ��ʹ�ܣ���ͨ����ģʽ

#define BMP280_TEMP_REG		  0xFA//�¶ȶ�ȡ�Ĵ�����ַ

#define BMP280_CALIB_T1_REG 0x88//У׼���ݵļĴ�����ַ
#define BMP280_CALIB_T2_REG 0x8A
#define BMP280_CALIB_T3_REG 0x8C

void bmp280_init(void);
float bmp280_get_temp(void);

#endif
