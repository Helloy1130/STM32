#include "sys.h"
#include "delay.h"
#include "led.h"
#include "timer.h"

//定时器3中断，触发后LED1反相，间隔1s

int main(void)
{		
	delay_init(); //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置 NVIC 中断分组 2:2 位抢占优先级， 2 位响应优先级
	
	LED_Init(); //LED 端口初始化
	TIM3_Int_Init(4999, 7199);//TIM3中断初始化 10KHz  计数0-4999   5000/10kHz=0.5s   间隔1s闪烁
	
	while(1);

}
