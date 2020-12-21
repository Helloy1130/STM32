#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "dac.h"
#include "timer.h"
#include "led.h"
#include "dma.h"
#include "math.h"

#define PI 3.1415926
#define DAC_DHR12R1    (u32)&(DAC->DHR12R1)   //DACͨ��1�����ַ

u16 Special_Wave[300]; 


//�������Ⲩ�ε����ݱ�
void Special_Wave_Data(u16 *D)
{
    u16 i;
    for( i=0;i<300;i++)
    {
		if(i<55)
			D[i]=(u16)((2-i/54.0)*4095/3.3);
		else if(i<110)
			D[i]=(u16)((1+(i-55)/54.0)*4095/3.3);
		else if(i<190)
			D[i]=(u16)(0);
		else if(i<245)
			D[i]=(u16)(2*sin((i-190)/54.0*PI/2)*4095/3.3);
		else if(i<300)
			D[i]=(u16)((2-(i-245)/54.0*2)*4095/3.3);
    }
}


int main(void)
{	
	delay_init(); //��ʱ������ʼ��
	uart_init(9600); //���ڳ�ʼ��Ϊ 9600
	
	LED_Init();
	
	Special_Wave_Data(Special_Wave);//������������
	
	Dac1_Init(); //DAC ͨ�� 1 ��ʼ��
	
	TIM2_Int_Init(1, 1199);//TIM3�жϳ�ʼ�� 60KHz  ����2   60/2=30k
	
	MYDMA_Config(DMA2_Channel3,(u32)DAC_DHR12R1,(u32)Special_Wave,300);//DMA2ͨ��3,����ΪDA1,�洢��ΪSpecial_Wave,����300
	TIM_Cmd(TIM2, ENABLE);
	
	LED0 = 0;
	
	while(1)
	{
		LED0=!LED0;
		delay_ms(300);
	}
		
}

