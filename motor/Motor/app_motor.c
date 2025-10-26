#include "app_motor.h"

float Velocity_Left,Velocity_Right;	//�����ٶ�(mm/s)  Wheel speed (mm/s)

#define MOTOR_IGNORE_PULSE (1300)//����  1450 25Khz   ��ֵ��Ҫ����ֹ״̬΢��   Dead zone   1450 25Khz   This value needs to be fine-tuned in static state

//�������� Filter dead zone
int PWM_Ignore(int pulse)
{
	if (pulse > 0) return pulse + MOTOR_IGNORE_PULSE;
  if (pulse < 0) return pulse - MOTOR_IGNORE_PULSE;
	return pulse;
}

int myabs(int a)
{ 		   
	int temp;
	if(a<0)  temp=-a;  
	else temp=a; 
	return temp;
}

/**************************************************************************
Function: Assign to PWM register
Input   : motor_left��Left wheel PWM��motor_right��Right wheel PWM
Output  : none
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int motor_left,int motor_right)
{
	if(motor_left == 0)//ͣ�� stop
	{
		L_PWMA = 0;
		L_PWMB = 0;
	}
	if(motor_right == 0)
	{
		R_PWMA = 0;
		R_PWMB = 0;
	}
	
	//���� left
  if(motor_left>0)	 //ǰ��   go ahead
	{
		L_PWMB = myabs(motor_left);
		L_PWMA = 0;
	}		
	else
	{
		L_PWMB = 0;
		L_PWMA = myabs(motor_left);
	}
	
	//���� right
	if(motor_right>0) //ǰ�� go ahead
	{
		R_PWMA = myabs(motor_right);
		R_PWMB = 0;
	}
	else //���� back
	{
		R_PWMA = 0;
		R_PWMB = myabs(motor_right);	
	}

}


/**************************************************************************
Function: PWM limiting range
Input   : IN��Input  max��Maximum value  min��Minimum value
Output  : Output
�������ܣ�����PWM��ֵ 
��ڲ�����IN���������  max���޷����ֵ  min���޷���Сֵ
����  ֵ���޷����ֵ
**************************************************************************/
int PWM_Limit(int IN,int max,int min)
{
	int OUT = IN;
	if(OUT>max) OUT = max;
	if(OUT<min) OUT = min;
	return OUT;
}


/**************************************************************************
Function: Encoder reading is converted to speed (mm/s)
Input   : none
Output  : none
�������ܣ�����������ת��Ϊ�ٶȣ�mm/s��
��ڲ�������
����  ֵ����
**************************************************************************/
void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right)
{ 	
	float Rotation_Speed_L,Rotation_Speed_R;						//���ת��  ת��=������������5msÿ�Σ�*��ȡƵ��/��Ƶ��/���ٱ�/���������� //Motor speed=Encoder reading (5ms each time) * Reading frequency/harmonics/reduction ratio/Encoder accuracy
	Rotation_Speed_L = encoder_left*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Left = Rotation_Speed_L*PI*Diameter_67;		//����������ٶ�=ת��*�ܳ� Calculate encoder speed=rotational speed * circumference
	Rotation_Speed_R = encoder_right*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Right = Rotation_Speed_R*PI*Diameter_67;		//����������ٶ�=ת��*�ܳ� Calculate encoder speed=rotational speed * circumference
	
}





