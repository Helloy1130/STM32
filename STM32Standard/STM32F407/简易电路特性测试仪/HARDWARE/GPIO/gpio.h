#ifndef _GPIO_H
#define _GPIO_H
#include "sys.h"


#define startSignal	PEout(6)//��FPGA���ӣ��ߵ�ƽ��Ч
#define Relay_inout	PAout(4)//�̵������Ʋ�����������͵�ƽ���룬�ߵ�ƽ���
#define Relay_RS	PAout(5)//�̵������Ʋ������裬�͵�ƽ�Ӳ������裬�ߵ�ƽ����
#define Relay_RL	PAout(6)//�̵������Ƹ��أ��͵�ƽ���أ��ߵ�ƽ����

void myGPIO_init(void);

#endif
