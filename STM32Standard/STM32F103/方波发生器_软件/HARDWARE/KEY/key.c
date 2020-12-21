#include "key.h"
#include "delay.h"

//������ʼ������
//PA15 PC5 PA0 ���ó�����
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructrue;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|
						RCC_APB2Periph_GPIOC,ENABLE);	//ʹ��PA,PC�˿�ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//�ر�JTAG��ʹ��SWD��������SWDģʽ���ԣ�PA15ռ����JTAGһ��IO��
	
	GPIO_InitStructrue.GPIO_Pin = GPIO_Pin_15;			//KEY1-->PA.15�˿�����
	GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;	//�������
	GPIO_Init(GPIOA,&GPIO_InitStructrue);				//��ʼ��GPIOA.15

	GPIO_InitStructrue.GPIO_Pin = GPIO_Pin_5;			//KEY0-->PC.5�˿�����
	GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;	//�������
	GPIO_Init(GPIOC,&GPIO_InitStructrue);				//��ʼ��GPIOC.5
	
	GPIO_InitStructrue.GPIO_Pin = GPIO_Pin_0;			//WK UP-->PA.0�˿�����
	GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;	//�������
	GPIO_Init(GPIOA,&GPIO_InitStructrue);				//��ʼ��GPIOA.0
	
}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ��
//0��û���κΰ�������
//KEY0_PRES�� KEY0 ����
//KEY1_PRES�� KEY1 ����
//WKUP_PRES�� WK_UP ����
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP
u8 KEY_Scan(u8 mode)
{
	static u8 key_up = 1;			//�����ɰ��±�־
	if(mode) key_up = 1;			//֧�����������ĳ������һֱ���ţ����һֱ������ֵ
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//ȥ����
		key_up = 0;//��ֹ��һ�ζ�δ���
		if(KEY0==0) return KEY0_PRES;
		else if(KEY1==0) return KEY1_PRES;
		else if(WK_UP) return WK_UP_PRES;
	}
	else if(KEY0==1&&KEY1==1&&WK_UP==0) key_up = 1;//��ȫ����ʱ������
	return 0;//�ް�������
}



































