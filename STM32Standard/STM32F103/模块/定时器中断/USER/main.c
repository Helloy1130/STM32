#include "sys.h"
#include "delay.h"
#include "led.h"
#include "timer.h"

//��ʱ��3�жϣ�������LED1���࣬���1s

int main(void)
{		
	delay_init(); //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //���� NVIC �жϷ��� 2:2 λ��ռ���ȼ��� 2 λ��Ӧ���ȼ�
	
	LED_Init(); //LED �˿ڳ�ʼ��
	TIM3_Int_Init(4999, 7199);//TIM3�жϳ�ʼ�� 10KHz  ����0-4999   5000/10kHz=0.5s   ���1s��˸
	
	while(1);

}
