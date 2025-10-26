/**
* @par Copyright (C): 2018-2028, Shenzhen Yahboom Tech
* @file         // main.c
* @author       // lly
* @version      // V1.0
* @date         // 240628
* @brief        // �������
* @details      
* @par History  // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼�
*               // �޸����ݼ���  Modification history list, each modification record should include the modification date, modifier and a brief description of the modification content
*/

#include "AllHeader.h"

u8 key_step = 0;
int16_t Position_L=0,Position_R=0;
int16_t Encoder_Left =0 ,Encoder_Right=0; 
int16_t Target_Velocity = 20,Target_Position = 5000; //���������Ŀ��λ�ú�ת�� Target positions and speeds of two motors


int main(void) 
{	
	bsp_init();
	
	while(1)
	{
		
		printf("vL:%d\t vR:%d\r\n",Encoder_Left,Encoder_Right); //��ӡ��ǰ10ms��ת�� Print the current 10ms rotational speed
		printf("pL:%d\t pR:%d\r\n",Position_L,Position_R);  //��ӡ��ǰ��λ��  Print the current location
		delay_ms(200);
		
		if(Key1_State(1))
		{
			printf("Key put down!\r\n"); //��ӡ�������� Press the print button
			LED = !LED; //ÿ����������LED״̬��ת Whenever the button is pressed, the LED status reverses
			key_step++;
			if(key_step >=4)
			{
				key_step = 0;
			}
		}
		
		
		if(key_step == 1)
		{
			Target_Velocity = 30; //ʹ�õ��ٶ� Speed of use
			Target_Position = 8000; //ת������Ŀ��λ�� Rotate to the target position
		}
		else if(key_step == 3)
		{
			Target_Velocity = 10; //ʹ�õ��ٶ� Speed of use
			Target_Position = 1000; //ת������Ŀ��λ�� Rotate to the target position 
		}
		else 
		{
			Target_Velocity = 20; //ʹ�õ��ٶ� Speed of use
			Target_Position = 5000; //ת������Ŀ��λ�� Rotate to the target position
		}
		

	}
}



