#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "pwm.h"
#include "led.h"
#include "exti.h"
#include "timer.h"

//WK_UP -- Ƶ�ʵ���		KEY0 -- Ƶ�ʵݼ�
u16 PWM_frequency;	//PWMƵ��
u16 PWM_cnt = 899;

int main(void)
{			
	u8 dir;				//���Ʒ��� 1:�𽥱��� 0���𽥱䰵
	u16 led0pwmval = 0;	//����ռ�ձ�
	
	delay_init(); 		//��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //���� NVIC �жϷ��� 2:2 λ��ռ���ȼ��� 2 λ��Ӧ���ȼ�
	
	LED_Init(); 		//LEDIO��ʼ��
	TIM1_PWM_Init(PWM_cnt,0);//����Ƶ PWMƵ��=72000KHZ/(899+1)=80KHz   ��ʼ״̬
	PWM_frequency = 72000/(PWM_cnt+1);//80KHz
	
	uart_init(9600);    //���ڳ�ʼ��Ϊ 9600
	TIM3_Int_Init(9999, 7199);//TIM3�жϳ�ʼ�� 10KHz  ����0-9999   10000/10kHz=1s
	
	EXTIX_Init();		//�ⲿ�жϳ�ʼ��
	
	while(1)
	{
		delay_ms(10);
		if(dir) led0pwmval++;
		else led0pwmval--;
		if(led0pwmval>=PWM_cnt-600) dir = 0;
		if(led0pwmval==0) dir = 1;
		TIM_SetCompare1(TIM1,led0pwmval);//�޸�PWM�����ռ�ձ�
	}		
}



