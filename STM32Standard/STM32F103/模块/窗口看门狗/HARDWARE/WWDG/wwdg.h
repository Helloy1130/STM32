#ifndef __WDG_H
#define __WDG_H
#include "sys.h"


void WWDG_NVIC_Init(void);
void WWDG_Set_Counter(u8 cnt);
void WWDG_Init(u8 tr,u8 wr,u32 fprer);
//Twwdg=(4096�� 2^WDGTB�� (T[5:0]+1)) /Fpclk1;
//���У�
//Twwdg�� WWDG ��ʱʱ�䣨��λΪ ms��
//Fpclk1�� APB1 ��ʱ��Ƶ�ʣ���λΪ Khz��
//WDGTB�� WWDG ��Ԥ��Ƶϵ��
//T[5:0]�����ڿ��Ź��ļ������� 6 λ

#endif
