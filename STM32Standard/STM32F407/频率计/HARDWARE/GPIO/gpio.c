#include "gpio.h"

//PC5,���ڿ���բ��
void GPIO_init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��
 
	//GPIOC5��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PC5�������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
	
	GATE = 0;//Ĭ�ϵ�

}
