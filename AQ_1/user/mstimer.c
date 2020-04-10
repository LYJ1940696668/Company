/**
  ******************************************************************************
  * @file CLY/mstimer.c 
  * @author  xingbeibei
  * @version V1.0.1
  * @date    March-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * 
	*ʹ��timer3
  *
  ******************************************************************************
  */



#include "stm32f10x.h"
#include "mstimer.h"

void MSTimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;                 //TIM_Perscaler������Ԥ��Ƶϵ��(36000-1)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;                   //TIM_Period�������Զ�װ���ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //�����ڶ�ʱ��ʱ��(CK_INT)Ƶ���������˲���(ETR��TIx)ʹ�õĲ���Ƶ��֮��ķ�Ƶ����,�����˲���(ETR,TIx)��Ϊ�˽�ETR�����ķ�Ƶ����ź��˲�����֤ͨ���ź�Ƶ�ʲ�����ĳ���޶�
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; 
	TIM_DeInit(TIM3);				
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);              //��ʼ����ʱ��3
	TIM_Cmd(TIM3,ENABLE);
}


