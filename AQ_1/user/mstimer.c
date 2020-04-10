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
	*使用timer3
  *
  ******************************************************************************
  */



#include "stm32f10x.h"
#include "mstimer.h"

void MSTimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;                 //TIM_Perscaler来设置预分频系数(36000-1)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;                   //TIM_Period来设置自动装入的值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //定义在定时器时钟(CK_INT)频率与数字滤波器(ETR，TIx)使用的采样频率之间的分频比例,数字滤波器(ETR,TIx)是为了将ETR进来的分频后的信号滤波，保证通过信号频率不超过某个限定
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; 
	TIM_DeInit(TIM3);				
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);              //初始化定时器3
	TIM_Cmd(TIM3,ENABLE);
}


