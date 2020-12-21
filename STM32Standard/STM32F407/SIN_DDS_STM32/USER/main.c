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
		SPI1_SCK  		-----> PB.3   	ʱ���ź�
		SPI1_CMD_CS    	-----> PB.4  	����Ƭѡ�ź�
		SPI1_DATA_CS	-----> PB.5		����Ƭѡ�ź�
        SPI1_MOSI 		-----> PB.6   	���豸������������豸��������
****************************************************************************/

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	
	LED_Init();					//��ʼ��LED 
 	LCD_Init();					//LCD��ʼ�� 
	KEY_Init(); 				//������ʼ��  
	EXTIX_Init();       		//��ʼ���ⲿ�ж����� 
	tp_dev.init();				//��������ʼ��
	SPI1_Init();				//SPI1��ʼ��

 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	 
	delay_ms(1500); 
	
	sim900a_sms_send_test();
	
}
