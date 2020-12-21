#include "spi.h"
//���ģ��SPI

void SPI1_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
 
	//GPIOB3,4,5,6��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4  | GPIO_Pin_5 | GPIO_Pin_6;//PB3,4,5,6�������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	GPIO_SetBits(GPIOB,GPIO_Pin_3);//ʱ���ź�Ĭ�ϸ�
	GPIO_SetBits(GPIOB,GPIO_Pin_4);//����Ƭѡ�ź�Ĭ��Ϊ��
	GPIO_SetBits(GPIOB,GPIO_Pin_5);//����Ƭѡ�ź�Ĭ��Ϊ��
	GPIO_SetBits(GPIOB,GPIO_Pin_6);//mosi�ź�Ĭ��Ϊ��
} 
//д����
void SPI1_Write_cmd(u8 Cmd) 
{
	u8 i;
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
	for(i=0;i<8;i++)
	{	
		GPIO_ResetBits(GPIOB,GPIO_Pin_3);				 
		if((Cmd&0x80) == 0x80)									// ����õ�һλ������
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_6);
		}			
		else
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		}		
		Cmd <<= 1;	          									// ��������1λ��Ϊ�´η�����׼��		
		GPIO_SetBits(GPIOB,GPIO_Pin_3);							// ������д������ 	
	}	
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
}
//д����
void SPI1_Write_data(u32 Data) 
{
	u8 i;
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	for(i=0;i<32;i++)
	{	
		GPIO_ResetBits(GPIOB,GPIO_Pin_3);				 
		if((Data&0x80000000) == 0x80000000)									// ����õ�һλ������
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_6);
		}			
		else
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		}		
		Data <<= 1;	          									// ��������1λ��Ϊ�´η�����׼��		
		GPIO_SetBits(GPIOB,GPIO_Pin_3);									// ������д������ 	
	}	
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
}
