#include "sys.h"
#include "led.h"
#include "key.h"
#include "delay.h"

//��������LED����
int main(void)
{
	delay_init();					//��ʱ������ʼ��
	LED_Init();						//LEDIO��ʼ��
	KEY_Init();						//����IO��ʼ��
	LED0 = 1;
	while(1)
	{
		if(KEY_Scan(0)==WK_UP_PRES)	//�ж�KEY0����
		{
			LED0 = !LED0;
		}	
	}
}
