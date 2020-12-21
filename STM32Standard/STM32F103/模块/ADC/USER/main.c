#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "lcd.h"



int main(void)
{	
	u16 vol;				
	float temp;		
	
 	delay_init();	    	 //延时函数初始化
	
	uart_init(9600);	 	//串口初始化为9600	
 	LCD_Init();
	LCD_Clear(WHITE); 		//背景为白色
	POINT_COLOR=BLUE;		//设置字体为蓝色	      
	LCD_ShowString(60,60,200,16,16,"Voltage:0.000V");
	
 	Adc_Init();		  		//ADC初始化
	
	while(1)
	{
		vol=Get_Adc_Average(ADC_Channel_1,10);
		temp=(float)vol*(3.3/4096);
		vol=temp;
		LCD_ShowxNum(124,60,vol,1,16,0);//显示电压值整数位
		LCD_ShowxNum(140,60,(temp-vol)*1000,3,16,0X80);//显示电压值小数
		delay_ms(250);	//每隔 250ms 读取一次 ADC通道 0 的值
	}
}
