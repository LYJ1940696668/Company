#ifndef __USART3_H
#define	__USART3_H

#include "stm32f10x.h"
#include <stdio.h>

//void USART3_Config(void);

int fputc(int ch, FILE *f);
void USART3_IRQHandler(void);

void send_command_CO2(void);
void CO2_Init(void);
void Read_CO2(void);
uint16_t CRC16_3(uint8_t Buff[],  uint8_t len);
#endif /* __USART3_H */
