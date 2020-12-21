#include "sys.h"
#include "delay.h"
#include "wwdg.h"
#include "led.h"


//通过 DS0 来指示 STM32 是否被复位了，如果被复位了就会点亮 300ms。 DS1 用来指示中
//断喂狗，每次中断喂狗翻转一次。

int main(void)
{		
	delay_init();	    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	
	LED_Init(); 
	LED0=0;
	delay_ms(300);
	WWDG_Init(0X7F,0X5F,WWDG_Prescaler_8);
	
	while(1)
	{
		LED0=1;
	}
}
