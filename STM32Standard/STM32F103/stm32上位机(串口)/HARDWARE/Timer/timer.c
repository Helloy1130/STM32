#include "timer.h"

//通用定时器 2 初始化
//这里时钟选择为 APB1 的 2 倍，而 APB1 为 36M，因此为72M
//arr：自动重装值。
//psc：时钟预分频数

void TIM2_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//时钟 TIM2 使能
	
	//定时器 TIM2 初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //初始化 TIM2
	
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);//设置TIME输出触发为更新模式
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = arr-100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//如果PWM1要为low PWM2要为High
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
}


//通用定时器 3 初始化
//这里时钟选择为 APB1 的 2 倍，而 APB1 为 36M
//arr：自动重装值。
//psc：时钟预分频数
//void TIM3_Init(u16 arr, u16 psc)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	//TIM_OCInitTypeDef TIM_OCInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//时钟 TIM3 使能
//	
//	//定时器 TIM2 初始化
//	TIM_TimeBaseStructure.TIM_Period = arr; //设置自动重装载寄存器周期的值
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置时钟频率除数的预分频值
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //初始化 TIM3
//	
//	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);//设置TIME输出触发为更新模式 
//	TIM_Cmd(TIM3,ENABLE);//使能定时中断
          
//}
	






