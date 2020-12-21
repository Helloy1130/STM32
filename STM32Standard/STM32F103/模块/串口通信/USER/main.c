#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key.h"

//通过串口通信，按键发送不同的值

int main(void)
{
	u8 t;
	delay_init(); 									//延时函数初始化
	uart_init(9600); 								//串口初始化为 9600
	KEY_Init(); 									//初始化与独立按键连接的硬件接口
	
	while(1)
	{
		t = KEY_Scan(0); 							//得到某个按键的标志值
		switch(t)
		{
			case KEY0_PRES:
							USART_SendData(USART1, 0X01); //向串口 1 发送数据
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
							break;
			case KEY1_PRES:
							USART_SendData(USART1, 0X02); //向串口 1 发送数据
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
							break;
			case WK_UP_PRES:
							USART_SendData(USART1, 0X03); //向串口 1 发送数据
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
							break;
			default:
					delay_ms(10);
		}
	}	
}

