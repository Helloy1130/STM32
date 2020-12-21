/*******************************************************************************************
										    声 明
    本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他
	
不可估量的BUG，天际智联不负任何责任。请勿商用！

* 程序版本：V1.01
* 程序日期：2018-8-18
* 程序作者：愤怒的小孩
* 版权所有：西安天际智联信息技术有限公司
*******************************************************************************************/
#include <math.h>
#include "control.h"
#include "pid.h"
#include "JY901.h"
#include "tim.h"

RC_TYPE  RC_Control;
//角度环PID 
PID_TYPE PID_ROL_Angle;
PID_TYPE PID_PIT_Angle;
PID_TYPE PID_YAW_Angle;
//角速度环PID 
PID_TYPE PID_ROL_Rate;
PID_TYPE PID_PIT_Rate;
PID_TYPE PID_YAW_Rate;

float Moto_PWM_1=0.0f,Moto_PWM_2=0.0f,Moto_PWM_3=0.0f,Moto_PWM_4=0.0f;
uint8_t Airplane_Enable = 1;

/******************************************************************************************
*函  数：void Control(FLOAT_ANGLE *att_in,FLOAT_XYZ *gyr_in, RC_TYPE *rc_in, uint8_t armed)
*功  能：姿态控制,角度环控制和角速度环控制
*参  数：att_in：测量值
*        gry_in: MPU6050读取的角速度值
*        rc_in : 遥控器设定值
*        armed记录命令
*返回值：无
*备  注：RoboFly 小四轴机头与电机示意图	
					 机头(Y+)
					   
				  M1    ↑    M2
					\   |   /
					 \  |  /
					  \ | /
			    ————————+————————>X+	
					  / | \
					 /  |  \
					/   |   \
				  M4    |    M3

	
	1. M1 M3电机逆时针旋转，M2 M4电机顺时针旋转
	2. X:是MPU6050的 X 轴，Y:是MPU6050的 Y 轴，Z轴正方向垂直 X-Y 面，竖直向上
	3. 绕 X 轴旋转为PITCH 角 
	   绕 Y 轴旋转为 ROLL 角 
	   绕 Z 轴旋转为 YAW  角
	4. 自己DIY时进行动力分配可以一个轴一个轴的分配，切勿三个轴同时分配。
*******************************************************************************************/
void Control(FLOAT_ANGLE *att_in,FLOAT_XYZ *gyr_in, RC_TYPE *rc_in)
{
	FLOAT_ANGLE Measure_Angle,Target_Angle;
	Measure_Angle.rol = att_in->rol; 
	Measure_Angle.pit = att_in->pit; 
	Measure_Angle.yaw = att_in->yaw; 
	Target_Angle.rol = (float)((rc_in->ROLL-1500)/12.0f);
	Target_Angle.pit = (float)((rc_in->PITCH-1500)/12.0f);
//	Target_Angle.yaw = (float)((1500-rc_in->YAW)/12.0f);
  
//	if(!SI24R1_Controlflag)
//	{
//		Target_Angle.yaw = 0;
//	}else if(fabs(Target_Angle.yaw )<4)
//	{
//		Target_Angle.yaw = 0;
//	}
//	if(1 == (GET_FLAG(FLY_MODE))) //无头模式
//	{
		Yaw_Carefree(&Target_Angle,&Measure_Angle);
//	}

	//角度环
	PID_Postion_Cal(&PID_ROL_Angle,Target_Angle.rol,Measure_Angle.rol);//ROLL角度环PID （输入角度 输出角速度）
	PID_Postion_Cal(&PID_PIT_Angle,Target_Angle.pit,Measure_Angle.pit);//PITH角度环PID （输入角度 输出角速度）
//	PID_Postion_Cal(&PID_YAW_Angle,Target_Angle.yaw,Measure_Angle.yaw);//YAW角度环PID  （输入角度 输出角速度）
	
	//角速度环
	PID_Postion_Cal(&PID_ROL_Rate,PID_ROL_Angle.OutPut,(gyr_in->Y*RadtoDeg)); //ROLL角速度环PID （输入角度环的输出，输出电机控制量）
	PID_Postion_Cal(&PID_PIT_Rate,PID_PIT_Angle.OutPut,-(gyr_in->X*RadtoDeg)); //PITH角速度环PID （输入角度环的输出，输出电机控制量）
	PID_Postion_Cal(&PID_YAW_Rate,Target_Angle.yaw*PID_YAW_Angle.P,gyr_in->Z*RadtoDeg); //YAW角速度环PID （输入角度，输出电机控制量）

//    Moto_PWM_1 = rc_in->THROTTLE - PID_ROL_Rate.OutPut;
//    Moto_PWM_2 = rc_in->THROTTLE + PID_ROL_Rate.OutPut;
//    Moto_PWM_3 = rc_in->THROTTLE - PID_ROL_Rate.OutPut;
//    Moto_PWM_4 = rc_in->THROTTLE + PID_ROL_Rate.OutPut;

//    Moto_PWM_1 = rc_in->THROTTLE + PID_PIT_Rate.OutPut;
//    Moto_PWM_2 = rc_in->THROTTLE + PID_PIT_Rate.OutPut;
//    Moto_PWM_3 = rc_in->THROTTLE - PID_PIT_Rate.OutPut;
//    Moto_PWM_4 = rc_in->THROTTLE - PID_PIT_Rate.OutPut;

    Moto_PWM_1 = rc_in->THROTTLE + PID_PIT_Rate.OutPut - PID_ROL_Rate.OutPut;
    Moto_PWM_2 = rc_in->THROTTLE + PID_PIT_Rate.OutPut + PID_ROL_Rate.OutPut;
    Moto_PWM_3 = rc_in->THROTTLE - PID_PIT_Rate.OutPut - PID_ROL_Rate.OutPut;
    Moto_PWM_4 = rc_in->THROTTLE - PID_PIT_Rate.OutPut + PID_ROL_Rate.OutPut;


//	//动力分配（自己DIY是动力分配一定要好好研究，动力分配搞错飞机肯定飞不起来!!!）
//	if(rc_in->THROTTLE>180&&armed)//当油门大于150时和飞机解锁时动力分配才生效
//	{
//		Moto_PWM_1 = rc_in->THROTTLE + PID_ROL_Rate.OutPut - PID_PIT_Rate.OutPut - PID_YAW_Rate.OutPut;
//		Moto_PWM_2 = rc_in->THROTTLE - PID_ROL_Rate.OutPut - PID_PIT_Rate.OutPut + PID_YAW_Rate.OutPut;
//		Moto_PWM_3 = rc_in->THROTTLE - PID_ROL_Rate.OutPut + PID_PIT_Rate.OutPut - PID_YAW_Rate.OutPut;
//		Moto_PWM_4 = rc_in->THROTTLE + PID_ROL_Rate.OutPut + PID_PIT_Rate.OutPut + PID_YAW_Rate.OutPut;
//	}
//	else
//	{
//		Moto_PWM_1 = 0;
//		Moto_PWM_2 = 0;
//		Moto_PWM_3 = 0;
//		Moto_PWM_4 = 0;
//	}

    int threshold = 72;

	if (Moto_PWM_1 > threshold){
        Moto_PWM_1 = threshold;
	} else if(Moto_PWM_1 <= 0) {
	    Moto_PWM_1 = 0;
	}
    if (Moto_PWM_2 > threshold){
        Moto_PWM_2 = threshold;
    } else if(Moto_PWM_2 <= 0) {
        Moto_PWM_2 = 0;
    }
    if (Moto_PWM_3 > threshold){
        Moto_PWM_3 = threshold;
    } else if(Moto_PWM_3 <= 0) {
        Moto_PWM_3 = 0;
    }
    if (Moto_PWM_4 > threshold){
        Moto_PWM_4 = threshold;
    } else if(Moto_PWM_4 <= 0) {
        Moto_PWM_4 = 0;
    }

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (int)Moto_PWM_1);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (int)Moto_PWM_2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, (int)Moto_PWM_3);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, (int)Moto_PWM_4);
}

/******************************************************************************************
* 函  数：void Yaw_Carefree(FLOAT_ANGLE *Target_Angle, const FLOAT_ANGLE *Measure_Angle)
* 功  能：无头角度控制
* 参  数：*Target_Angle 指向目标姿态角的指针
*         *Measure_Angle 测量姿态角的指针
* 返回值：针对目标航向角计算出的航向角
* 备  注：无头模式需调用此函数
*******************************************************************************************/
void Yaw_Carefree(FLOAT_ANGLE *Target_Angle, const FLOAT_ANGLE *Measure_Angle)
{
	float yawRad = fabs(Measure_Angle->yaw) * DegtoRad;
	float cosy = cosf(yawRad);
	float siny = sinf(yawRad);
	float originalRoll = Target_Angle->rol;
	float originalPitch = Target_Angle->pit;

	Target_Angle->rol = originalRoll * cosy + originalPitch * siny;
	Target_Angle->pit = originalPitch * cosy - originalRoll * siny;
}

