#include "bsp_timer.h"


u16 led_flag = 0; //1:进入闪烁状态 0:等待闪烁 //1: Entering flashing state 0: waiting for flashing
u16 led_twinkle_count = 0;// 闪烁计数 //Flashing Count

u16 led_count = 0; //开始计数 //Start counting




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

	//中断优先级NVIC设置  Interrupt priority NVIC setting
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;			  //TIM6中断 TIM6 interrupt
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; //先占优先级4级 Preemption priority level 4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //从优先级2级 From priority level 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ通道被使能 IRQ channel is enabled
	NVIC_Init(&NVIC_InitStructure);							  //初始化NVIC寄存器 Initialize NVIC registers

	TIM_Cmd(TIM6, ENABLE);
}




// TIM6中断 //TIM6 Interrupt service
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //检查TIM更新中断发生与否  Check whether TIM update interruption occurs
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);    //清除TIMx更新中断标志  Clear TIMx update interrupt flag
		led_count++;  //led服务显示标志 LED service display logo
		cotrol_led();//灯服务		3s内 灯快闪3次	 led service
		
	}
}


void cotrol_led(void)
{
		if(!led_flag)
		{
			if(led_count>300)//3S
			{
				led_count = 0;
				led_flag = 1;
			}
		}
		else
		{
			if(led_count>20)//200ms
			{
				led_count = 0;	
				LED = !LED;//状态反转 //State reversal
			
				led_twinkle_count++;
				if(led_twinkle_count == 6)
				{					
					LED = 0;
					led_twinkle_count = 0;
					led_flag = 0;
				}
				
			}
		}

}

