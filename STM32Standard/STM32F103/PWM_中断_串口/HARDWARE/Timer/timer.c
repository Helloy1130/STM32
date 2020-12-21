#include "timer.h"
#include "usart.h"

extern u16 PWM_frequency;

//通用定时器 3 中断初始化
//这里时钟选择为 APB1 的 2 倍，而 APB1 为 36M,因此为72M
//arr：自动重装值。
//psc：时钟预分频数
void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//时钟 TIM3 使能
	
	//定时器 TIM3 初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //初始化 TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //允许更新中断
	
	//中断优先级 NVIC 设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级 0 级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级 3 级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道被使能
	NVIC_Init(&NVIC_InitStructure); //初始化 NVIC 寄存器
	
	TIM_Cmd(TIM3, ENABLE); //使能 TIM3
	
}

//定时器 3 中断服务程序
//如果中断触发（溢出触发）
void TIM3_IRQHandler(void)
{
	u8 i;
	u8 PWM_f[3];
	PWM_f[0]=PWM_frequency/100+48; //取百位
	PWM_f[1]=(PWM_frequency/10)%10+48; //取十位
	PWM_f[2]=PWM_frequency%10+48; //取个位
;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查 TIM3 更新中断发生与否
	{
		printf("PWM的频率是: ");
		for(i=0;i<3;i++)
		{
			USART1->DR=PWM_f[i];
			while((USART1->SR&0X40)==0);//等待发送结束
		}
		printf("KHz");
		printf("\r\n\r\n");//换行
		while((USART1->SR&0X40)==0);//等待发送结束
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update ); //清除 TIM3 更新中断标志
	}
}
















