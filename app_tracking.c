#include "app_tracking.h"


//Ѳ��̽ͷ�Ĵ���  Processing of line inspection probe
static void track_deal_four(u8 *s1,u8 *s2,u8 *s3,u8 *s4)
{
	*s1 = IN_X1;
	*s2 = IN_X2;
	*s3 = IN_X3;
	*s4 = IN_X4;
}



//----------------PID-------------\\----//

#define Track_IRSpeed 15

//��ͨѲ��PID  Ordinary line patrol PID
#define IRTrack_Trun_KP (450) //400 500 //100-1000
#define IRTrack_Trun_KI (0.01) //0.1 0
#define IRTrack_Trun_KD (0.15) //0.2 0-2


//���Ѷ�Ѳ��PID  High-difficulty line patrol PID
#define DIFFTrack_Trun_KP (350) //250  //100-1000
#define DIFFTrack_Trun_KI (0.0001) //0
#define DIFFTrack_Trun_KD (0.15)  //0.13  0-2



#define IRTrack_Minddle  0 

int8_t error = 0;

void Set_IRtrack_speed(void)
{
	if(mode == Line_Track)
	{
		Move_X = Track_IRSpeed; 
	}
	else 
	{
		Move_X = 15; //���Ѷ�Ѳ�ߵ��ٶ�  High-difficulty line patrol speed
	}
	      
}



void PID_track_get(void)
{
	u8 x1,x2,x3,x4;
	track_deal_four(&x1,&x2,&x3,&x4);
	
	if(x2 == 1 && x1 == 0  && x3 == 1&& x4 == 1)
	{
		error = -1;
	}
	else if(x2 == 0 && x1 == 0  && x3 == 1&& x4 == 1)
	{
		error = -2;
	}
	else if(x2 == 0 && x1 == 1  && x3 == 1&& x4 == 1)
	{
		error = -3;
	}
	else if(x2 == 0 && x1 == 0  && x3 == 0&& x4 == 1)//ֱ�� Right Angle
	{
		error = -4;
	}
	
	else if(x2 == 1 && x1 == 1  && x3 == 0 && x4 == 1)
	{
		error = 1;
	}
	
	else if(x2 == 1 && x1 == 1  && x3 == 0&& x4 == 0)
	{
		error = 2;
	}
	
	else if(x2 == 1 && x1 == 1  && x3 == 1&& x4 == 0)
	{
		error = 3;
	}
	else if(x2 == 1 && x1 == 0  && x3 == 0&& x4 == 0)//ֱ�� Right Angle
	{
		error = 4;
	}

	else if ( x1 == 0  && x3 == 0) //ֱ��  Go straight
	{
		error = 0;
	}
	
	
}


//λ��ʽpid  Positional pid
int Turn_IRTrack_PD(float gyro)
{

	int IRTrackTurn = 0;
	float err = 0;	
	static float IRTrack_Integral;
	
	PID_track_get();  //��ȡƫ�� Obtain deviation
	
	err=error-IRTrack_Minddle;
	
	IRTrack_Integral +=err;
	
	if(mode == Line_Track) //��ͨѲ�� pid  Normal line patrol pid
	{
		  IRTrackTurn=err*IRTrack_Trun_KP+IRTrack_Trun_KI*IRTrack_Integral+gyro*IRTrack_Trun_KD;
	}
	else  //���Ѷ�Ѳ��  High-difficulty line patrol
	{
			IRTrackTurn=err*DIFFTrack_Trun_KP+DIFFTrack_Trun_KI*IRTrack_Integral+gyro*DIFFTrack_Trun_KD;
	}
			
	return IRTrackTurn;

}





