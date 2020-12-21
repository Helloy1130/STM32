#ifndef _LED_H
#define _LED_H

#include "sys.h"
//LED端口定义
#define LED0 PAout(8)	//PA8
#define LED1 PDout(2) 	//PD2

void LED_Init(void);	//LEDIO初始化
	
#endif
