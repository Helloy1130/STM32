#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "lcd.h"

u16 buf[240];//存储采集到的波形电压数值
u16 YPos[240];//存储Y坐标的数值

void WaveShow()
{
	u16 i;
	for(i=0;i<240;i++)//采集（可优化）
	{
		buf[i] = Get_Adc(ADC_Channel_1);
	}
	for(i=0;i<240;i++)//转换成坐标
	{
		YPos[i] = 240-buf[i]*100/4096;
		YPos[i+1] = 240-buf[i]*100/4096;
		if(YPos[i]<100) YPos[i] = 240; //超出范围不显示
		LCD_DrawLine(i,YPos[i],i+1,YPos[i+1]);//画线
		delay_ms(10);
		if(i==239)
		{
			LCD_Clear(WHITE);
			LCD_ShowString(60,60,200,16,16,"Vol_VPP:0.000V");
		}
	}
}

u16 Get_Vpp(void)	   //获取峰峰值
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
	
 	delay_init();	    	 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置 NVIC 中断分组 2:2 位抢占优先级， 2 位响应优先级
	
	uart_init(9600);	 	//串口初始化为9600	
 	LCD_Init();
	LCD_Clear(WHITE); 		//背景为白色
	POINT_COLOR=BLUE;		//设置字体为蓝色	
	LCD_ShowString(60,60,200,16,16,"Vol_VPP:0.000V");
	
 	Adc_Init();		  		//ADC初始化

	while(1)
	{
		WaveShow();
		vol_VPP=Get_Vpp();
		temp=(float)(vol_VPP*3.3/4096);
		vol_VPP=temp;
		LCD_ShowxNum(124,60,vol_VPP,1,16,0);//显示电压值整数位
		LCD_ShowxNum(140,60,(temp-vol_VPP)*1000,3,16,0X80);//显示电压值小数
		delay_ms(25); 		
	}
}
