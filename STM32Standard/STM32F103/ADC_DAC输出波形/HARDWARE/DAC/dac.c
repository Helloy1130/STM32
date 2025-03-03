
#include "dac.h"

void Dac1_Init(void)
{
	DAC_InitTypeDef DAC_InitType;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //使能PORTA通道时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // 端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //模拟输入GPIO_Mode_AIN
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4)	;//PA.4 输出高
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //使能DAC通道时钟 ,先配置好GPIO，再配置DAC，不然波形不对！！！
					
	DAC_InitType.DAC_Trigger=DAC_Trigger_T2_TRGO;//DAC触发为定时器3触发
	
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//屏蔽、幅值设置
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1输出缓存关闭 BOFF1=1
	DAC_Init(DAC_Channel_1,&DAC_InitType);	 //初始化DAC通道1

	DAC_Cmd(DAC_Channel_1, ENABLE);  //使能DAC1
	
	DAC_DMACmd(DAC_Channel_1, ENABLE);   //使能DAC对DMA的请求
	
  
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值

}

////设置通道1输出电压
////vol:0~3300,代表0~3.3V
//void Dac1_Set_Vol(u16 vol)
//{
//	float temp=vol;
//	temp/=1000;
//	temp=temp*4096/3.3;
//	DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12位右对齐数据格式设置DAC值
//}


