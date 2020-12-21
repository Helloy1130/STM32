#ifndef __WDG_H
#define __WDG_H
#include "sys.h"


void WWDG_NVIC_Init(void);
void WWDG_Set_Counter(u8 cnt);
void WWDG_Init(u8 tr,u8 wr,u32 fprer);
//Twwdg=(4096× 2^WDGTB× (T[5:0]+1)) /Fpclk1;
//其中：
//Twwdg： WWDG 超时时间（单位为 ms）
//Fpclk1： APB1 的时钟频率（单位为 Khz）
//WDGTB： WWDG 的预分频系数
//T[5:0]：窗口看门狗的计数器低 6 位

#endif
