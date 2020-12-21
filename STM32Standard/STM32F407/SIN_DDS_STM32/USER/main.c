#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "touch.h" 
#include "sim900a.h" 
#include "exti.h"
#include "spi.h"

/****************************************************************************
								SPI	
		SPI1_SCK  		-----> PB.3   	时钟信号
		SPI1_CMD_CS    	-----> PB.4  	命令片选信号
		SPI1_DATA_CS	-----> PB.5		数据片选信号
        SPI1_MOSI 		-----> PB.6   	主设备数据输出，从设备数据输入
****************************************************************************/

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化 
	KEY_Init(); 				//按键初始化  
	EXTIX_Init();       		//初始化外部中断输入 
	tp_dev.init();				//触摸屏初始化
	SPI1_Init();				//SPI1初始化

 	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	 
	delay_ms(1500); 
	
	sim900a_sms_send_test();
	
}
