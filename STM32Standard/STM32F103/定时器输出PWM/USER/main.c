#include "sys.h"
#include "delay.h"
#include "pwm.h"
#include "led.h"
#include "exti.h"
#include "usart.h"
#include "lcd.h"

//WK_UP -- Ƶ�ʵ���		KEY0 -- Ƶ�ʵݼ�

extern u16 PWM_cnt;

int main(void)
{
	u8 dir;				//���Ʒ��� 1:�𽥱��� 0���𽥱䰵
	u16 led0pwmval = 0;	//����ռ�ձ�
	
	delay_init(); 		//��ʱ������ʼ��
	uart_init(9600);    //���ڳ�ʼ��Ϊ 9600
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //���� NVIC �жϷ��� 2:2 λ��ռ���ȼ��� 2 λ��Ӧ���ȼ�

	LCD_Init();
	EXTIX_Init();		
	LED_Init(); 		//LEDIO��ʼ��
	TIM1_PWM_Init(899,0);//����Ƶ PWMƵ��=72000KHZ/(899+1)=80KHz 
	
	while(1)
	{
		delay_ms(10);
		if(dir) led0pwmval++;
		else led0pwmval--;
		if(led0pwmval>=PWM_cnt - 600) dir = 0;
		if(led0pwmval==0) dir = 1;
		
		TIM_SetCompare1(TIM1,led0pwmval);//�޸�PWM�����ռ�ձ�	
	}
		
}



