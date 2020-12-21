#include "sys.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "delay.h"

//外部中断，按键控制跑马灯的速度
int main(void)
{
	delay_init();					//延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断分组
	LED_Init();						//LEDIO初始化
	EXTIX_Init();					//外部中断初始化
	LED0 = 1;
	
	while(1)
	{
		if(flag==0)
		{
			LED0 = 0;
			LED1 = 1;
			delay_ms(300);
			LED0 = 1;
			LED1 = 0;
			delay_ms(300);
		}
		if(flag==1)
		{
			LED0 = 0;
			LED1 = 1;
			delay_ms(500);
			LED0 = 1;
			LED1 = 0;
			delay_ms(500);
		}
		if(flag==2)
		{
			LED0 = 0;
			LED1 = 1;
			delay_ms(300);
			LED0 = 1;
			LED1 = 0;
			delay_ms(300);
		}
		if(flag==3)
		{
			LED0 = 0;
			LED1 = 1;
			delay_ms(100);
			LED0 = 1;
			LED1 = 0;
			delay_ms(100);
		}
		
	}
}




