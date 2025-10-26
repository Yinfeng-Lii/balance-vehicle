#include "bsp_timer.h"


/**************************************************************************
Function function: TIM6 initialization, timed for 10 milliseconds
Entrance parameters: None
Return value: None
函数功能：TIM6初始化，定时10毫秒
入口参数：无
返回  值：无
**************************************************************************/
void TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //使能定时器的时钟  Enable the clock of the timer
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;			 // 预分频器  Prescaler
	TIM_TimeBaseStructure.TIM_Period = 99;				 //设定计数器自动重装值  Set the automatic reset value of the counter
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);                //清除TIM的更新标志位 Clear the update flag of TIM
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	//中断优先级NVIC设置 Interrupt priority NVIC setting
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;			  //TIM6中断 TIM6 interrupt
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; //先占优先级4级 Prioritize Level 4 first
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //从优先级2级 From priority level 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ通道被使能 IRQ channel enabled
	NVIC_Init(&NVIC_InitStructure);							  //初始化NVIC寄存器 Initialize NVIC registers

	TIM_Cmd(TIM6, ENABLE);
}





