#ifndef _IWDG_H
#define _IWDG_H

#include "sys.h"

void IWDG_Init(u8 prer,u16 rlr);
//Tout=((4�� 2^prer) �� rlr) /40
//���� Tout Ϊ���Ź����ʱ�䣨��λΪ ms���� prer Ϊ���Ź�ʱ��Ԥ��Ƶֵ��IWDG_PR ֵ����
//��ΧΪ 0~7�� rlr Ϊ���Ź�����װ��ֵ��IWDG_RLR ��ֵ����
void IWDG_Feed(void);//ι�������Ź�

#endif
