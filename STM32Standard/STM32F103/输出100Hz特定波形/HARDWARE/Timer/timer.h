#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

void TIM2_Int_Init(u16 arr, u16 psc);//TIM3中断初始化 arr:自动重装值	psc：时钟预分频数	TIM3 的时钟为 72M
//Tout= ((arr+1)*(psc+1))/Tclk；
//Tclk： TIM3 的输入时钟频率（单位为 Mhz）。
//Tout： TIM3 溢出时间（单位为 us）。

#endif
