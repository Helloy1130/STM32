#include "timer.h"
#include "math.h"
#include "usart.h"
#include "dac.h"

#define PI 3.1415926
u16 count;
//ͨ�ö�ʱ�� 3 �жϳ�ʼ��
//����ʱ��ѡ��Ϊ APB1 �� 2 ������ APB1 Ϊ 36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʱ�� TIM3 ʹ��
	
	//��ʱ�� TIM3 ��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //��ʼ�� TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //��������ж�
	
	//�ж����ȼ� NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ� 0 ��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ� 3 ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure); //��ʼ�� NVIC �Ĵ���
	
	TIM_Cmd(TIM3, ENABLE); //ʹ�� TIM3
	
}

//��ʱ�� 3 �жϷ������
//����жϴ��������������
void TIM3_IRQHandler(void)
{
	u16 vout;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //��� TIM3 �����жϷ������
	{
		if(count<100)
		{
			vout = 0;
			printf("%d\r\n",vout);
			Dac1_Set_Vol(vout);
			count++;
		}
		else if(count<250)
		{
			vout = (int)((2-(count-100)/150.0)*1000);
			printf("%d\r\n",vout);
			Dac1_Set_Vol(vout);
			count++;
		}
		else if(count<400)
		{
			vout = (int)((1+(count-250)/150.0)*1000);
			printf("%d\r\n",vout);
			Dac1_Set_Vol(vout);
			count++;
		}
		else if(count<600)
		{
			vout = 0;
			printf("%d\r\n",vout);
			Dac1_Set_Vol(vout);
			count++;
		}
		else if(count<750)
		{
			vout = (int)(2000*sin((count-600)*PI/300.0));
			printf("%d\r\n",vout);
			Dac1_Set_Vol(vout);
			count++;
		}
		else if(count<900)
		{
			vout = (int)((2-(count-750)/75.0)*1000);
			printf("%d\r\n",vout);
			Dac1_Set_Vol(vout);
			count++;
		}
		else if(count<1000)
		{
			vout = 0;
			printf("%d\r\n",vout);
			Dac1_Set_Vol(vout);
			count++;
		}
		else
		{
			count = 0;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update ); //��� TIM3 �����жϱ�־
	}
}
















