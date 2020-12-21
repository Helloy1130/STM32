#ifndef _EXTI_H
#define _EXTI_H

#include "sys.h"

extern u8 flag;//0表示没有按键按下：保持	1表示WK_UP按下	2表示KEY1按下	3表示KEY0按下

void EXTIX_Init(void);//外部中断初始化

#endif

