#include "spi.h"
//软件模拟SPI

void SPI1_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
 
	//GPIOB3,4,5,6初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4  | GPIO_Pin_5 | GPIO_Pin_6;//PB3,4,5,6推挽输出	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_SetBits(GPIOB,GPIO_Pin_3);//时钟信号默认高
	GPIO_SetBits(GPIOB,GPIO_Pin_4);//命令片选信号默认为高
	GPIO_SetBits(GPIOB,GPIO_Pin_5);//数据片选信号默认为高
	GPIO_SetBits(GPIOB,GPIO_Pin_6);//mosi信号默认为高
} 
//写命令
void SPI1_Write_cmd(u8 Cmd) 
{
	u8 i;
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
	for(i=0;i<8;i++)
	{	
		GPIO_ResetBits(GPIOB,GPIO_Pin_3);				 
		if((Cmd&0x80) == 0x80)									// 处理得到一位并发送
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_6);
		}			
		else
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		}		
		Cmd <<= 1;	          									// 数据左移1位，为下次发送做准备		
		GPIO_SetBits(GPIOB,GPIO_Pin_3);							// 上升沿写入数据 	
	}	
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
}
//写数据
void SPI1_Write_data(u32 Data) 
{
	u8 i;
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	for(i=0;i<32;i++)
	{	
		GPIO_ResetBits(GPIOB,GPIO_Pin_3);				 
		if((Data&0x80000000) == 0x80000000)									// 处理得到一位并发送
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_6);
		}			
		else
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		}		
		Data <<= 1;	          									// 数据左移1位，为下次发送做准备		
		GPIO_SetBits(GPIOB,GPIO_Pin_3);									// 上升沿写入数据 	
	}	
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
}
