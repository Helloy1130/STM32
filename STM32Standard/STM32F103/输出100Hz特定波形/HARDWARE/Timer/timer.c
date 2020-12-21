#include "timer.h"
#include "delay.h"
#include "usart.h"


//通用定时器 2 中断初始化
//这里时钟选择为 APB1 的 2 倍，而 APB1 为 36M
//arr：自动重装值。
//psc：时钟预分频数
void TIM2_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//时钟 TIM2 使能
	
	//定时器 TIM2 初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割
	//TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //初始化 TIM2
	
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);//设置TIME输出触发为更新模式
	
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //允许更新中断
	
//	//中断优先级 NVIC 设置
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3 中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级 0 级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级 3 级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道被使能
//	NVIC_Init(&NVIC_InitStructure); //初始化 NVIC 寄存器
	
	//TIM_Cmd(TIM3, ENABLE); //使能 TIM3
	
}

////定时器 3 中断服务程序
////如果中断触发（溢出触发）
//void TIM3_IRQHandler(void)
//{
//	u16 vout;
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查 TIM3 更新中断发生与否
//	{
////		if(flag==1)
////		{
////			if(count<50) 
////			{
////				vout = 0;
////				//printf("%d\r\n",vout);
////				Dac1_Set_Vol(vout);
////				count++;
////			}
////			else if(count<100)
////			{
////				vout = 1000;
////				//printf("%d\r\n",vout);
////				Dac1_Set_Vol(vout);
////				count++;
////			}
////			else 
////			{
////				count = 0;
////			}
////		}
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update ); //清除 TIM3 更新中断标志
//	}
//}
















