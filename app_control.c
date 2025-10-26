#include "app_control.h"


static u16 intstop_time =0 ;
float battery = 12;//��ʼ״̬�������� 12v The initial state is fully charged 12v
u8 ccd_conut = 0;
u8 ps2_conut = 0;


//�ⲿ�ж����ӳ� ����10ms���ӳ� �˷�����delay׼ȷ
//External interrupt delay at least 10ms This method is more accurate than delay
void delay_time_int(u16 time)
{
	intstop_time = time*2; //Ҫ*5��������ʱ�� //*5 is the final time
//	while(intstop_time); 
}


void set_time_int(u16 time)
{
	intstop_time = time; 
}

//����ʱ�� Return time
u16 get_time_int(void)
{
	return intstop_time;
}



void EXTI15_10_IRQHandler(void)
{
	int Encoder_Left,Encoder_Right;             					//���ұ�������������� Pulse counting of left and right encoders
	int Balance_Pwm,Velocity_Pwm,Turn_Pwm;		  					//ƽ�⻷PWM�������ٶȻ�PWM������ת��PWM�� Balance loop PWM variable, speed loop PWM variable, steering loop PWM variable
	
  // ����Ƿ����ж��¼�  Check if any interruption events have occurred
  if(MPU6050_INT==0)		
	{   
		EXTI->PR=1<<12;                           					//����жϱ�־λ Clear interrupt flag bit

		if(intstop_time>0)
		{
				intstop_time --;
		}
			
		if(mode == PS2_Control)
		{
				ps2_conut++;
			if(ps2_conut>20)//100ms
			{
				ps2_conut =0;
				PS2_Contorl_Car();
				
			}
		}
		else if(mode == Line_Track)
		{
			Set_IRtrack_speed(); //����Ѳ���ٶ� Infrared patrol speed
		}
		else if(mode == Diff_Line_track)
		{
			Set_IRtrack_speed(); //���Ѷ�Ѳ���ٶ� High-difficulty line patrol speed
		}
		else if(mode == CCD_Mode)
		{
			ccd_conut++;
			if(ccd_conut >4)//25ms
			{
				ccd_conut=0;
				Set_CCDtrack_speed(); //CCDѲ���ٶ� CCD patrol speed
				CCDShowBuf = CCD_Get_ADC_128X32(); //��ʾͼ��  Display image
				deal_data_ccd();//��ȡ��ֵ Get median
			}
		}
		else if (mode == ElE_Mode)
		{
			getEleData();
			Set_eletrack_speed(); //���Ѳ���ٶ� Electromagnetic patrol speed
		}
		
		else if (mode == K210_Line)//k210Ѳ�� K210 patrol line
		{
			Set_K210track_speed();
		}
		else if(mode == K210_Follow)
		{
			APP_K210X_Y_Follow_PID();//k210��ɫ���� K210 Color Follow
		}
		else if(mode == LiDar_Line)
		{
			LiDar_Straight(); //�״�Ѳǽ�� Radar patrol wall
		}
		else if(mode == LiDar_wall_Line)
		{
			LiDar_Straight_wall(); //�״�����ǽ���� The radar is walking along the wall
		}
		else if(mode == LiDar_aralm) //�״ﾯ�� Radar Guard
		{
			Get_DIS_Group();
		}
		
		
		
		
		Get_Angle(GET_Angle_Way);                     			//������̬��5msһ�Σ����ߵĲ���Ƶ�ʿ��Ը��ƿ������˲��ͻ����˲���Ч��  //Updating the posture once every 5ms, a higher sampling frequency can improve the effectiveness of Kalman filtering and complementary filtering
		Encoder_Left=Read_Encoder(MOTOR_ID_ML);            					//��ȡ���ֱ�������ֵ��ǰ��Ϊ��������Ϊ��   //Read the value of the left wheel encoder, forward is positive, backward is negative
		Encoder_Right=-Read_Encoder(MOTOR_ID_MR);           					//��ȡ���ֱ�������ֵ��ǰ��Ϊ��������Ϊ��   //Read the value of the right wheel encoder, forward is positive, backward is negative
		Get_Velocity_Form_Encoder(Encoder_Left,Encoder_Right); //��ȡ�ٶ� Obtain speed
				
		Balance_Pwm=Balance_PD(Angle_Balance,Gyro_Balance);    //ƽ��PID���� Gyro_Balanceƽ����ٶȼ��ԣ�ǰ��Ϊ��������Ϊ��   //Balance PID control gyro balance angular velocity polarity: forward tilt is positive, backward tilt is negative
		Velocity_Pwm=Velocity_PI(Encoder_Left,Encoder_Right);  //�ٶȻ�PID����	��ס���ٶȷ�����������   //Speed loop PID control. Remember, speed feedback is positive feedback
			

		//ת��PID����  Steering loop PID control
		if(mode == Line_Track || mode == Diff_Line_track)//��·Ѳ�� Four Route Patrol ���Ѷ�Ѳ�� High-difficulty line patrol
		{
			Turn_Pwm=Turn_IRTrack_PD(Gyro_Turn); 
		}
		else if(mode == CCD_Mode) //CCDѲ�� CCD patrol line
		{
			Turn_Pwm=Turn_CCD_PD(Gyro_Turn);
		}
		else if(mode == ElE_Mode) //���Ѳ�� Electromagnetic patrol line
		{
			Turn_Pwm=Turn_ELE_PD(Gyro_Turn);
		}
		else if (mode == K210_Line)
		{
			Turn_Pwm=Turn_K210_PD(Gyro_Turn);		
		}
		else
		{
			Turn_Pwm=Turn_PD(Gyro_Turn);
		} 
		
																
	
		
		
		Motor_Left=Balance_Pwm+Velocity_Pwm+Turn_Pwm;       //�������ֵ������PWM Calculate the final PWM of the left wheel motor
		Motor_Right=Balance_Pwm+Velocity_Pwm-Turn_Pwm;      //�������ֵ������PWM Calculate the final PWM of the right wheel motor
																												//PWMֵ����ʹС��ǰ��������ʹС������  Positive PWM values cause the car to move forward, negative values cause the car to move backward
																												
		//�˵����� Filter out dead zones
		Motor_Left = PWM_Ignore(Motor_Left);
		Motor_Right = PWM_Ignore(Motor_Right);
		
		//PWM�޷� PWM limiting
		Motor_Left=PWM_Limit(Motor_Left,2600,-2600); //25khz->2592 
		Motor_Right=PWM_Limit(Motor_Right,2600,-2600);		

		
		//ֻ������ģʽ�¼��С������ȥ�ͷ���(��̬���) Only in normal mode can the detection of the taking and lowering of the car be carried out (posture detection)
		if(mode == Normal)
		{
			if(Pick_Up(Acceleration_Z,Angle_Balance,Encoder_Left,Encoder_Right))//����Ƿ�С�������� Check if the car has been picked up
				Stop_Flag=1;	                           					//���������͹رյ�� If picked up, turn off the motor
			if(Put_Down(Angle_Balance,Encoder_Left,Encoder_Right))//����Ƿ�С�������� Check if the car has been lowered
				Stop_Flag=0;	                           					//��������¾�������� If it is put down, start the motor
		}
		
		if(Turn_Off(Angle_Balance,battery)==0)     					//����������쳣 		If there are no abnormalities
			Set_Pwm(Motor_Left,Motor_Right);         					//��ֵ��PWM�Ĵ��� 	Assign to PWM register
   }
	
}


/**************************************************************************
Function: Get angle
Input   : way��The algorithm of getting angle 1��DMP  2��kalman  3��Complementary filtering
Output  : none
�������ܣ���ȡ�Ƕ�	
��ڲ�����way����ȡ�Ƕȵ��㷨 1��DMP  2�������� 3�������˲�
����  ֵ����
**************************************************************************/	
void Get_Angle(u8 way)
{ 
	float gyro_x,gyro_y,accel_x,accel_y,accel_z;
	float Accel_Y,Accel_Z,Accel_X,Accel_Angle_x,Accel_Angle_y,Gyro_X,Gyro_Z,Gyro_Y;
	Temperature=Read_Temperature();      //��ȡMPU6050�����¶ȴ��������ݣ����Ʊ�ʾ�����¶ȡ� //Read the data from the MPU6050 built-in temperature sensor, which approximately represents the motherboard temperature.
	if(way==1)                           //DMP�Ķ�ȡ�����ݲɼ��ж϶�ȡ���ϸ���ѭʱ��Ҫ��  //The reading of DMP is interrupted during data collection, strictly following the timing requirements
	{	
		Read_DMP();                      	 //��ȡ���ٶȡ����ٶȡ����  //Read acceleration, angular velocity, and tilt angle
		Angle_Balance=Pitch;             	 //����ƽ�����,ǰ��Ϊ��������Ϊ�� //Update the balance tilt angle, with positive forward tilt and negative backward tilt
		Gyro_Balance=gyro[0];              //����ƽ����ٶ�,ǰ��Ϊ��������Ϊ��  //Update the balance angular velocity, with positive forward tilt and negative backward tilt
		Gyro_Turn=gyro[2];                 //����ת����ٶ� //Update steering angular velocity
		Acceleration_Z=accel[2];           //����Z����ٶȼ� //Update Z-axis accelerometer
	}			
	else
	{
		Gyro_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_XOUT_L);    //��ȡX�������� //Read X-axis gyroscope
		Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //��ȡY�������� //Read Y-axis gyroscope
		Gyro_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_L);    //��ȡZ�������� //Read Z-axis gyroscope
		Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //��ȡX����ٶȼ� //Read X-axis accelerometer
		Accel_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_YOUT_L); //��ȡX����ٶȼ� //Read Y-axis accelerometer
		Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //��ȡZ����ٶȼ� //Read Z-axis accelerometer
		if(Gyro_X>32768)  Gyro_X-=65536;                 //��������ת��  Ҳ��ͨ��shortǿ������ת�� Data type conversion can also be enforced through short type conversion
		if(Gyro_Y>32768)  Gyro_Y-=65536;                 //��������ת��  Ҳ��ͨ��shortǿ������ת�� Data type conversion can also be enforced through short type conversion
		if(Gyro_Z>32768)  Gyro_Z-=65536;                 //��������ת�� Data type conversion
		if(Accel_X>32768) Accel_X-=65536;                //��������ת�� Data type conversion
		if(Accel_Y>32768) Accel_Y-=65536;                //��������ת�� Data type conversion
		if(Accel_Z>32768) Accel_Z-=65536;                //��������ת�� Data type conversion
		Gyro_Balance=-Gyro_X;                            //����ƽ����ٶ� Update balance angular velocity
		accel_x=Accel_X/1671.84;
		accel_y=Accel_Y/1671.84;
		accel_z=Accel_Z/1671.84;
		gyro_x=Gyro_X/939.8;                              //����������ת�� Gyroscope range conversion
		gyro_y=Gyro_Y/939.8;                              //����������ת�� Gyroscope range conversion
		if(GET_Angle_Way==2)		  	
		{
			 Pitch= KF_X(accel_y,accel_z,-gyro_x)/PI*180;//�������˲� Kalman filtering 
			 Roll = KF_Y(accel_x,accel_z,gyro_y)/PI*180;
		}
		else if(GET_Angle_Way==3) 
		{  
				Accel_Angle_x = atan2(Accel_Y,Accel_Z)*180/PI; //��Accel_Y��accel_y�Ĳ����ó��ĽǶ���һ���ģ�ֻ�Ǳ߳���ͬ The angle obtained using Accel_Y and its parameters is the same, only the side length is different
				Accel_Angle_y = atan2(Accel_X,Accel_Z)*180/PI;
			
			 Pitch = -Complementary_Filter_x(Accel_Angle_x,Gyro_X/16.4);//�����˲� Complementary filtering
			 Roll = -Complementary_Filter_y(Accel_Angle_y,Gyro_Y/16.4);
		}
		Angle_Balance=Pitch;                              //����ƽ�����    Update the balance tilt angle
		Gyro_Turn=Gyro_Z;                                 //����ת����ٶ�  Update steering angular velocity
		Acceleration_Z=Accel_Z;                           //����Z����ٶȼ� Update Z-axis accelerometer
	}

}


/**************************************************************************
Function: Check whether the car is picked up
Input   : Acceleration��Z-axis acceleration��Angle��The angle of balance��encoder_left��Left encoder count��encoder_right��Right encoder count
Output  : 1��picked up  0��No action
�������ܣ����С���Ƿ�����
��ڲ�����Acceleration��z����ٶȣ�Angle��ƽ��ĽǶȣ�encoder_left���������������encoder_right���ұ���������
����  ֵ��1:С��������  0��С��δ������
**************************************************************************/
int Pick_Up(float Acceleration,float Angle,int encoder_left,int encoder_right)
{ 		   
	 static u16 flag,count0,count1,count2;
	 if(flag==0)                                                      //��һ��  Step 1
	 {
			if(myabs(encoder_left)+myabs(encoder_right)<50)               //����1��С���ӽ���ֹ Condition 1: The car is approaching a standstill
			count0++;
			else 
			count0=0;		
			if(count0>10)				
			flag=1,count0=0; 
	 } 
	 if(flag==1)                                                      //����ڶ��� Go to step 2
	 {
			if(++count1>200)       count1=0,flag=0;                       //��ʱ���ٵȴ�2000ms�����ص�һ�� No more waiting for 2000ms after timeout, return to the first step
			if(Acceleration>22000&&(Angle>(-20+Mid_Angle))&&(Angle<(20+Mid_Angle)))   //����2��С������0�ȸ��������� Condition 2, the car is picked up near 0 degrees
			flag=2; 
	 } 
	 if(flag==2)                                                       //������ Step 3
	 {
		  if(++count2>100)       count2=0,flag=0;                        //��ʱ���ٵȴ�1000ms Timeout no longer waits 1000ms
	    if(myabs(encoder_left+encoder_right)>50)                       //����3��С������̥��Ϊ�������ﵽ����ת��    Condition 3: The tires of the car reach their maximum speed due to positive feedback
      {
				flag=0;                                                                                     
				return 1;                                                    //��⵽С�������� Detected the car being picked up
			}
	 }
	return 0;
}
/**************************************************************************
Function: Check whether the car is lowered
Input   : The angle of balance��Left encoder count��Right encoder count
Output  : 1��put down  0��No action
�������ܣ����С���Ƿ񱻷���
��ڲ�����ƽ��Ƕȣ���������������ұ���������
����  ֵ��1��С������   0��С��δ����
**************************************************************************/
int Put_Down(float Angle,int encoder_left,int encoder_right)
{ 		   
	 static u16 flag;//,count;	 
	 if(Stop_Flag==0)                     //��ֹ���    Prevent false positives   
			return 0;	                 
	 if(flag==0)                                               
	 {
			if(Angle>(-10+Mid_Angle)&&Angle<(10+Mid_Angle)&&encoder_left==0&&encoder_right==0) //����1��С������0�ȸ����� Condition 1, the car is around 0 degrees
			flag=1; 
	 } 
	 if(flag==1)                                               
	 {
//		  if(++count>50)                     //��ʱ���ٵȴ� 500ms  Timeout no longer waits 500ms
//		  {
//				count=0;flag=0;
//		  }
		 //���������� Increase sensitivity
	    if((encoder_left>3&&encoder_left<40)||(encoder_right>3&&encoder_right<40)) //����2��С������̥��δ�ϵ��ʱ����Ϊת��  Condition 2: The tires of the car are manually rotated when not powered on
      {
				flag=0;
				return 1;                         //��⵽С�������� Detected that the car has been lowered
			}
	 }
	return 0;
}



