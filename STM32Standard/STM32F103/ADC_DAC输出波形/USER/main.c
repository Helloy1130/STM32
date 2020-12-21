#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "lcd.h"
#include "dac.h"
#include "timer.h"
#include "dma.h"
#include "math.h"


#define PI 3.1415926
#define DAC_DHR12R1    (u32)&(DAC->DHR12R1)   //DACͨ��1�����ַ


u16 SineWave_Value[256]; 
u16 SquareWave_Value[256];
u16 No_Wave_Value[256];
u16 Wave_Value[256];


/********���������***********/
//cycle :���α��λ�� (0~200)
//Um        :�����ѹ�ķ�ֵ(0~3)
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

/********���Ҳ������***********/
//cycle :���α��λ�� (0~256)
//Um        :�����ѹ�ķ�ֵ(0~1.5)
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
	u8 flag = 0;
	u16 i;
	u16 vol;				
	float temp;	
	
 	delay_init();	    	 //��ʱ������ʼ��
	
	SineWave_Data(200,SineWave_Value,1);//�������Ҳ������ݱ�
	SquareWave_Data(200,SquareWave_Value,1);//�������������ݱ�
	for(i=0;i<200;i++)
	{
		No_Wave_Value[i] = (u16)0;
	}
	
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	
	Adc_Init();		  		//ADC��ʼ��

	Dac1_Init(); 		//DAC ͨ�� 1 ��ʼ��

	TIM2_Int_Init(1, 179);//TIM2��ʼ�� 200KHz  ����2   2/200kHz=10us 179
	
	MYDMA_Config(DMA2_Channel3,(u32)DAC_DHR12R1,(u32)Wave_Value,200);//DMA2ͨ��3,����ΪDA1,�洢��ΪSquareWave_Value,����200
	TIM_Cmd(TIM2, ENABLE);
	
	LCD_Init();
	LCD_Clear(WHITE); 		//����Ϊ��ɫ
	POINT_COLOR=BLUE;		//��������Ϊ��ɫ	      
	LCD_ShowString(60,60,200,16,16,"Voltage:0.000V");
	
	while(1)
	{
		vol=Get_Adc_Average(ADC_Channel_1,10);
		temp=(float)vol*(3.3/4096);
		vol=temp;
		LCD_ShowxNum(124,60,vol,1,16,0);//��ʾ��ѹֵ����λ
		LCD_ShowxNum(140,60,(temp-vol)*1000,3,16,0X80);//��ʾ��ѹֵС��
		
		if(temp>0&&temp<=1&&flag!=1)//����
		{
			for(i=0;i<200;i++)
			{
				Wave_Value[i] = SquareWave_Value[i];
			}
			flag = 1;
		}
		if(temp>1&&temp<=2&&flag!=2)//���Ҳ�
		{
			for(i=0;i<200;i++)
			{
				Wave_Value[i] = SineWave_Value[i];
			}
			flag = 2;
		}
		if(temp>2&&flag!=0)//����ʾ
		{
			for(i=0;i<200;i++)
			{
				Wave_Value[i] = No_Wave_Value[i];
			}
			flag = 0;
		}
		delay_ms(25);	//ÿ�� 25ms ��ȡһ�� ADCͨ�� 0 ��ֵ
	}
}
