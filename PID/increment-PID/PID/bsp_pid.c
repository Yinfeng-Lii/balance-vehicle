#include "bsp_pid.h"


#define Incremental_KP (20)
#define Incremental_KI (8)
#define Incremental_KD (2)


extern int16_t Encoder_Left ,Encoder_Right;
extern int16_t Left_target ,Right_target;

int motor_L,motor_R;

// TIM6�ж� 10ms��һ�� TIM6 interrupts once every 10ms
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //���TIM�����жϷ������ Check whether TIM update interruption occurs
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);    //���TIMx�����жϱ�־ Clear TIMx update interrupt flag
		
		Encoder_Left = Read_Encoder(MOTOR_ID_ML);          					
		Encoder_Right = -Read_Encoder(MOTOR_ID_MR);   
		
		motor_L = Incremental_PI(Encoder_Left,Left_target);//��ߵ��   Left motor
		motor_R = Incremental_PI(Encoder_Right,Right_target);//�ұߵ�� Right motor
		
		motor_L = PWM_Ignore(motor_L);
		motor_R = PWM_Ignore(motor_R);
		
		//�޷� Limiting amplitude
		motor_L = PWM_Limit(motor_L,2500,-2500);
		motor_R = PWM_Limit(motor_R,2500,-2500);
		
		Set_Pwm(motor_L,motor_R);
		

		
	}
}


/**************************************************************************
Function Function: Incremental PI Controller
Entry parameters: encoder measurement value, target speed
Return value: Motor PWM
�������ܣ�����PI������
��ڲ���������������ֵ��Ŀ���ٶ�
����  ֵ�����PWM
��������ʽ��ɢPID��ʽ 
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)��������ƫ�� 
e(k-1)������һ�ε�ƫ��  �Դ����� 
pwm�����������
**************************************************************************/
int Incremental_PI (int Encoder,int Target)
{ 	
		static float error,Pwm,Last_error,Last_last_error;
		error=Target-Encoder;                                  //Calculate deviation
		Pwm+=Incremental_KP*(error-Last_error)+
				 Incremental_KI*error + 
				 Incremental_KD*(error-2*Last_error+Last_last_error);   //Incremental PID controller

		Last_error=error;	                                   //Save the previous deviation
		Last_last_error = Last_error;  //Save the previous deviation

		return Pwm;                                           //Incremental output
}


