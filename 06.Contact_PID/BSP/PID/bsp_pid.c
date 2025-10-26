#include "bsp_pid.h"

//λ��ʽ Positional type
#define Position_KP (0.8)
#define Position_KI (0.001)
#define Position_KD (0.5)

//����ʽ Incremental 
#define Incremental_KP (20)
#define Incremental_KI (1)
#define Incremental_KD (2)


extern int16_t Encoder_Left ,Encoder_Right;
extern int16_t Target_Velocity ,Target_Position;
extern int16_t Position_L,Position_R;

int motor_L,motor_R;//PWM���� PWM pulse
int Position_Motor_L,Position_Motor_R;//λ��pid�㵽��ֵ Position PID calculated value

// TIM6�ж� 10ms��һ��
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //���TIM�����жϷ������ Check whether TIM update interruption occurs
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);    //���TIMx�����жϱ�־ Clear TIMx update interrupt flag
		
		Encoder_Left = Read_Encoder(MOTOR_ID_ML);          					
		Encoder_Right = -Read_Encoder(MOTOR_ID_MR);   
		
		Position_L +=Encoder_Left;
		Position_R +=Encoder_Right;
		
		Position_Motor_L = Position_PID(Position_L,Target_Position);
		Position_Motor_R = Position_PID(Position_R,Target_Position);
		
		motor_L = Incremental_PI(Encoder_Left,Position_Motor_L);
		motor_R = Incremental_PI(Encoder_Right,Position_Motor_R);
		
		motor_L = PWM_Ignore(motor_L);
		motor_R = PWM_Ignore(motor_R);
		
		//�޷� Limit
		motor_L = PWM_Limit(motor_L,2500,-2500);
		motor_R = PWM_Limit(motor_R,2500,-2500);
		
		Set_Pwm(motor_L,motor_R);
		

		
	}
}

/**************************************************************************
Function Function: Position based PID Controller
Entrance parameters: encoder measurement position information, target position
Return value: Motor PWM
�������ܣ�λ��ʽPID������
��ڲ���������������λ����Ϣ��Ŀ��λ��
����  ֵ�����PWM
**************************************************************************/
int Position_PID (int position,int target)
{ 	
		static float error,Pwm,Integral_error,Last_error;
		error=target-position;                                  //����ƫ�� Calculate deviation
		Integral_error+=error;	                                 //���ƫ��Ļ��� Calculate the integral of the deviation

		Integral_error=PWM_Limit(Integral_error,Target_Velocity,-Target_Velocity); //�����޷� Integral limit

		Pwm=Position_KP*error+Position_KI*Integral_error+Position_KD*(error-Last_error);       //λ��ʽPID������ Position based PID controller

		Last_error=error;                                       //������һ��ƫ��  Save the previous deviation
	
		Pwm=PWM_Limit(Pwm,Target_Velocity,-Target_Velocity);  //λ��ת��������޷� Position conversion increase output limit
		
		return Pwm;                                           //������� Incremental output
}




/**************************************************************************
Function Function: Incremental PI Controller
Entry parameters: encoder measurement value, target speed
Return value: Motor PWM
�������ܣ�����PI������
��ڲ���������������ֵ��Ŀ���ٶ�
����  ֵ�����PWM
**************************************************************************/
int Incremental_PI (int Encoder,int Target)
{ 	
		static float error,Pwm,Last_error,Last_last_error; 
		error=Target-Encoder;                                  //����ƫ��  Calculate deviation
		Pwm+=Incremental_KP*(error-Last_error)+
				 Incremental_KI*error + 
				 Incremental_KD*(error-2*Last_error+Last_last_error);   //����ʽPID������ Incremental PID controller

		Last_error=error;	                                   //������һ��ƫ��  Save the previous deviation
		Last_last_error = Last_error;  //�������ϴ�ƫ�� Save the previous deviation

		return Pwm;                                           //������� Incremental output
}


