#ifndef _KEY_H
#define _KEY_H

#include "sys.h"
//���������˿ڶ���
#define KEY1 PAin(15)	
#define KEY0 PCin(5) 
#define WK_UP PAin(0)

#define KEY0_PRES	1	//KEY0����		
#define KEY1_PRES	2	//KEY1����
#define WK_UP_PRES	3	//WK_UP����

void KEY_Init(void);	//����IO��ʼ��
//����ɨ�躯�� mode:0,��֧��������;1,֧��������;
//����ֵ��
//0��û���κΰ�������
//KEY0_PRES�� KEY0 ����
//KEY1_PRES�� KEY1 ����
//WKUP_PRES�� WK_UP ����
u8 KEY_Scan(u8 mode);

#endif
