#include "app_ele_tracking.h"


#define Track_ELESpeed 35
void Set_eletrack_speed(void)
{
	if(ele_seat==0)//���ڴų��� Not within the magnetic field
	{
		Move_X = 0;//ͣ�� stop
		return;
	}
	
	Move_X = Track_ELESpeed;       
}



#define ELE_Trun_KP (25) //25  0-25-����d��ֵ  0-25-value without adding d
#define ELE_Trun_KD (0.1) //0.1 0-2
#define ELE_Minddle  135 //��ֵ��Ҫ�����Լ��������� This value needs to be adjusted according to one's own track
//130 150:����2������  Can run in 2 directions

//λ��ʽpid  Positional pid
int Turn_ELE_PD(float gyro)
{
	if(ele_seat==0)//���ڴų���  Not in magnetic field
	{
		return 0;
	}
	
	int ELETurn = 0;
	float err = 0;	  
	
	err=ele_seat-ELE_Minddle;
	
	ELETurn=err*ELE_Trun_KP+gyro*ELE_Trun_KD;
	
	return ELETurn;

}

