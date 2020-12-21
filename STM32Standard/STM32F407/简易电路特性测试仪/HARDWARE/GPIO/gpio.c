#include "gpio.h"

void myGPIO_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA GPIOE时钟
 
	//GPIOE6初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PE6推挽输出	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化
	
	//GPIOA4.5.6初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;//PA4,PA5,PA6
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_6);//默认低
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//默认低
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);//默认低
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);//默认低

}
