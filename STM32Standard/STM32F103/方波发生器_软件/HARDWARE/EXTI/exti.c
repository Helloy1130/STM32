#include "exti.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"

extern u16 freq_ctr1;
extern u16 freq_ctr2;
extern float freq;
extern u8 flag;
extern u8 change;

//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//�ⲿ�жϣ���Ҫʹ�� AFIO ʱ��
	
	KEY_Init();			//��ʼ��������Ӧ io ģʽ
	 
	//GPIOC.5  �ж����Լ��жϳ�ʼ������	KEY0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);				//���� EXTI_InitStructure ��ָ���Ĳ�����ʼ������ EXTI �Ĵ���
	
	//GPIOA.15 �ж����Լ��жϳ�ʼ������	KEY1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);				//���� EXTI_InitStructure ��ָ���Ĳ�����ʼ������ EXTI �Ĵ���
	
	//GPIOA.0 �ж����Լ��жϳ�ʼ������	WK_UP
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);				//���� EXTI_InitStructure ��ָ���Ĳ�����ʼ������ EXTI �Ĵ���
	
	//�����жϵķ��飬��ʹ���ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;				//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 	//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 			//�����ȼ� 1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//ʹ���ⲿ�ж�ͨ��0
	NVIC_Init(&NVIC_InitStructure);				//���� NVIC_InitStructure ��ָ���Ĳ�����ʼ������ NVIC �Ĵ���
	
	//�����жϵķ��飬��ʹ���ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��9_5_
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 	//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 			//�����ȼ� 1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//ʹ���ⲿ�ж�ͨ��0
	NVIC_Init(&NVIC_InitStructure);				//���� NVIC_InitStructure ��ָ���Ĳ�����ʼ������ NVIC �Ĵ���
	
	//�����жϵķ��飬��ʹ���ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��15_10_
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 	//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; 			//�����ȼ� 1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//ʹ���ⲿ�ж�ͨ��0
	NVIC_Init(&NVIC_InitStructure);				//���� NVIC_InitStructure ��ָ���Ĳ�����ʼ������ NVIC �Ĵ���
	
}


//�жϺ���
//�ⲿ�ж�0�ķ�����������WK_UP�������жϼ��
void EXTI0_IRQHandler(void)
{ 
	delay_ms(10);//����
	if(WK_UP==1)
	{	
		if(flag==1||flag==2)//���������ǲ�
		{
			if(freq_ctr1 > 24)//һֱ���ӵ�10k
			{
				freq_ctr1--;
			}
			else
			{
				freq_ctr1 = 49;
			}
			freq = 250000.0 / (freq_ctr1 + 1);
			TIM2_Int_Init(freq_ctr1, 2);
		}
		if(flag==3||flag==4)//���Ҳ� ����
		{
			if(freq_ctr2 > 13)//һֱ���ӵ�50k
			{
				freq_ctr2--;
			}
			else
			{
				freq_ctr2 = 29;
			}
			freq = 750000.0 / (freq_ctr2 + 1);
			TIM4_Int_Init(freq_ctr2, 0);
		}
		//freq_ctr2 = (freq_ctr1+1)*3/5 - 1;
		//TIM4_Int_Init(freq_ctr2, 0);
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //��� EXTI0 ��·�ϵ��жϱ�־λ
}

//�ⲿ�ж�9_5_�ķ�����������KEY0�������жϼ��
void EXTI9_5_IRQHandler(void)
{ 
	delay_ms(10);//����
	if(KEY0==0)
	{
		if(flag==1||flag==2)//���������ǲ�
		{
			if(freq_ctr1 < 49)//һֱ���ٵ�5k
			{
				freq_ctr1++;
			}
			else
			{
				freq_ctr1 = 24;
			}
			freq = 250000.0 / (freq_ctr1 + 1);
			TIM2_Int_Init(freq_ctr1, 2);
		}
		if(flag==3||flag==4)//���Ҳ� ����
		{
			if(freq_ctr2 < 29)//һֱ���ٵ�25k
			{
				freq_ctr2++;
			}
			else
			{
				freq_ctr2 = 13;
			}
			freq = 750000.0 / (freq_ctr2 + 1);
			TIM4_Int_Init(freq_ctr2, 0);
		}
		//freq_ctr2 = (freq_ctr1+1)*3/5 - 1;
		//TIM4_Int_Init(freq_ctr2, 0);
	}
	EXTI_ClearITPendingBit(EXTI_Line5); //��� EXTI0 ��·�ϵ��жϱ�־λ
}

//�ⲿ�ж�15_10_�ķ�����������KEY1�������жϼ��
void EXTI15_10_IRQHandler(void)									//����KEY1����������
{ 
	delay_ms(10);//����
	if(KEY1==0)
	{
		if(flag==4) flag = 1;
		else flag++;
		if(flag==1)//�������
		{
			freq_ctr1 = 49;
			freq = 250000.0 / (freq_ctr1 + 1);//5k
			TIM2_Int_Init(freq_ctr1, 2);//TIM2�жϳ�ʼ�� 5k
			LCD_ShowString(60,60,200,16,16,"Wave: square in ");
		}
		else if(flag==2)//���ǲ����
		{
			LCD_ShowString(60,60,200,16,16,"Wave: triangle  ");
		}
		else if(flag==3)//���Ҳ����
		{
			LCD_ShowString(60,60,200,16,16,"Wave: sin       ");
			freq_ctr2 = 29;
			freq = 750000.0 / (freq_ctr2 + 1);//25k
			TIM4_Int_Init(freq_ctr2, 0);//TIM4�жϳ�ʼ�� 25k
		}
		else if(flag==4)//�������
		{
			LCD_ShowString(60,60,200,16,16,"Wave: square out");
		}
		change = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line15); //��� EXTI15 ��·�ϵ��жϱ�־λ
}



