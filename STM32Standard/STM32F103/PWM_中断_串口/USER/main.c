#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "pwm.h"
#include "led.h"
#include "exti.h"
#include "timer.h"

//WK_UP -- 频率递增		KEY0 -- 频率递减
u16 PWM_frequency;	//PWM频率
u16 PWM_cnt = 899;

int main(void)
{			
	u8 dir;				//控制方向 1:逐渐变亮 0：逐渐变暗
	u16 led0pwmval = 0;	//调整占空比
	
	delay_init(); 		//延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置 NVIC 中断分组 2:2 位抢占优先级， 2 位响应优先级
	
	LED_Init(); 		//LEDIO初始化
	TIM1_PWM_Init(PWM_cnt,0);//不分频 PWM频率=72000KHZ/(899+1)=80KHz   初始状态
	PWM_frequency = 72000/(PWM_cnt+1);//80KHz
	
	uart_init(9600);    //串口初始化为 9600
	TIM3_Int_Init(9999, 7199);//TIM3中断初始化 10KHz  计数0-9999   10000/10kHz=1s
	
	EXTIX_Init();		//外部中断初始化
	
	while(1)
	{
		delay_ms(10);
		if(dir) led0pwmval++;
		else led0pwmval--;
		if(led0pwmval>=PWM_cnt-600) dir = 0;
		if(led0pwmval==0) dir = 1;
		TIM_SetCompare1(TIM1,led0pwmval);//修改PWM输出的占空比
	}		
}



