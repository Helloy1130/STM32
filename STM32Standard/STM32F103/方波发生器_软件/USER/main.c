#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "dac.h"
#include "timer.h"
#include "led.h"
#include "dma.h"
#include "math.h"
#include "exti.h"
#include "lcd.h"

#define PI 3.1415926
#define DAC_DHR12R1    (u32)&(DAC->DHR12R1)   //DAC通道1输出地址
#define DAC_DHR12R2    (u32)&(DAC->DHR12R2)   //DAC通道2输出地址

u8 flag = 1;//1: 方波 2： 三角波 3：正弦波 25k-50k 4:方波 25k-50k
u8 change = 0;
u16 SquareWave_Value1[96];
u16 TriangleWave_Value[96];
u16 SineWave_Value[96]; 
u16 SquareWave_Value2[96];
u16 Wave_Value1[96];
u16 Wave_Value2[96];
u16 freq_ctr1 = 49;//默认5k
u16 freq_ctr2 = 29;//默认25k
float freq = 5000;

/********方波输出表***********/
//cycle :波形表的位数 (0~256)
//Um        :输出电压的峰值(0~3.3)
/*******************************/
void SquareWave_Data( u16 cycle ,u16 *D,float Um)
{
    u16 i;
    for( i=0;i<cycle;i++)
    {
		if(i<cycle/2)
			D[i]=(u16)(Um*4095/3.3);
		else
			D[i]=(u16)(0);
    }
}

/********三角波输出表***********/
//cycle :波形表的位数 (0~256)
//Um        :输出电压的峰值(0~1.5)
/*******************************/
void TriangleWave_Data( u16 cycle ,u16 *D,float Um)
{
    u16 i;
    for( i=0;i<cycle;i++)
    {
        if(i<23)
			D[i]=(u16)((Um+i*Um/24)*4095/3.3);
		else if(i<72)
			D[i]=(u16)((2*Um-(i-23)*Um/24)*4095/3.3);
		else
			D[i]=(u16)(((i-71)*Um/24)*4095/3.3);
    }
}
/********正弦波输出表***********/
//cycle :波形表的位数 (0~256)
//Um        :输出电压的峰值(0~1.5)
/*******************************/
void SineWave_Data( u16 cycle ,u16 *D,float Um)
{
    u16 i;
    for( i=0;i<cycle;i++)
    {
        D[i]=(u16)((Um*sin(( 1.0*i/(cycle-1))*2*PI)+Um)*4095/3.3);
    }
}

int main(void)
{	
	u16 freq_int;
	u8 i;
	delay_init(); //延时函数初始化
	uart_init(9600); //串口初始化为 9600
	
	LED_Init();
	
	SquareWave_Data(96,SquareWave_Value1,3);//产生方波的数据表
	TriangleWave_Data(96,TriangleWave_Value,1.5);//产生三角波的数据表
	SineWave_Data(96,SineWave_Value,1.5);//产生正弦波的数据表
	SquareWave_Data(96,SquareWave_Value2,3);//产生方波的数据表
	
	Dac1_Init(); //DAC 通道 1 初始化
	Dac2_Init(); //DAC 通道 2 初始化
	
	EXTIX_Init();	
	
	TIM2_Int_Init(freq_ctr1, 2);//TIM2中断初始化 5k
	TIM4_Int_Init(freq_ctr2, 0);
	
	for(i=0;i<96;i++)
	{
		Wave_Value1[i] = SquareWave_Value1[i];
	}
	for(i=0;i<96;i++)
	{
		Wave_Value2[i] = SineWave_Value[i];
	}
	MYDMA_Config(DMA2_Channel3,(u32)DAC_DHR12R1,(u32)Wave_Value1,96);//DMA2通道3,外设为DA1,存储器为Wave_Value1,长度96
	MYDMA_Config(DMA2_Channel4,(u32)DAC_DHR12R2,(u32)Wave_Value2,96);//DMA2通道4,外设为DA2,存储器为Wave_Value2,长度96
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	
	LCD_Init();
	LCD_Clear(WHITE); 		//背景为白色
	POINT_COLOR=RED;		//设置字体为红色	      
	LCD_ShowString(60,60,200,16,16,"Wave: square");
	LCD_ShowString(60,80,200,16,16,"Vpp: 3 V");
	LCD_ShowString(60,100,200,16,16,"Freq: 5000.000 Hz");
	
	LED0 = 0;
	
	while(1)
	{
		if(flag==1&&change)
		{
			for(i=0;i<96;i++)
			{
				Wave_Value1[i] = SquareWave_Value1[i];
			}
			change = 0;
		}
		if(flag==2&&change)
		{
			for(i=0;i<96;i++)
			{
				Wave_Value1[i] = TriangleWave_Value[i];
			}
			change = 0;
		}
		if(flag==3&&change)
		{
			for(i=0;i<96;i++)
			{
				Wave_Value2[i] = SineWave_Value[i];
			}
			change = 0;
		}
		if(flag==4&&change)
		{
			for(i=0;i<96;i++)
			{
				Wave_Value2[i] = SquareWave_Value2[i];
			}
			change = 0;
		}
		freq_int=(u16)freq;
		LCD_ShowxNum(100,100,freq_int,5,16,0);//显示频率整数位
		LCD_ShowxNum(148,100,(freq-freq_int)*1000,3,16,0X80);//显示频率小数
		LED0=!LED0;
		delay_ms(300);
	}
		
}

