#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "lcd.h"

u16 buf[240];//�洢�ɼ����Ĳ��ε�ѹ��ֵ
u16 YPos[240];//�洢Y�������ֵ

void WaveShow()
{
	u16 i;
	for(i=0;i<240;i++)//�ɼ������Ż���
	{
		buf[i] = Get_Adc(ADC_Channel_1);
	}
	for(i=0;i<240;i++)//ת��������
	{
		YPos[i] = 240-buf[i]*100/4096;
		YPos[i+1] = 240-buf[i]*100/4096;
		if(YPos[i]<100) YPos[i] = 240; //������Χ����ʾ
		LCD_DrawLine(i,YPos[i],i+1,YPos[i+1]);//����
		delay_ms(10);
		if(i==239)
		{
			LCD_Clear(WHITE);
			LCD_ShowString(60,60,200,16,16,"Vol_VPP:0.000V");
		}
	}
}

u16 Get_Vpp(void)	   //��ȡ���ֵ
{
	u16 max_voc = buf[0];
	u16 min_voc = buf[0];
	u16 i = 0;
	u16 Vpp = 0;
	for(i=1;i<240;i++)
	{
		if(buf[i]>max_voc)
		{
			max_voc = buf[i];
		}
		if(buf[i]<min_voc)
		{
			min_voc = buf[i];
		}			
	} 	
	Vpp = max_voc-min_voc;
	return Vpp;	
}

int main(void)
{	
	u16 vol_VPP;	
	float temp;		
	
 	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //���� NVIC �жϷ��� 2:2 λ��ռ���ȼ��� 2 λ��Ӧ���ȼ�
	
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600	
 	LCD_Init();
	LCD_Clear(WHITE); 		//����Ϊ��ɫ
	POINT_COLOR=BLUE;		//��������Ϊ��ɫ	
	LCD_ShowString(60,60,200,16,16,"Vol_VPP:0.000V");
	
 	Adc_Init();		  		//ADC��ʼ��

	while(1)
	{
		WaveShow();
		vol_VPP=Get_Vpp();
		temp=(float)(vol_VPP*3.3/4096);
		vol_VPP=temp;
		LCD_ShowxNum(124,60,vol_VPP,1,16,0);//��ʾ��ѹֵ����λ
		LCD_ShowxNum(140,60,(temp-vol_VPP)*1000,3,16,0X80);//��ʾ��ѹֵС��
		delay_ms(25); 		
	}
}
