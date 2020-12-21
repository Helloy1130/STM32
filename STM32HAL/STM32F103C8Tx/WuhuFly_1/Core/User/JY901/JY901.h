#ifndef WUHUFLY_JY901_H
#define WUHUFLY_JY901_H

#include <stdbool.h>
#include "main.h"

#define G			9.80665f		      	// m/s^2
#define RadtoDeg    57.324841f				//弧度到角度 (弧度 * 180/3.1415)
#define DegtoRad    0.0174533f				//角度到弧度 (角度 * 3.1415/180)

#define JY_SAVE         0x00
#define JY_CALSW        0x01
#define JY_RSW          0x02
#define JY_RRATE        0x03
#define JY_BAUD         0x04
#define JY_AXOFFSET     0x05
#define JY_AYOFFSET     0x06
#define JY_AZOFFSET     0x07
#define JY_GXOFFSET     0x08
#define JY_GYOFFSET     0x09
#define JY_GZOFFSET     0x0a
#define JY_HXOFFSET     0x0b
#define JY_HYOFFSET     0x0c
#define JY_HZOFFSET     0x0d
#define JY_D0MODE       0x0e
#define JY_D1MODE       0x0f
#define JY_D2MODE       0x10
#define JY_D3MODE       0x11
#define JY_D0PWMH       0x12
#define JY_D1PWMH       0x13
#define JY_D2PWMH       0x14
#define JY_D3PWMH       0x15
#define JY_D0PWMT       0x16
#define JY_D1PWMT       0x17
#define JY_D2PWMT       0x18
#define JY_D3PWMT       0x19
#define JY_IICADDR      0x1a
#define JY_LEDOFF       0x1b
#define JY_GPSBAUD      0x1c

#define JY_YYMM         0x30
#define JY_DDHH         0x31
#define JY_MMSS         0x32
#define JY_MS           0x33
#define JY_AX           0x34
#define JY_AY           0x35
#define JY_AZ           0x36
#define JY_GX           0x37
#define JY_GY           0x38
#define JY_GZ           0x39
#define JY_HX           0x3a
#define JY_HY           0x3b
#define JY_HZ           0x3c
#define JY_Roll         0x3d
#define JY_Pitch        0x3e
#define JY_Yaw          0x3f
#define JY_TEMP         0x40
#define JY_D0Status     0x41
#define JY_D1Status     0x42
#define JY_D2Status     0x43
#define JY_D3Status     0x44
#define JY_PressureL    0x45
#define JY_PressureH    0x46
#define JY_HeightL      0x47
#define JY_HeightH      0x48
#define JY_LonL         0x49
#define JY_LonH         0x4a
#define JY_LatL         0x4b
#define JY_LatH         0x4c
#define JY_GPSHeight    0x4d
#define JY_GPSYAW       0x4e
#define JY_GPSVL        0x4f
#define JY_GPSVH        0x50

#define JY_DIO_MODE_AIN     0
#define JY_DIO_MODE_DIN     1
#define JY_DIO_MODE_DOH     2
#define JY_DIO_MODE_DOL     3
#define JY_DIO_MODE_DOPWM   4
#define JY_DIO_MODE_GPS     5

extern bool JY901_recv_flag;
extern const uint8_t JY901_save_conf_cmd[5];
extern const uint8_t JY901_imu_cali_cmd[5];
extern const uint8_t JY901_mag_cali_cmd[5];
extern const uint8_t JY901_quit_cali_cmd[5];

void JY901_startIIC(uint8_t address);     //设定0x50地址的IIC初始化
bool JY901_readSerialData(uint8_t data);  //处理接收的数据
bool JY901_receiveSerialData(void);       //接收串口数据
void JY901_readData(uint8_t address,      //address地址
                    uint8_t length,       //长度length
                    uint8_t data[]);      //手动读取ucLength长度的数据
uint16_t JY901_getTime(const char *);     //获取时间，'Y'年，'M'月，'D'天，'h'时，'m'分，'s'秒，'l'毫秒
double JY901_getAccX(void);       //获取加速度
double JY901_getAccY(void);       //获取加速度
double JY901_getAccZ(void);       //获取加速度
double JY901_getGyroX(void);      //获取角速度
double JY901_getGyroY(void);      //获取角速度
double JY901_getGyroZ(void);      //获取角速度
double JY901_getMagX(void);       //获取磁场
double JY901_getMagY(void);       //获取磁场
double JY901_getMagZ(void);       //获取磁场
int16_t JY901_getAccRawX(void);   //获取加速度计原始数据
int16_t JY901_getAccRawY(void);   //获取加速度计原始数据
int16_t JY901_getAccRawZ(void);   //获取加速度计原始数据
int16_t JY901_getGyroRawX(void);  //获取陀螺仪原始数据
int16_t JY901_getGyroRawY(void);  //获取陀螺仪原始数据
int16_t JY901_getGyroRawZ(void);  //获取陀螺仪原始数据
int16_t JY901_getMagRawX(void);   //获取磁力计原始数据
int16_t JY901_getMagRawY(void);   //获取磁力计原始数据
int16_t JY901_getMagRawZ(void);   //获取磁力计原始数据

double JY901_getRoll(void);
double JY901_getPitch(void);
double JY901_getYaw(void);

int32_t JY901_getPressure(void);  //获取压力
int32_t JY901_getAltitude(void);  //获取高度
int16_t JY901_getD0Status(void);  //获取端口输出
int16_t JY901_getD1Status(void);  //获取端口输出
int16_t JY901_getD2Status(void);  //获取端口输出
int16_t JY901_getD3Status(void);  //获取端口输出
int32_t JY901_getLon(void);       //获取经度
int32_t JY901_getLat(void);       //获取纬度
double JY901_getGPSH(void);       //GPS 高度
double JY901_getGPSY(void);       //GPS 航向角
double JY901_getGPSV(void);       //GPS 地速
void JY901_saveConf(void);        //保存配置
void JY901_quitCali(void);        //退出校准
void JY901_caliIMU(void);         //IMU校准
void JY901_caliMag(void);         //磁力计校准
unsigned long JY901_getLastTime(void);

#endif //WUHUFLY_JY901_H
