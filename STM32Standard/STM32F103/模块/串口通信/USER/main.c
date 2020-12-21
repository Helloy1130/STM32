#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key.h"

//ͨ������ͨ�ţ��������Ͳ�ͬ��ֵ

int main(void)
{
	u8 t;
	delay_init(); 									//��ʱ������ʼ��
	uart_init(9600); 								//���ڳ�ʼ��Ϊ 9600
	KEY_Init(); 									//��ʼ��������������ӵ�Ӳ���ӿ�
	
	while(1)
	{
		t = KEY_Scan(0); 							//�õ�ĳ�������ı�־ֵ
		switch(t)
		{
			case KEY0_PRES:
							USART_SendData(USART1, 0X01); //�򴮿� 1 ��������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
							break;
			case KEY1_PRES:
							USART_SendData(USART1, 0X02); //�򴮿� 1 ��������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
							break;
			case WK_UP_PRES:
							USART_SendData(USART1, 0X03); //�򴮿� 1 ��������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
							break;
			default:
					delay_ms(10);
		}
	}	
}

