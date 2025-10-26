#include "bsp_pid.h"


#define Position_KP (2.2)
#define Position_KI (0.013)
#define Position_KD (0.05)


extern int16_t Position_Left ,Position_Right;
extern int16_t Left_target ,Right_target;

int motor_L,motor_R;

// TIM6�ж� 10ms��һ�� TIM6 interruption: once every 10ms
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //���TIM�����жϷ������ Check whether TIM update interruption occurs
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);    //���TIMx�����жϱ�־ Clear TIMx update interrupt flag
		
		Position_Left = Read_Position(MOTOR_ID_ML);          					
		Position_Right = -Read_Position(MOTOR_ID_MR);   
		
		motor_L = Position_PID(Position_Left,Left_target);//��ߵ��  left motor
		motor_R = Position_PID(Position_Right,Right_target);//�ұߵ�� right motor
		
		motor_L = PWM_Ignore(motor_L);
		motor_R = PWM_Ignore(motor_R);
		
		//�޷� limit
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
����λ��ʽ��ɢPID��ʽ 
pwm=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
pwm�������
**************************************************************************/
int Position_PID (int position,int target)
{ 	
		static float error,Pwm,Integral_error,Last_error;
		error=target-position;                                  //����ƫ�� Calculate deviation
		Integral_error+=error;	                                 //���ƫ��Ļ��� //Calculate the integral of the deviation
		if(Integral_error>1000)Integral_error=1000;
		if(Integral_error<-1000)Integral_error=-1000;

		Pwm=Position_KP*error+Position_KI*Integral_error+Position_KD*(error-Last_error);       //λ��ʽPID������ Position based PID controller

		Last_error=error;                                       //������һ��ƫ��  Save the previous deviation
		return Pwm;                                           //������� Incremental output
}

