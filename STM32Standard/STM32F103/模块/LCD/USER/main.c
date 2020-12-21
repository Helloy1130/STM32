#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "exti.h"
#include "key.h"


// LCD ����ʾ���֣���ʹ���ܹ����ҹ��������Ұ����ߵ磬 ��ټ�ѵʹ�ҿ��֡�
// ʹ�ð��������л����ֹ��������л������ٶȣ�
//WK_UP ���Ʒ���
//KEY0����

extern u8 dir; //���Ʒ��� 0���� 1����
extern u8 speed;//�����ٶ�

int main(void)
{ 
	int i = 0;
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600����LCD��ʼ�����õ���print
 	LCD_Init();				//LCD��ʼ��
	POINT_COLOR=RED; 		//������ɫ�Ǻ�ɫ
	LCD_Clear(WHITE); 		//����Ϊ��ɫ
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //���� NVIC �жϷ��� 2:2 λ��ռ���ȼ��� 2 λ��Ӧ����
	KEY_Init();			//����IO��ʼ��
	EXTIX_Init();		//�ⲿ�жϳ�ʼ��
	
  	while(1) 
	{
		while(!dir)
		{
			POINT_COLOR=RED; 		//������ɫ�Ǻ�ɫ
			LCD_ShowString_hanzi1(i,150,200,12,12,"�Ұ����ߵ磬��ټ�ѵʹ�ҿ���");
			delay_ms(speed);
			POINT_COLOR=WHITE; 		//������ɫ�ǰ�ɫ
			LCD_ShowString(i-1,150,1,12,12,"1");
			i++;
			if(i>=240) i = 0;
		}
		while(dir)
		{
			POINT_COLOR=RED; 		//������ɫ�Ǻ�ɫ
			LCD_ShowString_hanzi1(i,150,200,12,12,"�Ұ����ߵ磬��ټ�ѵʹ�ҿ���");
			delay_ms(speed);
			POINT_COLOR=WHITE; 		//������ɫ�ǰ�ɫ
			LCD_ShowString(i+167,150,1,12,12,"1");
			i--;
			if(i<=-168) i = 240;
		}
	} 
}


