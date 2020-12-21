#include "sys.h"
#include "delay.h"
#include "wwdg.h"
#include "led.h"


//ͨ�� DS0 ��ָʾ STM32 �Ƿ񱻸�λ�ˣ��������λ�˾ͻ���� 300ms�� DS1 ����ָʾ��
//��ι����ÿ���ж�ι����תһ�Ρ�

int main(void)
{		
	delay_init();	    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	
	LED_Init(); 
	LED0=0;
	delay_ms(300);
	WWDG_Init(0X7F,0X5F,WWDG_Prescaler_8);
	
	while(1)
	{
		LED0=1;
	}
}
