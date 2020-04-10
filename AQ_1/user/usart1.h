#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

typedef struct
{
    uint16_t PM1_0_std;
    uint16_t PM2_5_std;
    uint16_t PM10_std;
    uint16_t PM1_0;
    uint16_t PM2_5;
    uint16_t PM10;
}st_PM;

extern st_PM  PM_value;


//int fputc(int ch, FILE *f);
void USART1_IRQHandler(void);
void send_command_pm(void);
void send_beidongcommand(void) ;
void Read_PM(void);
void PM_Init(void);
#endif /* __USART1_H */
