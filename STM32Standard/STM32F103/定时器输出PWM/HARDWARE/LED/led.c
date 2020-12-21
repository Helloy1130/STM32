#include "led.h"

//初始化 PA8 和 PD2 为输出口.并使能这两个口的时钟
//LEDIO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|
						RCC_APB2Periph_GPIOD,ENABLE);	//使能PA,PD端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;			//LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);				//初始化GPIOA.8
	GPIO_SetBits(GPIOA,GPIO_Pin_8);						//PA.8 输出高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 			//LED1-->PD.2 端口配置, 推挽输出
	GPIO_Init(GPIOD, &GPIO_InitStructure);				//推挽输出 ， IO 口速度为 50MHz
	GPIO_SetBits(GPIOD,GPIO_Pin_2); 					//PD.2 输出高	
}
