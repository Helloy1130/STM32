#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "exti.h"
#include "key.h"


// LCD 屏显示文字，并使其能够向右滚动；“我爱无线电， 暑假集训使我快乐”
// 使用按键可以切换文字滚动方向，切换滚动速度；
//WK_UP 控制方向
//KEY0高速

extern u8 dir; //控制方向 0：右 1：左
extern u8 speed;//控制速度

int main(void)
{ 
	int i = 0;
	delay_init();	    	 //延时函数初始化	  
	uart_init(9600);	 	//串口初始化为9600，在LCD初始化中用到了print
 	LCD_Init();				//LCD初始化
	POINT_COLOR=RED; 		//画笔颜色是红色
	LCD_Clear(WHITE); 		//背景为白色
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置 NVIC 中断分组 2:2 位抢占优先级， 2 位响应优先
	KEY_Init();			//按键IO初始化
	EXTIX_Init();		//外部中断初始化
	
  	while(1) 
	{
		while(!dir)
		{
			POINT_COLOR=RED; 		//画笔颜色是红色
			LCD_ShowString_hanzi1(i,150,200,12,12,"我爱无线电，暑假集训使我快乐");
			delay_ms(speed);
			POINT_COLOR=WHITE; 		//画笔颜色是白色
			LCD_ShowString(i-1,150,1,12,12,"1");
			i++;
			if(i>=240) i = 0;
		}
		while(dir)
		{
			POINT_COLOR=RED; 		//画笔颜色是红色
			LCD_ShowString_hanzi1(i,150,200,12,12,"我爱无线电，暑假集训使我快乐");
			delay_ms(speed);
			POINT_COLOR=WHITE; 		//画笔颜色是白色
			LCD_ShowString(i+167,150,1,12,12,"1");
			i--;
			if(i<=-168) i = 240;
		}
	} 
}


