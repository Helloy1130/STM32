#include "exti.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

u8 flag;

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
		flag = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //清除 EXTI0 线路上的中断标志位
}

//外部中断9_5_的服务函数，负责KEY0按键的中断检测
void EXTI9_5_IRQHandler(void)
{ 
	delay_ms(10);//消抖
	if(KEY0==1)
	{
		flag = 3;
	}
	EXTI_ClearITPendingBit(EXTI_Line5); //清除 EXTI0 线路上的中断标志位
}

//外部中断15_10_的服务函数，负责KEY1按键的中断检测
void EXTI15_10_IRQHandler(void)
{ 
	delay_ms(10);//消抖
	if(KEY1==0)
	{
		flag = 2;
	}
	EXTI_ClearITPendingBit(EXTI_Line15); //清除 EXTI15 线路上的中断标志位
}



