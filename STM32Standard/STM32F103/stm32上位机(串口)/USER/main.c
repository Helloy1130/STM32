#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "dac.h"
#include "timer.h"
#include "dma.h"
#include "math.h"
#include "adc.h"

/****************************/
//		位数			300
//		输出频率			50-1KHz
/****************************/

#define PI 3.1415926
#define DAC_DHR12R1    (u32)&(DAC->DHR12R1)   //DA1地址

u16 SineWave_Value[300]; 
u16 SquareWave_Value[300];
u16 TriangularWave_Value[300];
u16 ZigzagWave_Value[300];

u16 Wave_Value[300];
extern u8 flag;
extern u16 ADC_ConvertedValue[300];


/********方波输出表***********/
//cycle :波形表的位数 
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

/********正弦波输出表***********/
//cycle :波形表的位数 
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

/********三角波输出表***********/
//cycle :波形表的位数 
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

/********锯齿波输出表***********/
//cycle :波形表的位数 
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
	delay_init(); //延时函数初始化
	uart_init(19200); //串口初始化为 19200
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //设置NVIC中断分组1:2位抢占优先级，2位响应优先级
	
	SquareWave_Data(300, SquareWave_Value);//产生波形数据
	SineWave_Data(300, SineWave_Value);
	TriangularWave_Data(300, TriangularWave_Value);
	ZigzagWave_Data(300, ZigzagWave_Value);
	for(i=0;i<300;i++)
	{
		Wave_Value[i] = SineWave_Value[i];//默认正弦波
	}
	
	Dac1_Init(); //DAC 通道 1 初始化
	
	TIM2_Init(2399, 0);//TIM2中断初始化 300KHz 
	
	DMA2_Config(DMA2_Channel3, (u32)DAC_DHR12R1, (u32)Wave_Value, 300);//DMA2通道3,外设为DA1,存储器为Wavedata,长度200
	
	DMA1_Config();//存储ADC的值发给串口
	
	Adc_Init();//ADC由定时器触发，所得的值存到DMA1中
	
	while(1)
	{
		if(flag == 1)
		{
			for(i=0;i<300;i++)
			{
				USART1->DR = (u8)(ADC_ConvertedValue[i]/10);
				while((USART1->SR&0X40)==0);//等待发送结束
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

