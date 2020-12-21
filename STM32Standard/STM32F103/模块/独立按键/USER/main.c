#include "sys.h"
#include "led.h"
#include "key.h"
#include "delay.h"

//按键控制LED亮灭
int main(void)
{
	delay_init();					//延时函数初始化
	LED_Init();						//LEDIO初始化
	KEY_Init();						//按键IO初始化
	LED0 = 1;
	while(1)
	{
		if(KEY_Scan(0)==WK_UP_PRES)	//判断KEY0按下
		{
			LED0 = !LED0;
		}	
	}
}
