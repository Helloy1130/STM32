/*******************************************************************************************
										    声 明
    本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他
	
不可估量的BUG，天际智联不负任何责任。请勿商用！

* 程序版本：V1.01
* 程序日期：2018-8-18
* 程序作者：愤怒的小孩
* 版权所有：西安天际智联信息技术有限公司
*******************************************************************************************/
#ifndef _STRUCTCONFIG_H
#define _STRUCTCONFIG_H

#include <stdio.h>
#include <stdint.h>
#include "main.h"

/************************************************PID参数调试上位机波形显示*************************************************/
/* 如果想观察哪个轴的PID波形，就取消掉哪个注释，但是一次只能取消掉一个注释 */
//#define ROL_PID_DEBUG    
//#define PIT_PID_DEBUG
#define YAW_PID_DEBUG
//#define ALT_PID_DEBUGE
/**************************************************************************************************************************/

/********************************************飞机状态记录组SENSER_OFFSET_FLAG**********************************************/
extern uint8_t   SENSER_OFFSET_FLAG; //标志位组

//每一位对应功能
#define GYRO_OFFSET 0x01 //第一位陀螺仪校准标志位
#define ACC_OFFSET 0x02  //第二位加速度校准标志位
#define BAR_OFFSET 0x04  //第三位气压计校准标志位
#define MAG_OFFSET 0x08  //第四位磁力计校准标志位
#define FLY_ENABLE 0x10  //第五位解锁上锁
#define WiFi_ONOFF 0x20  //第六位WiFi开关
#define FLY_MODE   0x40  //第七位模式选择(0:无头模式(默认) 1:有头模式)

//对 SENSER_OFFSET_FLAG 的位的操作
#define SENSER_FLAG_SET(FLAG)   SENSER_OFFSET_FLAG|=FLAG                //标志位置1
#define SENSER_FLAG_RESET(FLAG) SENSER_OFFSET_FLAG&=~FLAG               //标志位值0
#define GET_FLAG(FLAG)         (SENSER_OFFSET_FLAG&FLAG)==FLAG ? 1 : 0  //获取标志位状态

/**************************************************************************************************************************/

//数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
#define Byte0(data)       ( *( (char *)(&data)	  ) )
#define Byte1(data)       ( *( (char *)(&data) + 1) )
#define Byte2(data)       ( *( (char *)(&data) + 2) )
#define Byte3(data)       ( *( (char *)(&data) + 3) )

//三轴整型（MPU6050原始数据）
typedef struct
{
	int16_t X;
	int16_t Y;
	int16_t Z;
}INT16_XYZ;

//三轴浮点型
typedef struct
{
	float X;
	float Y;
	float Z;
}FLOAT_XYZ;

//姿态解算后的角度
typedef struct
{
	float rol;
	float pit;
	float yaw;
}FLOAT_ANGLE;

//遥控器的数据结构 
typedef struct
{
	int16_t ROLL;
	int16_t PITCH;
	int16_t THROTTLE;
	int16_t YAW;
}RC_TYPE;

//FBM32的数据结构 
typedef struct FBMTYPE
{
  int32_t  ADPress;
  int32_t  ADTemp;
  float  RP;
  float  RT;
  float  RPFilter;
  float  InitPress;
  float  Altitude;
  float  AltitudeFilter;
  uint16_t C0, C1, C2, C3, C6, C8, C9, C10, C11, C12;
  uint32_t C4, C5, C7;
	
} FBMTYPE;

//高度融合的数据结构
typedef struct NAV_tt
{
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
    float ax;
    float ay;
    float az;
} nav_t;

//PID算法的数据结构 
typedef struct PID
{
  float P;         //参数
  float I;
  float D;
  float Error;     //比例项
  float Integral;  //积分项
  float Differ;    //微分项
  float PreError;
  float PrePreError;
  float Ilimit; 
  float Irang;
  float Pout;
  float Iout;
  float Dout;
  float OutPut;   
  uint8_t Ilimit_flag;    //积分分离	
}PID_TYPE;   

//保存的参数数据结构 
typedef struct PIDSave
{
	//陀螺仪校准数据 
	uint16_t  ACC_OFFSET_X;
	uint16_t  ACC_OFFSET_Y;
	uint16_t  ACC_OFFSET_Z;
	uint16_t  GYRO_OFFSET_X;
	uint16_t  GYRO_OFFSET_Y;
	uint16_t  GYRO_OFFSET_Z;
	//角度环 
	uint16_t  ROL_Angle_P;
	uint16_t  ROL_Angle_I;
	uint16_t  ROL_Angle_D;
	uint16_t  PIT_Angle_P;
	uint16_t  PIT_Angle_I;
	uint16_t  PIT_Angle_D;
	uint16_t  YAW_Angle_P;
	uint16_t  YAW_Angle_I;
	uint16_t  YAW_Angle_D;
	//角速度环 
	uint16_t  ROL_Rate_P;
	uint16_t  ROL_Rate_I;
	uint16_t  ROL_Rate_D;
	uint16_t  PIT_Rate_P;
	uint16_t  PIT_Rate_I;
	uint16_t  PIT_Rate_D;
	uint16_t  YAW_Rate_P;
	uint16_t  YAW_Rate_I;
	uint16_t  YAW_Rate_D;
	//高度环
	uint16_t  ALT_Rate_P;
	uint16_t  ALT_Rate_I;
	uint16_t  ALT_Rate_D;
	uint16_t  ALT_P;
	uint16_t  ALT_I;
	uint16_t  ALT_D;
	uint16_t  SI24R1addr;

}PID_SAVE;

//电池电压管理数据结构 
typedef struct BATT_TYPE
{
	float BattAdc;
	float BattRealV;
	float BattMeasureV;
	float BattAlarmV;
	float BattFullV;
}BATT_TYPE;

//姿态解算
extern FLOAT_XYZ 	 Gyr_rad;
extern FLOAT_ANGLE   Att_Angle;
//遥控数据缓存
extern float Moto_PWM_1,Moto_PWM_2,Moto_PWM_3,Moto_PWM_4;
//角度环PID
extern PID_TYPE PID_ROL_Angle;
extern PID_TYPE PID_PIT_Angle;
extern PID_TYPE PID_YAW_Angle;
//角速度环PID
extern PID_TYPE PID_ROL_Rate;
extern PID_TYPE PID_PIT_Rate;
extern PID_TYPE PID_YAW_Rate;
//参数保存
extern RC_TYPE  RC_Control;
//功能键
extern uint8_t Airplane_Enable;

#endif
