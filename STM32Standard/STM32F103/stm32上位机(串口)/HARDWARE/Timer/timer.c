#include "timer.h"

//ͨ�ö�ʱ�� 2 ��ʼ��
//����ʱ��ѡ��Ϊ APB1 �� 2 ������ APB1 Ϊ 36M�����Ϊ72M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��

void TIM2_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʱ�� TIM2 ʹ��
	
	//��ʱ�� TIM2 ��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //��ʼ�� TIM2
	
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);//����TIME�������Ϊ����ģʽ
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = arr-100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//���PWM1ҪΪlow PWM2ҪΪHigh
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
}


//ͨ�ö�ʱ�� 3 ��ʼ��
//����ʱ��ѡ��Ϊ APB1 �� 2 ������ APB1 Ϊ 36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//void TIM3_Init(u16 arr, u16 psc)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	//TIM_OCInitTypeDef TIM_OCInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʱ�� TIM3 ʹ��
//	
//	//��ʱ�� TIM2 ��ʼ��
//	TIM_TimeBaseStructure.TIM_Period = arr; //�����Զ���װ�ؼĴ������ڵ�ֵ
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //��ʼ�� TIM3
//	
//	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);//����TIME�������Ϊ����ģʽ 
//	TIM_Cmd(TIM3,ENABLE);//ʹ�ܶ�ʱ�ж�
          
//}
	






