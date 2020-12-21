#include "sys.h"
#include "delay.h"
#include "spi.h"
#include "lcd.h"
#include "usart.h"	
#include "24cxx.h" 
#include "myiic.h"
#include "touch.h" 
#include "sim900a.h"

//			FPGA			    			stm32
//spi_cs_data    	PIN_A3	    	SPI4_CS   -----> PA.0   		片选信号
//spi_sck        	PIN_A4       	SPI4_CLK  -----> PA.3   		时钟信号
//spi_mosi	       	PIN_A5	    	SPI4_MOSI -----> PA.2		 	单片机输出，fpga输入							
//spi_miso	      	PIN_A6	    	SPI4_MISO -----> PA.1           单片机输入，fpga输出


u8 send_flag;//发送信号
u8 TXData[4];

int main()
{
	u16 transfer_data = 0;
	u8 i = 0;

	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	LCD_Init();			 	
	POINT_COLOR=RED;//设置字体为红色 
	
	SPI_Configuration(SPI_4);			//模拟SPI
	
	tp_dev.init();			//电阻触摸屏
	
	while(1)
	{
		for(i=0;i<4;i++)
		{
			TXData[i] = 99;
		}
		sim900a_sms_send_test();
		
		if(send_flag)
		{
			for(i=0;i<4;i++)
			{
				if(TXData[i]!=99)
				{
					transfer_data *= 10;
					transfer_data += TXData[i];//10进制数
				}
			}
			SPI4_Write_Byte(transfer_data);	//发送数据
            delay_ms(1000);
			LCD_ShowString(30+54,50,156,24,16,"SUCCESSFUL!");
			delay_ms(1000);
		}
		else
		{
			printf("error\r\n");
			delay_ms(1000);	
		}
		printf("%d\r\n",transfer_data);
		send_flag = 0;
		transfer_data = 0;
	}
}
