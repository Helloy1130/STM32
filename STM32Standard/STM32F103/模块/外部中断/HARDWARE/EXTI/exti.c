#include "exti.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

u8 flag;

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
		flag = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //��� EXTI0 ��·�ϵ��жϱ�־λ
}

//�ⲿ�ж�9_5_�ķ�����������KEY0�������жϼ��
void EXTI9_5_IRQHandler(void)
{ 
	delay_ms(10);//����
	if(KEY0==1)
	{
		flag = 3;
	}
	EXTI_ClearITPendingBit(EXTI_Line5); //��� EXTI0 ��·�ϵ��жϱ�־λ
}

//�ⲿ�ж�15_10_�ķ�����������KEY1�������жϼ��
void EXTI15_10_IRQHandler(void)
{ 
	delay_ms(10);//����
	if(KEY1==0)
	{
		flag = 2;
	}
	EXTI_ClearITPendingBit(EXTI_Line15); //��� EXTI15 ��·�ϵ��жϱ�־λ
}



