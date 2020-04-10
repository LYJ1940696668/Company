#include "WDG.h"


void InitIwdg(void)
{
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        /* IWDGRST flag set */
        /* Turn on LED1 */
        //     STM_EVAL_LEDOn(LED1);

        /* Clear reset flags */
        RCC_ClearFlag();
    }	
    /* 写入0x5555,用于允许狗狗寄存器写入功能 */  
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);    
	  /* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/  
	  IWDG_SetPrescaler(IWDG_Prescaler_64);    
	  /* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/  //看门狗复位时间=（预分频*重装载值）/40KHZ  2500*64/40000=4S 
	  IWDG_SetReload(4000);      //  对应6.4s
//      IWDG_SetReload(2500);      //  对应5s
  	/* 喂狗*/   
	  IWDG_ReloadCounter();    
	  /* 使能狗狗*/ 
    IWDG_Enable();
}
