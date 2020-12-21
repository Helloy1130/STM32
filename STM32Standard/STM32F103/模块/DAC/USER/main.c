#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "lcd.h"
#include "dac.h"
#include "key.h"
#include "led.h"


int main(void)
{	
	u16 adcx;
	float temp;
	u8 t=0;
	u16 dacval=0;
	u8 key;
	delay_init(); //��ʱ������ʼ��
	uart_init(9600); //���ڳ�ʼ��Ϊ 9600
	LED_Init(); //��ʼ���� LED ���ӵ�Ӳ���ӿ�
	LCD_Init(); //��ʼ�� LCD
	KEY_Init(); //������ʼ��
	Adc_Init(); //ADC ��ʼ��
	Dac1_Init(); //DAC ͨ�� 1 ��ʼ��
	
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,130,200,16,16,"WK_UP:+  KEY0:-");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(60,150,200,16,16,"DAC VAL:");
	
	LCD_ShowString(60,170,200,16,16,"DAC VOL:0.000V");
	LCD_ShowString(60,190,200,16,16,"ADC VOL:0.000V");
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);
	
	while(1)
	{
		t++;
		key=KEY_Scan(0);
		if(key==WK_UP_PRES)
		{
			if(dacval<4000)dacval+=200;
			DAC_SetChannel1Data(DAC_Align_12b_R, dacval);
		}
		else if(key==KEY0_PRES)
		{
			if(dacval>200)	dacval-=200;
			else dacval=0;
			DAC_SetChannel1Data(DAC_Align_12b_R, dacval);
		}
		if(t==10||key==KEY0_PRES||key==WK_UP_PRES)
		//WKUP/KEY1 ������,���߶�ʱʱ�䵽��
		{
			adcx=DAC_GetDataOutputValue(DAC_Channel_1);
			LCD_ShowxNum(124,150,adcx,4,16,0); //��ʾ DAC �Ĵ���ֵ
			temp=(float)adcx*(3.3/4096); //�õ� DAC ��ѹֵ
			adcx=temp;
			LCD_ShowxNum(124,170,temp,1,16,0); //��ʾ��ѹֵ��������
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(140,170,temp,3,16,0X80); //��ʾ��ѹֵ��С������
			adcx=Get_Adc_Average(ADC_Channel_1,10); //�õ� ADC ת��ֵ
			temp=(float)adcx*(3.3/4096); //�õ� ADC ��ѹֵ
			adcx=temp;
			LCD_ShowxNum(124,190,temp,1,16,0); //��ʾ��ѹֵ��������
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(140,190,temp,3,16,0X80); //��ʾ��ѹֵ��С������
			LED0=!LED0;
			t=0;
		}
		delay_ms(10);
	}
}

