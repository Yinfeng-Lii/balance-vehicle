#ifndef __APP_MOTOR_H_
#define __APP_MOTOR_H_

#include "ALLHeader.h" 

#define PI 3.14159265							//PIԲ����
#define Control_Frequency  200.0	//��������ȡƵ��
#define Diameter_67  67.0 				//����ֱ��67mm 
#define EncoderMultiples   4.0 		//��������Ƶ��
#define Encoder_precision  11.0 	//���������� 11��
#define Reduction_Ratio  30.0			//���ٱ�30
#define Perimeter  210.4867 			//�ܳ�����λmm


void Set_Pwm(int motor_left,int motor_right);
int PWM_Limit(int IN,int max,int min);

void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right);
uint8_t Turn_Off(float angle, float voltage);
	
int PWM_Ignore(int pulse);

#endif

