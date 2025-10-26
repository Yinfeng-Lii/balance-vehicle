#include "app_mode.h"

uint8_t angle_max = 40;
Car_Mode mode = Normal;//Normal; //ģʽΪ����


//ģʽѡ�� ����š����������ģʽ�л�
//Mode selection: Use the hand to twist the wheel to switch modes
void Mode_select(void)
{
	int16_t mode_cnt = 0;
	OLED_Draw_Line("1.Standard Mode", 1, true, true); 

	while(!Key1_State(1)) 
	{
		mode_cnt +=Read_Encoder(MOTOR_ID_ML);
		mode_cnt +=-Read_Encoder(MOTOR_ID_MR);
		car_mode(mode_cnt);//ģʽѡ�� Mode selection
		show_mode_oled();//oled��ʾģʽ oled display mode
		
	}
	
	Set_Mid_Angle();//ģʽ���úú�,���û�е��ֵ After setting the mode, set the mechanical median value
	Set_angle();//���õ������ Set the inclination angle for falls
	Set_control_speed();//����ң�ص��ٶ� Set the speed of the remote control


	Set_PID();//ĳЩģʽ����Ҫ��������һ��ƽ��pid Some modes require special settings for balancing PID

}


void car_mode(int16_t cnt)
{
	static int16_t cnt_old ;
	
	if(myabs(myabs(cnt)-myabs(cnt_old))>250)
	{
		if(cnt < cnt_old)
		{
			mode = (Car_Mode)((mode - 1) %Mode_Max); //��С ö��(u8) -1��Ϊ255  Large to small   enumeration (u8) -1 is 255
			if(mode > Mode_Max)
			{
				mode = (Car_Mode)(Mode_Max -1);
			}
		}
		else
		{
			mode = (Car_Mode)((mode + 1) %Mode_Max); //С����  Small to Large
		}
		
		cnt_old = cnt; //��ֵ  Assignment
//		printf("%d\r\n",mode);
	}
	
}


//����ģʽ���û�е��ֵ Set the mechanical median according to the mode
void Set_Mid_Angle(void)
{
	switch ((uint8_t)mode)
	{
		case Normal:   	
		case U_Follow:
		case U_Avoid:    		 	
		case Weight_M:  		 	
		case Line_Track: 
		case Diff_Line_track:
		case PS2_Control:   	Mid_Angle = 0;  break;
		
		case CCD_Mode:   			Mid_Angle = 1;  break;
		case ElE_Mode:   			Mid_Angle = -4; break;
		
		case K210_QR:   			
		case K210_Line:  	 		
		case K210_Follow:
		case K210_SelfLearn:
		case K210_mnist:			Mid_Angle = -1; break;
		
		case LiDar_avoid:   	
		case LiDar_Follow:    
		case LiDar_aralm:   	
		case LiDar_Patrol:  	
		case LiDar_Line: 			
		case LiDar_wall_Line: 		Mid_Angle = 0;  break;
	}

}

//ң�ص��ٶȳ�ʼ��
//Speed initialization of remote control
void Set_control_speed()
{
	if(mode == Normal || mode == Weight_M) //�����͸���ģʽ����Ҫ��ʼ����ֵ  Both normal and load modes need to initialize this value
	{
		Car_Target_Velocity=30;
		Car_Turn_Amplitude_speed=36;
	}
	else if(mode == PS2_Control)
	{
		Car_Target_Velocity = 30;
		Car_Turn_Amplitude_speed = 48;
	}

}

//���õ��������
//Set the inclination angle for falls
void Set_angle(void)
{
	if((mode == Line_Track)||(mode == ElE_Mode)||(mode == Diff_Line_track))
	{
		angle_max = 16;
	}
	else if(mode == CCD_Mode)
	{
		angle_max = 25;
	}
	else if((mode == K210_QR)||(mode == K210_Line)||(mode == K210_Follow)||(mode == K210_SelfLearn)||(mode == K210_mnist))
	{
		angle_max = 30;
	}
	else
	{
		angle_max = 40;
	}

}


extern float Balance_Kp,Balance_Kd,Velocity_Kp,Velocity_Ki,Turn_Kp,Turn_Kd; //������ֱ�����ٶȻ���ת�� //Introduce vertical rings, speed rings, and steering rings
void Set_PID(void)
{
	if(mode == Weight_M || mode == K210_Follow) //���� Load bearing
	{
		
		Balance_Kp =9600;
		Balance_Kd =75 ; 
		Velocity_Kp=7000; 
	  Velocity_Ki=35;  
	  Turn_Kp=1400; 
		Turn_Kd=20;

	}
//	else if(mode ==Diff_Line_track) //���Ѷ�Ѳ�� High-difficulty line patrol
//	{
//		Balance_Kp =8400;
//		Balance_Kd =42; 
//		Velocity_Kp=6500;
//		Velocity_Ki=32.5; 
//		Turn_Kp=2500; 
//		Turn_Kd=20; 
//	}
	else if(mode == ElE_Mode)//��� electromagnetism
	{
		Balance_Kp =9900;
		Balance_Kd =72 ;

		Velocity_Kp=7000; 
		Velocity_Ki=35; 


		Turn_Kp=2500; 
		Turn_Kd=20;
	}
	else if(mode == K210_Line)
	{
		Balance_Kp =12000;
		Balance_Kd =72 ;

		Velocity_Kp=8000; 
		Velocity_Ki=40;  

		Turn_Kp=2500; 
		Turn_Kd=20;

	}

	else if(mode == LiDar_Line || mode == LiDar_wall_Line)
	{
		Balance_Kp =10200;
		Balance_Kd =75 ; 

		Velocity_Kp =9000; 
		Velocity_Ki =45;  

		Turn_Kp=2500; 
		Turn_Kd=20;

	}
	else if(mode == Normal || mode == PS2_Control || mode == LiDar_Patrol)
	{
		Balance_Kp =9600;
		Balance_Kd =48 ; 

		Velocity_Kp =6200; 
		Velocity_Ki =31;  

		Turn_Kp =1700; 
		Turn_Kd =20;
	}

}

