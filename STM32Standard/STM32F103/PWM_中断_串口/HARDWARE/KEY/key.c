#include "key.h"
#include "delay.h"

//按键初始化函数
//PA15 PC5 PA0 设置成输入
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructrue;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|
						RCC_APB2Periph_GPIOC,ENABLE);	//使能PA,PC端口时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭JTAG，使能SWD，可以用SWD模式调试（PA15占用了JTAG一个IO）
	
	GPIO_InitStructrue.GPIO_Pin = GPIO_Pin_15;			//KEY1-->PA.15端口配置
	GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;	//上拉输出
	GPIO_Init(GPIOA,&GPIO_InitStructrue);				//初始化GPIOA.15

	GPIO_InitStructrue.GPIO_Pin = GPIO_Pin_5;			//KEY0-->PC.5端口配置
	GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;	//上拉输出
	GPIO_Init(GPIOC,&GPIO_InitStructrue);				//初始化GPIOC.5
	
	GPIO_InitStructrue.GPIO_Pin = GPIO_Pin_0;			//WK UP-->PA.0端口配置
	GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;	//下拉输出
	GPIO_Init(GPIOA,&GPIO_InitStructrue);				//初始化GPIOA.0
	
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//返回值：
//0，没有任何按键按下
//KEY0_PRES， KEY0 按下
//KEY1_PRES， KEY1 按下
//WKUP_PRES， WK_UP 按下
//注意此函数有响应优先级,KEY0>KEY1>WK_UP
u8 KEY_Scan(u8 mode)
{
	static u8 key_up = 1;			//按键可按下标志
	if(mode) key_up = 1;			//支持连按，如果某个按键一直按着，则会一直返回其值
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//去抖动
		key_up = 0;//防止按一次多次触发
		if(KEY0==0) return KEY0_PRES;
		else if(KEY1==0) return KEY1_PRES;
		else if(WK_UP) return WK_UP_PRES;
	}
	else if(KEY0==1&&KEY1==1&&WK_UP==0) key_up = 1;//完全弹起时可允许按
	return 0;//无按键按下
}



































