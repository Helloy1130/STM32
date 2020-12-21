#include "gpio.h"

void myGPIO_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOA GPIOEʱ��
 
	//GPIOE6��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PE6�������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
	
	//GPIOA4.5.6��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;//PA4,PA5,PA6
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_6);//Ĭ�ϵ�
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//Ĭ�ϵ�
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);//Ĭ�ϵ�
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);//Ĭ�ϵ�

}
