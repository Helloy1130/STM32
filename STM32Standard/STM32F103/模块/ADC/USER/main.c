#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "lcd.h"



int main(void)
{	
	u16 vol;				
	float temp;		
	
 	delay_init();	    	 //��ʱ������ʼ��
	
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600	
 	LCD_Init();
	LCD_Clear(WHITE); 		//����Ϊ��ɫ
	POINT_COLOR=BLUE;		//��������Ϊ��ɫ	      
	LCD_ShowString(60,60,200,16,16,"Voltage:0.000V");
	
 	Adc_Init();		  		//ADC��ʼ��
	
	while(1)
	{
		vol=Get_Adc_Average(ADC_Channel_1,10);
		temp=(float)vol*(3.3/4096);
		vol=temp;
		LCD_ShowxNum(124,60,vol,1,16,0);//��ʾ��ѹֵ����λ
		LCD_ShowxNum(140,60,(temp-vol)*1000,3,16,0X80);//��ʾ��ѹֵС��
		delay_ms(250);	//ÿ�� 250ms ��ȡһ�� ADCͨ�� 0 ��ֵ
	}
}
