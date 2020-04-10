#ifndef __DELAY_H
#define __DELAY_H 			   
#include <stm32f10x.h>
//Mini STM32?a﹞⊿～?
//那1車?SysTick米???赤“??那y?㏒那????車3迄??DD1邦角赤
//～邦角“delay_us,delay_ms
void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif
