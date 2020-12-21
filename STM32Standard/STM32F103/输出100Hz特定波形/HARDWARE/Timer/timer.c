#include "timer.h"
#include "delay.h"
#include "usart.h"


//ͨ�ö�ʱ�� 2 �жϳ�ʼ��
//����ʱ��ѡ��Ϊ APB1 �� 2 ������ APB1 Ϊ 36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void TIM2_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʱ�� TIM2 ʹ��
	
	//��ʱ�� TIM2 ��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�
	//TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //��ʼ�� TIM2
	
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);//����TIME�������Ϊ����ģʽ
	
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //��������ж�
	
//	//�ж����ȼ� NVIC ����
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3 �ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ� 0 ��
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ� 3 ��
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ����ʹ��
//	NVIC_Init(&NVIC_InitStructure); //��ʼ�� NVIC �Ĵ���
	
	//TIM_Cmd(TIM3, ENABLE); //ʹ�� TIM3
	
}

////��ʱ�� 3 �жϷ������
////����жϴ��������������
//void TIM3_IRQHandler(void)
//{
//	u16 vout;
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //��� TIM3 �����жϷ������
//	{
////		if(flag==1)
////		{
////			if(count<50) 
////			{
////				vout = 0;
////				//printf("%d\r\n",vout);
////				Dac1_Set_Vol(vout);
////				count++;
////			}
////			else if(count<100)
////			{
////				vout = 1000;
////				//printf("%d\r\n",vout);
////				Dac1_Set_Vol(vout);
////				count++;
////			}
////			else 
////			{
////				count = 0;
////			}
////		}
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update ); //��� TIM3 �����жϱ�־
//	}
//}
















