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
	delay_init(); //延时函数初始化
	uart_init(9600); //串口初始化为 9600
	LED_Init(); //初始化与 LED 连接的硬件接口
	LCD_Init(); //初始化 LCD
	KEY_Init(); //按键初始化
	Adc_Init(); //ADC 初始化
	Dac1_Init(); //DAC 通道 1 初始化
	
	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(60,130,200,16,16,"WK_UP:+  KEY0:-");
	POINT_COLOR=BLUE;//设置字体为蓝色
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
		//WKUP/KEY1 按下了,或者定时时间到了
		{
			adcx=DAC_GetDataOutputValue(DAC_Channel_1);
			LCD_ShowxNum(124,150,adcx,4,16,0); //显示 DAC 寄存器值
			temp=(float)adcx*(3.3/4096); //得到 DAC 电压值
			adcx=temp;
			LCD_ShowxNum(124,170,temp,1,16,0); //显示电压值整数部分
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(140,170,temp,3,16,0X80); //显示电压值的小数部分
			adcx=Get_Adc_Average(ADC_Channel_1,10); //得到 ADC 转换值
			temp=(float)adcx*(3.3/4096); //得到 ADC 电压值
			adcx=temp;
			LCD_ShowxNum(124,190,temp,1,16,0); //显示电压值整数部分
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(140,190,temp,3,16,0X80); //显示电压值的小数部分
			LED0=!LED0;
			t=0;
		}
		delay_ms(10);
	}
}

