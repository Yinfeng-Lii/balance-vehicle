#include "bsp_timer.h"


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

	//�ж����ȼ�NVIC���� Interrupt priority NVIC setting
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;			  //TIM6�ж� TIM6 interrupt
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; //��ռ���ȼ�4�� Prioritize Level 4 first
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //�����ȼ�2�� From priority level 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQͨ����ʹ�� IRQ channel enabled
	NVIC_Init(&NVIC_InitStructure);							  //��ʼ��NVIC�Ĵ��� Initialize NVIC registers

	TIM_Cmd(TIM6, ENABLE);
}





