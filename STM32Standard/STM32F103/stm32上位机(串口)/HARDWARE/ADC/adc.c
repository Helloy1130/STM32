#include "adc.h"
#include "timer.h"
#include "dma.h"

//���ⲿ�����źű�ѡΪADC�����ע��ת��ʱ��ֻ�����������ؿ�������ת�� 
void  Adc_Init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��

	//PA2 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//�ɶ�ʱ��2.2����
	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ��� 
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12Hz
	
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ�� 
	//����ÿ��Ҫת����ͨ��������ʱ�佨�龡����һ�㣬�Ի�ýϸߵ�׼ȷ�ȣ����������ή
	//�� ADC ��ת�����ʡ�
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��2,����ʱ��Ϊ239.5����	

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);//�ⲿ����ʹ�ܣ������ADCģ����ԣ�ʵ������Ȼ��32�ڲ���
  
	ADC_DMACmd(ADC1,ENABLE);//ʹ��DMA1
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);	 //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);//�ⲿ����ʹ�ܣ������ADCģ����ԣ�ʵ������Ȼ��32�ڲ���
	TIM_Cmd(TIM2,ENABLE);
          
}



