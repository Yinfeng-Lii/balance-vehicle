/**
* @par Copyright (C): 2018-2028, Shenzhen Yahboom Tech
* @file         // main.c
* @author       // lly
* @version      // V1.0
* @date         // 240628
* @brief        // ������� Program entry
* @details      
* @par History  // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼�
*               // �޸����ݼ���  Modification history list, each modification record should include the modification date, modifier and a brief description of the modification content
*/

#include "AllHeader.h"

u8 key_step = 0;
int16_t Position_Left = 10000,Position_Right = 10000; 
int16_t Left_target = -5000,Right_target = -5000; 



int main(void) 
{	
	bsp_init();
	
	while(1)
	{
		
		printf("L:%d\t R:%d\r\n",Position_Left,Position_Right);
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
			Left_target = -5500;
			Right_target = -5500;
		}
		else if(key_step == 3)
		{
			Left_target = -4500;
			Right_target = -4500;
		}
		else 
		{
			Left_target = -5000;
			Right_target = -5000;
		}
		

	}
}



