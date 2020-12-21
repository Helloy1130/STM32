#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "iwdg.h"


//�����ÿ��Ź��� DS0 ����������� WK_UP �������£���ι����ֻҪ WK_UP ��ͣ�İ���
//���Ź���һֱ���������λ������ DS0 �ĳ�����һ���������Ź������ʱ�䣨Tout����û����
//��ô���ᵼ�³����������⽫���� DS0 Ϩ��һ�Ρ�

int main(void)
{	
	delay_init();
	LED_Init();
	KEY_Init();
	
	delay_ms(300);   	 //���˿��õ���
	
	IWDG_Init(4,625);    //���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s
	LED0=0;				 //����LED0
	
	while(1)
	{
		if(KEY_Scan(0)==WK_UP_PRES)	IWDG_Feed();//���WK_UP����,��ι��
		delay_ms(10);
	}
}
