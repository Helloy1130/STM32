#include "sys.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "delay.h"

//�ⲿ�жϣ�������������Ƶ��ٶ�
int main(void)
{
	delay_init();					//��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����жϷ���
	LED_Init();						//LEDIO��ʼ��
	EXTIX_Init();					//�ⲿ�жϳ�ʼ��
	LED0 = 1;
	
	while(1)
	{
		if(flag==0)
		{
			LED0 = 0;
			LED1 = 1;
			delay_ms(300);
			LED0 = 1;
			LED1 = 0;
			delay_ms(300);
		}
		if(flag==1)
		{
			LED0 = 0;
			LED1 = 1;
			delay_ms(500);
			LED0 = 1;
			LED1 = 0;
			delay_ms(500);
		}
		if(flag==2)
		{
			LED0 = 0;
			LED1 = 1;
			delay_ms(300);
			LED0 = 1;
			LED1 = 0;
			delay_ms(300);
		}
		if(flag==3)
		{
			LED0 = 0;
			LED1 = 1;
			delay_ms(100);
			LED0 = 1;
			LED1 = 0;
			delay_ms(100);
		}
		
	}
}




