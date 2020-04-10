#ifndef    __RTC_H
#define    __RTC_H 



#include "stm32f10x.h"   
#include <stdio.h> //用到printf函数的串口的输出函数  注意勾选MicroLIB  
#include <stdlib.h>
//#include <intrins.h>
//#include "stm32f10x_usart.h"
//#include "date.h"
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


//void USART_Configuration(void);
//void NVIC_Configuration(void);
//void RTC_Configuration(void);
//void start_rct(void);   
//uint32_t Time_Regulate(void);  
//void Time_Adjust(void);  
//void Time_Show(void);
//void SaveTime(uint32_t TimeVar);  
//void Time_Display(uint32_t TimeVar);  
//uint32_t USART_Scanf(uint32_t value); 
//int fputc(int ch,FILE *f);

//extern uint8_t TimeDisplay;
void Time_Get(void);
void RTC_Configuration(void);
void RTC_NVIC_Configuration(void);
u32 Time_Regulate(void);
void Time_Adjust(void);
void Time_Show(u16 x,u16 y);
void Time_Display(u32 TimeVar,u16 x,u16 y);
#endif 