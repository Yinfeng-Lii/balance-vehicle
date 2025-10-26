#ifndef __APP_LIDAR_H_
#define __APP_LIDAR_H_


#include "ALLHeader.h"

//���ͺ� stm32���ڴ� 512k  This model of stm32 has 512k memory
#define TempLen_Max				256

#define SILEN					  	360   //һ��Э���SI����  SI length of a packet protocol
#define TminiLEN_MAX  		360  	//��Ч���ݴ���  Effective data storage
#define TminiCYCLE 	  	  360   

#define Lidar_HeaderLSB 0xAA
#define Lidar_HeaderMSB 0x55

//�״���Ч����  Radar valid data
extern uint32_t Tminidis[TminiCYCLE];
extern uint8_t lidar_new_pack;

//�״��������Ϣ�ṹ��  The final information structure of the radar
typedef struct _TminiPlus_radar
{
	int 		dis; //���� distance
	double  angle;//�Ƕ�  angle
}TminiPlus_Radar_t;


//�״�SI������Ч���ݽṹ��  Radar SI sampling valid data structure
typedef struct _DataSI_
{
	uint16_t 		Intensity; //��ǿ Э����8λ,��Ϊ����Ҫ���ֱ�Ӳ���16λ  The light intensity protocol is 8 bits, but because it needs to be XORed later, it is directly filled to 16 bits
	uint16_t  	SI_dis;//û����ǰ�ľ���  Distance before solution
}DataSI_t;


typedef struct TminiPlus_Data{
	uint16_t PH;		 		//��ͷ  Baotou
	uint8_t	 CT;		 		//CT��Ϣ  CT Information
	uint8_t	 LSN; 	 		//SI�ĳ���  SI length
	uint16_t FSA; 	 		//��ʼ��  Starting angle
	uint16_t LSA; 	 		//������  End Angle
	uint16_t CS;  	 		//У����  Check code
	DataSI_t SI[SILEN]; //������Ч����  Data valid array
}TminiPlus_MsgData_t;


void Start_Lidar(void);
void Stop_Lidar(void);

void recv_lidar_data(u8 rxtemp);
void Deal_lidardata(void);


//���º���������Ҫ�������ļ�ʹ��  Try not to use the following functions in other files
void Deal_Radar(void);
void Get_Tmini_Data(TminiPlus_MsgData_t *msg);
uint8_t Tmini_checkout(TminiPlus_MsgData_t *msg);
int Get_Tminidis(uint16_t dis_temp);
double Get_Mid_Tminiangle(double abs_angle,uint8_t index,uint8_t data_len,double s_angle);
double Get_Start_Stop_Tminiangle(uint16_t S_angle);
double measure_if_360(double angle);
void Store_360angle_data(uint8_t mylength);





#endif

