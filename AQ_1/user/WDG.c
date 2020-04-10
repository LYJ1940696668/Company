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
    /* д��0x5555,�����������Ĵ���д�빦�� */  
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);    
	  /* ����ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/  
	  IWDG_SetPrescaler(IWDG_Prescaler_64);    
	  /* ι��ʱ�� 5s/6.4MS=781 .ע�ⲻ�ܴ���0xfff*/  //���Ź���λʱ��=��Ԥ��Ƶ*��װ��ֵ��/40KHZ  2500*64/40000=4S 
	  IWDG_SetReload(4000);      //  ��Ӧ6.4s
//      IWDG_SetReload(2500);      //  ��Ӧ5s
  	/* ι��*/   
	  IWDG_ReloadCounter();    
	  /* ʹ�ܹ���*/ 
    IWDG_Enable();
}
