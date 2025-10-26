#include "bsp_pid.h"


#define Position_KP (2.2)
#define Position_KI (0.013)
#define Position_KD (0.05)


extern int16_t Position_Left ,Position_Right;
extern int16_t Left_target ,Right_target;

int motor_L,motor_R;

// TIM6中断 10ms进一次 TIM6 interruption: once every 10ms
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //检查TIM更新中断发生与否 Check whether TIM update interruption occurs
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);    //清除TIMx更新中断标志 Clear TIMx update interrupt flag
		
		Position_Left = Read_Position(MOTOR_ID_ML);          					
		Position_Right = -Read_Position(MOTOR_ID_MR);   
		
		motor_L = Position_PID(Position_Left,Left_target);//左边电机  left motor
		motor_R = Position_PID(Position_Right,Right_target);//右边电机 right motor
		
		motor_L = PWM_Ignore(motor_L);
		motor_R = PWM_Ignore(motor_R);
		
		//限幅 limit
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
根据位置式离散PID公式 
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
pwm代表输出
**************************************************************************/
int Position_PID (int position,int target)
{ 	
		static float error,Pwm,Integral_error,Last_error;
		error=target-position;                                  //计算偏差 Calculate deviation
		Integral_error+=error;	                                 //求出偏差的积分 //Calculate the integral of the deviation
		if(Integral_error>1000)Integral_error=1000;
		if(Integral_error<-1000)Integral_error=-1000;

		Pwm=Position_KP*error+Position_KI*Integral_error+Position_KD*(error-Last_error);       //位置式PID控制器 Position based PID controller

		Last_error=error;                                       //保存上一次偏差  Save the previous deviation
		return Pwm;                                           //增量输出 Incremental output
}

