#include "exti.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"

extern u16 freq_ctr1;
extern u16 freq_ctr2;
extern float freq;
extern u8 flag;
extern u8 change;

//外部中断初始化函数
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//外部中断，需要使能 AFIO 时钟
	
	KEY_Init();			//初始化按键对应 io 模式
	 
	//GPIOC.5  中断线以及中断初始化配置	KEY0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);				//根据 EXTI_InitStructure 中指定的参数初始化外设 EXTI 寄存器
	
	//GPIOA.15 中断线以及中断初始化配置	KEY1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);				//根据 EXTI_InitStructure 中指定的参数初始化外设 EXTI 寄存器
	
	//GPIOA.0 中断线以及中断初始化配置	WK_UP
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);				//根据 EXTI_InitStructure 中指定的参数初始化外设 EXTI 寄存器
	
	//配置中断的分组，并使能中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;				//使能按键所在的外部中断通道0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 	//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 			//子优先级 1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//使能外部中断通道0
	NVIC_Init(&NVIC_InitStructure);				//根据 NVIC_InitStructure 中指定的参数初始化外设 NVIC 寄存器
	
	//配置中断的分组，并使能中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				//使能按键所在的外部中断通道9_5_
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 	//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 			//子优先级 1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//使能外部中断通道0
	NVIC_Init(&NVIC_InitStructure);				//根据 NVIC_InitStructure 中指定的参数初始化外设 NVIC 寄存器
	
	//配置中断的分组，并使能中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道15_10_
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 	//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; 			//子优先级 1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//使能外部中断通道0
	NVIC_Init(&NVIC_InitStructure);				//根据 NVIC_InitStructure 中指定的参数初始化外设 NVIC 寄存器
	
}


//中断函数
//外部中断0的服务函数，负责WK_UP按键的中断检测
void EXTI0_IRQHandler(void)
{ 
	delay_ms(10);//消抖
	if(WK_UP==1)
	{	
		if(flag==1||flag==2)//方波或三角波
		{
			if(freq_ctr1 > 24)//一直增加到10k
			{
				freq_ctr1--;
			}
			else
			{
				freq_ctr1 = 49;
			}
			freq = 250000.0 / (freq_ctr1 + 1);
			TIM2_Int_Init(freq_ctr1, 2);
		}
		if(flag==3||flag==4)//正弦波 方波
		{
			if(freq_ctr2 > 13)//一直增加到50k
			{
				freq_ctr2--;
			}
			else
			{
				freq_ctr2 = 29;
			}
			freq = 750000.0 / (freq_ctr2 + 1);
			TIM4_Int_Init(freq_ctr2, 0);
		}
		//freq_ctr2 = (freq_ctr1+1)*3/5 - 1;
		//TIM4_Int_Init(freq_ctr2, 0);
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //清除 EXTI0 线路上的中断标志位
}

//外部中断9_5_的服务函数，负责KEY0按键的中断检测
void EXTI9_5_IRQHandler(void)
{ 
	delay_ms(10);//消抖
	if(KEY0==0)
	{
		if(flag==1||flag==2)//方波或三角波
		{
			if(freq_ctr1 < 49)//一直减少到5k
			{
				freq_ctr1++;
			}
			else
			{
				freq_ctr1 = 24;
			}
			freq = 250000.0 / (freq_ctr1 + 1);
			TIM2_Int_Init(freq_ctr1, 2);
		}
		if(flag==3||flag==4)//正弦波 方波
		{
			if(freq_ctr2 < 29)//一直减少到25k
			{
				freq_ctr2++;
			}
			else
			{
				freq_ctr2 = 13;
			}
			freq = 750000.0 / (freq_ctr2 + 1);
			TIM4_Int_Init(freq_ctr2, 0);
		}
		//freq_ctr2 = (freq_ctr1+1)*3/5 - 1;
		//TIM4_Int_Init(freq_ctr2, 0);
	}
	EXTI_ClearITPendingBit(EXTI_Line5); //清除 EXTI0 线路上的中断标志位
}

//外部中断15_10_的服务函数，负责KEY1按键的中断检测
void EXTI15_10_IRQHandler(void)									//板子KEY1功能有问题
{ 
	delay_ms(10);//消抖
	if(KEY1==0)
	{
		if(flag==4) flag = 1;
		else flag++;
		if(flag==1)//方波输出
		{
			freq_ctr1 = 49;
			freq = 250000.0 / (freq_ctr1 + 1);//5k
			TIM2_Int_Init(freq_ctr1, 2);//TIM2中断初始化 5k
			LCD_ShowString(60,60,200,16,16,"Wave: square in ");
		}
		else if(flag==2)//三角波输出
		{
			LCD_ShowString(60,60,200,16,16,"Wave: triangle  ");
		}
		else if(flag==3)//正弦波输出
		{
			LCD_ShowString(60,60,200,16,16,"Wave: sin       ");
			freq_ctr2 = 29;
			freq = 750000.0 / (freq_ctr2 + 1);//25k
			TIM4_Int_Init(freq_ctr2, 0);//TIM4中断初始化 25k
		}
		else if(flag==4)//方波输出
		{
			LCD_ShowString(60,60,200,16,16,"Wave: square out");
		}
		change = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line15); //清除 EXTI15 线路上的中断标志位
}



