#ifndef _LED_H
#define _LED_H

#include "sys.h"
//LED�˿ڶ���
#define LED0 PAout(8)	//PA8
#define LED1 PDout(2) 	//PD2

void LED_Init(void);	//LEDIO��ʼ��
	
#endif
