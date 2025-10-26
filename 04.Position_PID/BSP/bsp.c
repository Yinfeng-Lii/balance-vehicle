#include "bsp.h"

void bsp_init(void)
{
	
	DIY_NVIC_PriorityGroupConfig(2);	  //�����жϷ���   //Set interrupt grouping
	delay_init();	    	            //��ʱ������ʼ��	 //Delay function initialization
	JTAG_Set(JTAG_SWD_DISABLE);     //�ر�JTAG�ӿ�    //Close JTAG interface
	JTAG_Set(SWD_ENABLE);           //��SWD�ӿ� �������������SWD�ӿڵ��� //Opening the SWD interface allows for debugging using the motherboard's SWD interface
	
	Init_Led_GPIO();						//LED //Onboard LED
	uart_init(115200);
	KEYAll_GPIO_Init();
	
	BalanceCar_Motor_Init();     		//���GPIO��ʼ�� //Motor GPIO initialization
	BalanceCar_PWM_Init(2880,0); 
	Encoder_Init_TIM3();
	Encoder_Init_TIM4();
	
	TIM6_Init(); //pid ���ٴ��� Speed measurement processing

}


void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //��������ʱ��	  Activate auxiliary clock  
	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24] Clear MAPR [26:24]
	AFIO->MAPR|=temp;       //����jtagģʽ Set jtag mode
} 


/**************************************************************************
Function: Set NVIC group
Input   : NVIC_Group
Output  : none
�������ܣ������жϷ���
��ڲ�����NVIC_Group:NVIC���� 0~4 �ܹ�5�� 	
����  ֵ����
**************************************************************************/ 
void DIY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ Take the last three
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������  Read previous settings
	temp&=0X0000F8FF; //�����ǰ����   Clear previous groups
	temp|=0X05FA0000; //д��Կ��  Write the key
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	  Set grouping   	  				   
}



