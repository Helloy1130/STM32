#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "ds18b20.h"
#include "pwm.h"
#include "lcd.h"
#include "24cxx.h" 
#include "myiic.h"
#include "touch.h" 
#include "sim900a.h"
#include "beep.h"
#include "led.h"
#include "pid.h"

//		PA8 --> PWM1输出 占空比越大，加热越慢		PA11 --> PWM2输出
//		PA0 --> 温度传感器I/O					PA2 --> 蜂鸣器

extern u8 set_flag;//设定温度的信号
extern u8 temp[4];
extern short temperature;//温度传感器接收到的值
double set_temp = 0;

int main(void)
{	
	u8 i;
	
	u16 int_temp = 0;
	u8 point_flag;
	
	delay_init();	    	 //延时函数初始化	  
	uart_init(9600);	 	//串口初始化为9600
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置 NVIC 中断分组 2:2 位抢占优先级， 2 位响应优先级
	
	BEEP_Init();
 	LCD_Init();
	LED_Init();
	LCD_Clear(WHITE); 		//背景为白色
 	POINT_COLOR=RED;//设置字体为红色 
	TIM1_PWM_Init(899,0);//不分频。PWM频率=72000/(899+1)=80Khz 总的计数值为899
	
 	while(DS18B20_Init())	//DS18B20初始化	
	{
		LCD_ShowString(60,50,200,16,16,"DS18B20 Error");
		delay_ms(200);
		LCD_Fill(60,50,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(60,50,200,16,16,"DS18B20 OK");
	delay_ms(3000);
	LCD_Clear(WHITE);
	
	tp_dev.init();			//电阻触摸屏
	
	PIDParament_Init() ;//pid初始化
	
	while(1)
	{	
		for(i=0;i<4;i++)
		{
			temp[i] = 99;
		}
		point_flag = 4;
		
		sim900a_sms_send_test();//使用数字键盘设定温度
		
		if(set_flag)
		{
			for(i=0;i<4;i++)
			{
				if(temp[i]!=99)
				{
					if(temp[i]==10)
					{
						point_flag = i;
						continue;
					}
					int_temp *= 10;
					int_temp += temp[i];//10进制数
				}
			}
			if(point_flag==4) set_temp = int_temp;
			else
			{
				for(i=3-point_flag;i>0;i--)
				{
					set_temp = int_temp/10.0;
				}
			}
			delay_ms(1000);				
			LCD_ShowString(30+54,80,156,24,16,"SUCCESSFUL!");
			delay_ms(1000);
		}
		else
		{
			printf("error!\r\n");
			delay_ms(1000);	
		}
		set_flag = 0;
		int_temp = 0;
	}		
}






