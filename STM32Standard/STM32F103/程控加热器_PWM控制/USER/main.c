#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "ds18b20.h"
#include "pwm.h"
#include "lcd.h"
#include "24cxx.h" 
#include "myiic.h"
#include "touch.h" 
#include "sim900a.h"
#include "beep.h"
#include "led.h"
#include "pid.h"

//		PA8 --> PWM1��� ռ�ձ�Խ�󣬼���Խ��		PA11 --> PWM2���
//		PA0 --> �¶ȴ�����I/O					PA2 --> ������

extern u8 set_flag;//�趨�¶ȵ��ź�
extern u8 temp[4];
extern short temperature;//�¶ȴ��������յ���ֵ
double set_temp = 0;

int main(void)
{	
	u8 i;
	
	u16 int_temp = 0;
	u8 point_flag;
	
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //���� NVIC �жϷ��� 2:2 λ��ռ���ȼ��� 2 λ��Ӧ���ȼ�
	
	BEEP_Init();
 	LCD_Init();
	LED_Init();
	LCD_Clear(WHITE); 		//����Ϊ��ɫ
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	TIM1_PWM_Init(899,0);//����Ƶ��PWMƵ��=72000/(899+1)=80Khz �ܵļ���ֵΪ899
	
 	while(DS18B20_Init())	//DS18B20��ʼ��	
	{
		LCD_ShowString(60,50,200,16,16,"DS18B20 Error");
		delay_ms(200);
		LCD_Fill(60,50,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(60,50,200,16,16,"DS18B20 OK");
	delay_ms(3000);
	LCD_Clear(WHITE);
	
	tp_dev.init();			//���败����
	
	PIDParament_Init() ;//pid��ʼ��
	
	while(1)
	{	
		for(i=0;i<4;i++)
		{
			temp[i] = 99;
		}
		point_flag = 4;
		
		sim900a_sms_send_test();//ʹ�����ּ����趨�¶�
		
		if(set_flag)
		{
			for(i=0;i<4;i++)
			{
				if(temp[i]!=99)
				{
					if(temp[i]==10)
					{
						point_flag = i;
						continue;
					}
					int_temp *= 10;
					int_temp += temp[i];//10������
				}
			}
			if(point_flag==4) set_temp = int_temp;
			else
			{
				for(i=3-point_flag;i>0;i--)
				{
					set_temp = int_temp/10.0;
				}
			}
			delay_ms(1000);				
			LCD_ShowString(30+54,80,156,24,16,"SUCCESSFUL!");
			delay_ms(1000);
		}
		else
		{
			printf("error!\r\n");
			delay_ms(1000);	
		}
		set_flag = 0;
		int_temp = 0;
	}		
}






