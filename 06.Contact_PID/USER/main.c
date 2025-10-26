/**
* @par Copyright (C): 2018-2028, Shenzhen Yahboom Tech
* @file         // main.c
* @author       // lly
* @version      // V1.0
* @date         // 240628
* @brief        // 程序入口
* @details      
* @par History  // 修改历史记录列表，每条修改记录应包括修改日期、修改者及
*               // 修改内容简述  Modification history list, each modification record should include the modification date, modifier and a brief description of the modification content
*/

#include "AllHeader.h"

u8 key_step = 0;
int16_t Position_L=0,Position_R=0;
int16_t Encoder_Left =0 ,Encoder_Right=0; 
int16_t Target_Velocity = 20,Target_Position = 5000; //两个电机的目标位置和转速 Target positions and speeds of two motors


int main(void) 
{	
	bsp_init();
	
	while(1)
	{
		
		printf("vL:%d\t vR:%d\r\n",Encoder_Left,Encoder_Right); //打印当前10ms的转速 Print the current 10ms rotational speed
		printf("pL:%d\t pR:%d\r\n",Position_L,Position_R);  //打印当前的位置  Print the current location
		delay_ms(200);
		
		if(Key1_State(1))
		{
			printf("Key put down!\r\n"); //打印按键按下 Press the print button
			LED = !LED; //每当按键按下LED状态反转 Whenever the button is pressed, the LED status reverses
			key_step++;
			if(key_step >=4)
			{
				key_step = 0;
			}
		}
		
		
		if(key_step == 1)
		{
			Target_Velocity = 30; //使用的速度 Speed of use
			Target_Position = 8000; //转动到的目标位置 Rotate to the target position
		}
		else if(key_step == 3)
		{
			Target_Velocity = 10; //使用的速度 Speed of use
			Target_Position = 1000; //转动到的目标位置 Rotate to the target position 
		}
		else 
		{
			Target_Velocity = 20; //使用的速度 Speed of use
			Target_Position = 5000; //转动到的目标位置 Rotate to the target position
		}
		

	}
}



