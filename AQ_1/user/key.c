#include "stm32f10x.h"
#include "key.h"
#include "delay.h"
#include "interface_disp.h"
void EXTIX_Init(void)
{
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

    key_Init();	 //	按键端口初始化
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
	
    /* key 1 exti */
	//GPIOE.0 中断线以及中断初始化配置   下降沿触发KEY1
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;	
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
    /* key 2 exti */
	 //GPIOB.8	  中断线以及中断初始化配置 下降沿触发 //KEY2
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
  	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
    /* key 3 exti */
	  //GPIOG.7	  中断线以及中断初始化配置  下降沿触发	//KEY3
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource7);
  	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
    /* key 4 exti */
	  //GPIOG.8	  中断线以及中断初始化配置  下降沿触发	//KEY4
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
  	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
    /* key 5 exti */
		//GPIOG.15	  中断线以及中断初始化配置  下降沿触发	//KEY5
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource15);
  	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键KEY1所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级0， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键KEY2所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级0， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键KEY3所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级1， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键KEY4所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级1， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键KEY5所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;					//子优先级0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 	
}


void key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;        //KEY4/KEY3/KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                           //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);                                  //初始化GPIOB3,4,8
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                           //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);                                  //初始化GPIOE0
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                           //设置成上拉输入
 	GPIO_Init(GPIOG, &GPIO_InitStructure);                                  //初始化GPIOG15
	
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	GPIO_SetBits(GPIOE,GPIO_Pin_0);
	GPIO_SetBits(GPIOG,GPIO_Pin_15 | GPIO_Pin_7 | GPIO_Pin_8);
	
}