#include "bsp_timer.h"


u16 led_flag = 0; //1:������˸״̬ 0:�ȴ���˸ //1: Entering flashing state 0: waiting for flashing
u16 led_twinkle_count = 0;// ��˸���� //Flashing Count

u16 led_count = 0; //��ʼ���� //Start counting




/**************************************************************************
Function function: TIM6 initialization, timed for 10 milliseconds
Entrance parameters: None
Return value: None
�������ܣ�TIM6��ʼ������ʱ10����
��ڲ�������
����  ֵ����
**************************************************************************/
void TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʹ�ܶ�ʱ����ʱ��  Enable the clock of the timer
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;			 // Ԥ��Ƶ��  Prescaler
	TIM_TimeBaseStructure.TIM_Period = 99;				 //�趨�������Զ���װֵ  Set the automatic reset value of the counter
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);                //���TIM�ĸ��±�־λ Clear the update flag of TIM
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	//�ж����ȼ�NVIC����  Interrupt priority NVIC setting
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;			  //TIM6�ж� TIM6 interrupt
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; //��ռ���ȼ�4�� Preemption priority level 4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //�����ȼ�2�� From priority level 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQͨ����ʹ�� IRQ channel is enabled
	NVIC_Init(&NVIC_InitStructure);							  //��ʼ��NVIC�Ĵ��� Initialize NVIC registers

	TIM_Cmd(TIM6, ENABLE);
}




// TIM6�ж� //TIM6 Interrupt service
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //���TIM�����жϷ������  Check whether TIM update interruption occurs
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);    //���TIMx�����жϱ�־  Clear TIMx update interrupt flag
		led_count++;  //led������ʾ��־ LED service display logo
		cotrol_led();//�Ʒ���		3s�� �ƿ���3��	 led service
		
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
				LED = !LED;//״̬��ת //State reversal
			
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

