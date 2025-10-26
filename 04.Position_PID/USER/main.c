/**
* @par Copyright (C): 2018-2028, Shenzhen Yahboom Tech
* @file         // main.c
* @author       // lly
* @version      // V1.0
* @date         // 240628
* @brief        // 程序入口 Program entry
* @details      
* @par History  // 修改历史记录列表，每条修改记录应包括修改日期、修改者及
*               // 修改内容简述  Modification history list, each modification record should include the modification date, modifier and a brief description of the modification content
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



