#include "dma.h"
#include "usart.h"

#define ADC1_DR_Address    ((u32)0x4001244C)		//ADC1�ĵ�ַ  
volatile u16 ADC_ConvertedValue[300];			//ADC����������  
u8 flag;

/*********************************************/
//DMA1.1 ����>ADC1.1       DMA2.3 ����>DAC1.1 
/*********************************************/

//DMA2�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//cndtr:���ݴ����� 

//DAC1
void DMA2_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//ʹ��DMA����
	
    DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize = cndtr;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ����,���ִ���(16bits)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ����
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMAͨ�� xӵ�г������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
	
	DMA_Cmd(DMA_CHx, ENABLE); //ʹ��DMAͨ��	  	
} 

//DMA1 ADC1
// ע��ADCΪ12λģ��ת������ֻ��ADCConvertedValue�ĵ�12λ��Ч
void DMA1_Config()
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	
    DMA_DeInit(DMA1_Channel1);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;  //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴����赽�ڴ�
	DMA_InitStructure.DMA_BufferSize = 300;  //DMAͨ����DMA����Ĵ�С
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ����,���ִ���(16bits)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ����
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� 1ӵ�и����ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);		//ʹ�ܴ�������ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, ENABLE); //ʹ��DMAͨ��	  	
	
}

//DMA1	�жϴ�����
void  DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)
	{
		flag = 1;
	}
	DMA_ClearITPendingBit(DMA1_IT_TC1);
}




