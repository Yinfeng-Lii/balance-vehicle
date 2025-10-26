#include "pid_control.h"


float Balance_K = 2.0; //2.0
float Velocity_K = 1.35;//1.35
float Turn_K = 1.0; //1.0



//�����pidΪ�˺õ�,���ŵ�100����
//The PID below has been increased to 100 times for easy tuning

//ֱ����PD���Ʋ���
//Vertical loop PD control parameters
float Balance_Kp =9600;//��Χ0-288  Range 0-288
float Balance_Kd =75; //��Χ0-2  Range 0-2

//�ٶȻ�PI���Ʋ���
//PI control parameters for speed loop
float Velocity_Kp=6000; //��Χ0-72 6000  Range 0-72 6000
float Velocity_Ki=30;  //kp/200  30

//ת��PD���Ʋ���
//Steering ring PD control parameters
float Turn_Kp=1400; //��������� �����������ֻ��ƽ����Բ���,����ת�ٶ��й� This can be adjusted according to one's own needs, but the balance can be left unadjusted, depending on the rotation speed
float Turn_Kd=30; //��Χ 0-2  Range 0-2

//ǰ���ٶ�
//Forward speed
float Car_Target_Velocity=0; //0-10
//��ת�ٶ�
//Rotation speed
float Car_Turn_Amplitude_speed=0; //0-60

/**************************************************************************
Function: Absolute value function 
Input   : a��Number to be converted
Output  : unsigned int
�������ܣ�����ֵ����
��ڲ�����a����Ҫ�������ֵ����
����  ֵ���޷�������
**************************************************************************/	
int myabs(int a)
{ 		   
	int temp;
	if(a<0)  temp=-a;  
	else temp=a; 
	return temp;
}


/**************************************************************************
Function: Vertical PD control
Input   : Angle:angle��Gyro��angular velocity
Output  : balance��Vertical control PWM
�������ܣ�ֱ��PD����		
��ڲ�����Angle:�Ƕȣ�Gyro�����ٶ�
����  ֵ��balance��ֱ������PWM
**************************************************************************/	
int Balance_PD(float Angle,float Gyro)
{  
   float Angle_bias,Gyro_bias;
	 int balance;
	 Angle_bias=Mid_Angle-Angle;                       				//���ƽ��ĽǶ���ֵ �ͻ�е��� Find the median angle and mechanical correlation for equilibrium
	 Gyro_bias=0-Gyro; 
	 balance=-Balance_Kp/100*Angle_bias-Gyro_bias*Balance_Kd/100; //����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ��  Calculate the motor PWM PD control for balance control kp is the P coefficient kd is the D coefficient
	
	if(mode == Weight_M)
		balance = balance*Balance_K;//���� Load bearing
	
	 return balance;
}


/**************************************************************************
Function: Speed PI control
Input   : encoder_left��Left wheel encoder reading��encoder_right��Right wheel encoder reading
Output  : Speed control PWM
�������ܣ��ٶȿ���PWM		
��ڲ�����encoder_left�����ֱ�����������encoder_right�����ֱ���������
����  ֵ���ٶȿ���PWM
**************************************************************************/
//�޸�ǰ�������ٶȣ����޸�Target_Velocity�����磬�ĳ�60
// To change the forward and backward speed, please modify Target_Velocity, for example, change it to 60
int Velocity_PI(int encoder_left,int encoder_right)
{  
    static float velocity,Encoder_Least,Encoder_bias,Movement;
	  static float Encoder_Integral;
	  //================ң��ǰ�����˲��� Remote control forward and backward part====================// 
									       	
		if(g_newcarstate==enRUN || g_newcarstate==enps2Fleft || g_newcarstate==enps2Fright)    	Movement=Car_Target_Velocity;	  //ң��ǰ���ź� Remote control forward signal
		else if(g_newcarstate==enBACK || g_newcarstate==enps2Bleft || g_newcarstate==enps2Bright)	Movement=-Car_Target_Velocity;  //ң�غ����ź� Remote control reverse signal
	
		else if(g_newcarstate==enAvoid)  Movement=-10; //����������ź� ʹ�ù̶��ٶ����� Ultrasonic evasion signal operates at a fixed speed
		else if(g_newcarstate==enFollow)  Movement=10; //�����������ź� Ultrasonic wave follows the signal
	   
		else  
				Movement=Move_X;
	
		
   //================�ٶ�PI������ Speed PI controller=====================//	
		Encoder_Least =0-(encoder_left+encoder_right);                    //��ȡ�����ٶ�ƫ��=Ŀ���ٶȣ��˴�Ϊ�㣩-�����ٶȣ����ұ�����֮�ͣ� //Obtain the latest speed deviation=target speed (here zero) - measured speed (sum of left and right encoders) 
		Encoder_bias *= 0.84;		                                          //һ�׵�ͨ�˲���      //First order low-pass filter  
		Encoder_bias += Encoder_Least*0.16;	                              //һ�׵�ͨ�˲����������ٶȱ仯 //First order low-pass filter to slow down speed changes
		Encoder_Integral +=Encoder_bias;                                  //���ֳ�λ�� ����ʱ�䣺5ms //Integral offset time: 5ms
		Encoder_Integral=Encoder_Integral+Movement;                       //����ң�������ݣ�����ǰ������ //Receive remote control data and control forward and backward movement
		if(Encoder_Integral>8000)  	Encoder_Integral=8000;             //�����޷� //Integral limit
		if(Encoder_Integral<-8000)	  Encoder_Integral=-8000;            //�����޷�	 //Integral limit
		velocity=-Encoder_bias*Velocity_Kp/100-Encoder_Integral*Velocity_Ki/100;     //�ٶȿ���	//Speed control
		
		if(Turn_Off(Angle_Balance,battery)==1) Encoder_Integral=0;//����رպ�������� //Clear points after motor shutdown

		if(mode == Weight_M)
			velocity = velocity*Velocity_K;//���� Load bearing
		
	  return velocity;
} 



/**************************************************************************
Function: Turn control
Input   : Z-axis angular velocity
Output  : Turn control PWM
�������ܣ�ת����� 
��ڲ�����Z��������
����  ֵ��ת�����PWM
**************************************************************************/
float myTurn_Kd = 0;
int Turn_PD(float gyro)
{
	 static float Turn_Target,turn_PWM; 
	 float Kp=Turn_Kp,Kd;			//�޸�ת���ٶȣ����޸�Turn_Amplitude���� To modify the steering speed, please modify Turn_Smplitude
	//===================ң��������ת���� Remote control left and right rotation part=================//
	if(g_newcarstate==enLEFT || g_newcarstate==enps2Fleft || g_newcarstate==enps2Bleft)	        Turn_Target=-Car_Turn_Amplitude_speed;
	else if(g_newcarstate==enRIGHT || g_newcarstate==enps2Fright || g_newcarstate==enps2Bright)	  Turn_Target=Car_Turn_Amplitude_speed; 
	
	//���������̶��ٶ��� Left turn, right turn, fixed speed running
	else if(g_newcarstate == enTLEFT) Turn_Target=-50;
	else if(g_newcarstate == enTRIGHT) Turn_Target=50;
	else
	{
		Turn_Target=0; 
	}
	
	//�����ң����ֱ�� If it is remote control walking in a straight line
	if(g_newcarstate==enRUN || g_newcarstate==enBACK )
	{
		Kd=Turn_Kd; 
	} 
	else Kd=myTurn_Kd; 
	

  //===================ת��PD������ Turn to PD controller=================//
	 turn_PWM=Turn_Target*Kp/100+gyro*Kd/100+Move_Z; //���Z�������ǽ���PD����  Combining Z-axis gyroscope for PD control
	
	if(mode == Weight_M)
		turn_PWM = turn_PWM*Turn_K;//���ز��� Load bearing parameters
	
	 return turn_PWM;								 				 //ת��PWM��תΪ������תΪ�� Steering ring PWM: Right turn is positive, left turn is negative
}




