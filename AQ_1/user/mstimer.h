/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : timer.h
* Author             : Deng Jian
* Version            : V1.0.0
* Date               : 01/16/2009
* Description        : use TIM3 to provide 1ms time tick.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MSTIMER_H__
#define __MSTIMER_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_tim.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : TimerSet
* Description    : set a timer
* Input          : t	U16 variable to store the expire time 
		 : val  ticks to the expire time, must less than 0x7FFF
* Output         : None
* Return         : None
*******************************************************************************/
#define MSTimerSet(t, val)		t = (u16)(TIM3->CNT) + (u16)((u16)val << 1)

/*******************************************************************************
* Function Name  : TimerCheck
* Description    : check if the timer is expired
* Input          : t	U16 variable that stores the expire time 
* Output         : 0	if the timer is not expired
                 : 1    if the timer is expired
* Return         : None
*******************************************************************************/
#define MSTimerCheck(t)		((u16)((u16)(TIM3->CNT) - (u16)t) < ((u16)(~((u16)0)) >> 1))

/* Exported functions ------------------------------------------------------- */

/*******************************************************************************
* Function Name  : TimerInit
* Description    : Init TIM3 to generate 1ms time tick.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void MSTimerInit(void);

#endif	//#ifndef __MSTIMER_H__

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
