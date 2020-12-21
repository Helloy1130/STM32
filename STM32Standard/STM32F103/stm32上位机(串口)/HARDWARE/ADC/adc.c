#include "adc.h"
#include "timer.h"
#include "dma.h"

//当外部触发信号被选为ADC规则或注入转换时，只有它的上升沿可以启动转换 
void  Adc_Init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟

	//PA2 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//由定时器2.2触发
	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器 
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//配置ADC时钟，为PCLK2的6分频，即12Hz
	
	//设置指定ADC的规则组通道，一个序列，采样时间 
	//对于每个要转换的通道，采样时间建议尽量长一点，以获得较高的准确度，但是这样会降
	//低 ADC 的转换速率。
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道2,采样时间为239.5周期	

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);//外部触发使能（相对于ADC模块而言，实际上仍然在32内部）
  
	ADC_DMACmd(ADC1,ENABLE);//使能DMA1
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	ADC_StartCalibration(ADC1);	 //开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);//外部触发使能（相对于ADC模块而言，实际上仍然在32内部）
	TIM_Cmd(TIM2,ENABLE);
          
}



