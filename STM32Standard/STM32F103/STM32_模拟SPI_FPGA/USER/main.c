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
//spi_cs_data    	PIN_A3	    	SPI4_CS   -----> PA.0   		Ƭѡ�ź�
//spi_sck        	PIN_A4       	SPI4_CLK  -----> PA.3   		ʱ���ź�
//spi_mosi	       	PIN_A5	    	SPI4_MOSI -----> PA.2		 	��Ƭ�������fpga����							
//spi_miso	      	PIN_A6	    	SPI4_MISO -----> PA.1           ��Ƭ�����룬fpga���


u8 send_flag;//�����ź�
u8 TXData[4];

int main()
{
	u16 transfer_data = 0;
	u8 i = 0;

	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	LCD_Init();			 	
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	
	SPI_Configuration(SPI_4);			//ģ��SPI
	
	tp_dev.init();			//���败����
	
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
					transfer_data += TXData[i];//10������
				}
			}
			SPI4_Write_Byte(transfer_data);	//��������
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
