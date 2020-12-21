#include "timer.h"
#include "usart.h"

extern u16 PWM_frequency;

//ͨ�ö�ʱ�� 3 �жϳ�ʼ��
//����ʱ��ѡ��Ϊ APB1 �� 2 ������ APB1 Ϊ 36M,���Ϊ72M
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
	u8 i;
	u8 PWM_f[3];
	PWM_f[0]=PWM_frequency/100+48; //ȡ��λ
	PWM_f[1]=(PWM_frequency/10)%10+48; //ȡʮλ
	PWM_f[2]=PWM_frequency%10+48; //ȡ��λ
;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //��� TIM3 �����жϷ������
	{
		printf("PWM��Ƶ����: ");
		for(i=0;i<3;i++)
		{
			USART1->DR=PWM_f[i];
			while((USART1->SR&0X40)==0);//�ȴ����ͽ���
		}
		printf("KHz");
		printf("\r\n\r\n");//����
		while((USART1->SR&0X40)==0);//�ȴ����ͽ���
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update ); //��� TIM3 �����жϱ�־
	}
}
















