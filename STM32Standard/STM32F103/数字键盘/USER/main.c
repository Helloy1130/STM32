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
 	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	 
	LCD_Init();			   	//��ʼ��LCD 		 	
	tp_dev.init();			//��������ʼ��			
	sim900a_sms_send_test();
	delay_ms(1500);		
}

