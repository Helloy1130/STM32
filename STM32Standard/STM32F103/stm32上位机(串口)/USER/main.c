#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "dac.h"
#include "timer.h"
#include "dma.h"
#include "math.h"
#include "adc.h"

/****************************/
//		λ��			300
//		���Ƶ��			50-1KHz
/****************************/

#define PI 3.1415926
#define DAC_DHR12R1    (u32)&(DAC->DHR12R1)   //DA1��ַ

u16 SineWave_Value[300]; 
u16 SquareWave_Value[300];
u16 TriangularWave_Value[300];
u16 ZigzagWave_Value[300];

u16 Wave_Value[300];
extern u8 flag;
extern u16 ADC_ConvertedValue[300];


/********���������***********/
//cycle :���α��λ�� 
//V     :1V
/****************************/
void SquareWave_Data( u16 cycle ,u16 *D)
{
    u16 i;
    for( i=0;i<cycle;i++)
    {
		if(i<cycle/2)
			D[i]=(u16)(4095*1/3.3);
		else
			D[i]=(u16)(0);
    }
}

/********���Ҳ������***********/
//cycle :���α��λ�� 
//VPP   ;2V
/******************************/
void SineWave_Data( u16 cycle ,u16 *D)
{
    u16 i;
    for( i=0;i<cycle;i++)
    {
        D[i]=(u16)((sin(( 1.0*i/(cycle-1))*2*PI)+1)*4095/3.3);
    }
}

/********���ǲ������***********/
//cycle :���α��λ�� 
//VPP   ;2V
/******************************/
void TriangularWave_Data( u16 cycle ,u16 *D)
{
    u16 i;
    for( i=0;i<cycle;i++)
    {
        if(i<cycle/4)
			D[i]=(u16)((i/74.0+1)*4095/3.3);
		else if(i<cycle/2)
			D[i]=(u16)((2-(i-75)/74.0)*4095/3.3);
		else if(i<cycle*3/4)
			D[i]=(u16)((1-(i-150)/74.0)*4095/3.3);
		else
			D[i]=(u16)((i-225)/74.0*4095/3.3);
    }
}

/********��ݲ������***********/
//cycle :���α��λ�� 
//VPP   ;2V
/*******************************/
void ZigzagWave_Data( u16 cycle ,u16 *D)
{
    u16 i;
    for( i=0;i<cycle;i++)
    {
        D[i]=(u16)(2*4095*i/299.0/3.3);
    }
}


int main(void)
{	
	u16 i;
	delay_init(); //��ʱ������ʼ��
	uart_init(19200); //���ڳ�ʼ��Ϊ 19200
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //����NVIC�жϷ���1:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	SquareWave_Data(300, SquareWave_Value);//������������
	SineWave_Data(300, SineWave_Value);
	TriangularWave_Data(300, TriangularWave_Value);
	ZigzagWave_Data(300, ZigzagWave_Value);
	for(i=0;i<300;i++)
	{
		Wave_Value[i] = SineWave_Value[i];//Ĭ�����Ҳ�
	}
	
	Dac1_Init(); //DAC ͨ�� 1 ��ʼ��
	
	TIM2_Init(2399, 0);//TIM2�жϳ�ʼ�� 300KHz 
	
	DMA2_Config(DMA2_Channel3, (u32)DAC_DHR12R1, (u32)Wave_Value, 300);//DMA2ͨ��3,����ΪDA1,�洢��ΪWavedata,����200
	
	DMA1_Config();//�洢ADC��ֵ��������
	
	Adc_Init();//ADC�ɶ�ʱ�����������õ�ֵ�浽DMA1��
	
	while(1)
	{
		if(flag == 1)
		{
			for(i=0;i<300;i++)
			{
				USART1->DR = (u8)(ADC_ConvertedValue[i]/10);
				while((USART1->SR&0X40)==0);//�ȴ����ͽ���
			}
			flag = 0;
			delay_ms(1000);
		}
		
		if(USART_RX_STA)
		{
			if(USART_RX_BUF[0] == '1') 
			{
				for(i=0;i<300;i++)
				{
					Wave_Value[i] = SquareWave_Value[i];
				}
			}
			else if(USART_RX_BUF[0] == '2')
			{
				for(i=0;i<300;i++)
				{
					Wave_Value[i] = SineWave_Value[i];
				}
			}
			else if(USART_RX_BUF[0] == '3')
			{
				for(i=0;i<300;i++)
				{
					Wave_Value[i] = TriangularWave_Value[i];
				}
			}
			else if(USART_RX_BUF[0] == '4')
			{
				for(i=0;i<300;i++)
				{
					Wave_Value[i] = ZigzagWave_Value[i];
				}
			}
			else USART_RX_STA = 0;
			
			if(USART_RX_BUF[1] == 'A')//50Hz
			{
				TIM2_Init(4799, 0);//15K
				USART_RX_STA = 0;
			}
			else if(USART_RX_BUF[1] == 'B')//100Hz
			{
				TIM2_Init(2399, 0);//30K
				USART_RX_STA = 0;
			}
			else if(USART_RX_BUF[1] == 'C')//300Hz
			{
				TIM2_Init(799, 0);//90K
				USART_RX_STA = 0;
			}
			else if(USART_RX_BUF[1] == 'D')//500Hz
			{
				TIM2_Init(479, 0);//150K
				USART_RX_STA = 0;
			}
			else if(USART_RX_BUF[1] == 'E')//1000Hz
			{
				TIM2_Init(239, 0);//300K
				USART_RX_STA = 0;
			}
			else USART_RX_STA = 0;
			
		}
	}
	
}

