#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"  
#include "24cxx.h" 
#include "myiic.h"
#include "touch.h" 
#include "sim900a.h"

   	

 int main(void)
 { 
 	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
	 
	LCD_Init();			   	//初始化LCD 		 	
	tp_dev.init();			//触摸屏初始化			
	sim900a_sms_send_test();
	delay_ms(1500);		
}

