#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

void TIM2_Int_Init(u16 arr, u16 psc);//TIM3�жϳ�ʼ�� arr:�Զ���װֵ	psc��ʱ��Ԥ��Ƶ��	TIM3 ��ʱ��Ϊ 72M
//Tout= ((arr+1)*(psc+1))/Tclk��
//Tclk�� TIM3 ������ʱ��Ƶ�ʣ���λΪ Mhz����
//Tout�� TIM3 ���ʱ�䣨��λΪ us����

#endif
