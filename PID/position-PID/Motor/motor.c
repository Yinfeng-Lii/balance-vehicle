#include "motor.h"
/**************************************************************************
Function: Initialize Motor Interface
Input   : none
Output  : none
�������ܣ���ʼ������ӿ�
��ڲ�������
����  ֵ����
**************************************************************************/
void BalanceCar_Motor_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //ʹ��PC�˿�ʱ�� Enable PC port clock
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	//�˿����� port configuration
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;      //������� Push pull output
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //50M
  GPIO_Init(GPIOC, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOC Initialize GPOC according to the set parameters
}



/**************************************************************************
Function: Initialize PWM to drive motor
Input   : arr��Auto reload value��psc��Prescaler coefficient
Output  : none
�������ܣ���ʼ��PWM������������� 
��ڲ�����arr���Զ���װֵ��psc��Ԥ��Ƶϵ��
����  ֵ����
**************************************************************************/
void BalanceCar_PWM_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	TIM_DeInit(TIM8);
	TIM_TimeBaseStructure.TIM_Period = arr - 1; //Set the value of the automatic reload register cycle for the next update event loading activity
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //Set the prescaler value used as the divisor of the TIMx clock frequency. No division
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //TDTS = Tck_tim Set clock division: TDTS=Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM Up Counting Mode
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //Initialize the time base unit of TIMx based on the parameters specified in TIM_TimeBaseInitStruct
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //Select Timer Mode: TIM Pulse Width Modulation Mode 1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // Compare output enablement
	TIM_OCInitStructure.TIM_Pulse = 0;                            //Set the pulse value to be loaded into the capture comparison register
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //Output polarity: TIM output has a relatively high polarity
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //TIMx  Initialize peripheral TIMx based on the parameters specified in TIM_SICInitStruct
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //TIMx  Initialize peripheral TIMx based on the parameters specified in TIM_SICInitStruct
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);  //TIMx  Initialize peripheral TIMx based on the parameters specified in TIM_SICInitStruct
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);  //TIMx  Initialize peripheral TIMx based on the parameters specified in TIM_SICInitStruct
	TIM_CtrlPWMOutputs(TIM8,ENABLE);	//Main output enable
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH1 pre load enable
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH2 pre load enable
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH3 pre load enable
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH4 pre load enable
	
	TIM_ARRPreloadConfig(TIM8, ENABLE); //Enable TIMx pre loaded registers on ARR
	TIM_Cmd(TIM8, ENABLE);                   //Enable Timer 8

}
