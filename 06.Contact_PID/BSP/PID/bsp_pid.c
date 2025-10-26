#include "bsp_pid.h"

//位置式 Positional type
#define Position_KP (0.8)
#define Position_KI (0.001)
#define Position_KD (0.5)

//增量式 Incremental 
#define Incremental_KP (20)
#define Incremental_KI (1)
#define Incremental_KD (2)


extern int16_t Encoder_Left ,Encoder_Right;
extern int16_t Target_Velocity ,Target_Position;
extern int16_t Position_L,Position_R;

int motor_L,motor_R;//PWM脉冲 PWM pulse
int Position_Motor_L,Position_Motor_R;//位置pid算到的值 Position PID calculated value

// TIM6中断 10ms进一次
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //检查TIM更新中断发生与否 Check whether TIM update interruption occurs
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);    //清除TIMx更新中断标志 Clear TIMx update interrupt flag
		
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
		
		//限幅 Limit
		motor_L = PWM_Limit(motor_L,2500,-2500);
		motor_R = PWM_Limit(motor_R,2500,-2500);
		
		Set_Pwm(motor_L,motor_R);
		

		
	}
}

/**************************************************************************
Function Function: Position based PID Controller
Entrance parameters: encoder measurement position information, target position
Return value: Motor PWM
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM
**************************************************************************/
int Position_PID (int position,int target)
{ 	
		static float error,Pwm,Integral_error,Last_error;
		error=target-position;                                  //计算偏差 Calculate deviation
		Integral_error+=error;	                                 //求出偏差的积分 Calculate the integral of the deviation

		Integral_error=PWM_Limit(Integral_error,Target_Velocity,-Target_Velocity); //积分限幅 Integral limit

		Pwm=Position_KP*error+Position_KI*Integral_error+Position_KD*(error-Last_error);       //位置式PID控制器 Position based PID controller

		Last_error=error;                                       //保存上一次偏差  Save the previous deviation
	
		Pwm=PWM_Limit(Pwm,Target_Velocity,-Target_Velocity);  //位置转增量输出限幅 Position conversion increase output limit
		
		return Pwm;                                           //增量输出 Incremental output
}




/**************************************************************************
Function Function: Incremental PI Controller
Entry parameters: encoder measurement value, target speed
Return value: Motor PWM
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
**************************************************************************/
int Incremental_PI (int Encoder,int Target)
{ 	
		static float error,Pwm,Last_error,Last_last_error; 
		error=Target-Encoder;                                  //计算偏差  Calculate deviation
		Pwm+=Incremental_KP*(error-Last_error)+
				 Incremental_KI*error + 
				 Incremental_KD*(error-2*Last_error+Last_last_error);   //增量式PID控制器 Incremental PID controller

		Last_error=error;	                                   //保存上一次偏差  Save the previous deviation
		Last_last_error = Last_error;  //保存上上次偏差 Save the previous deviation

		return Pwm;                                           //增量输出 Incremental output
}


