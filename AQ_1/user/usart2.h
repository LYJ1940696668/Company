

#ifndef    __USART2_H
#define    __USART2_H 



#include "stm32f10x.h"   
#include <stdio.h> //用到printf函数的串口的输出函数  注意勾选MicroLIB  
#include <stdlib.h>
#include "string.h"
//#include <intrins.h>
#include "stm32f10x_usart.h"

#define USARTm                   USART2
#define USARTm_GPIO              GPIOA
#define USARTm_CLK               RCC_APB1Periph_USART2
#define USARTm_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USARTm_RxPin             GPIO_Pin_3
#define USARTm_TxPin             GPIO_Pin_2
#define USARTm_IRQn              USART2_IRQn
#define USARTm_IRQHandler        USART2_IRQHandler

#define UART2_RX_BUF_LEN  1024
void send_command(void) ;
void USART_Configuration(void);
void Send_date(void);
void Delay(vu32 nCount);
void data(void);
void Send_error_date(void);
//void UARTInit(unsigned char ch, u32 baud, unsigned short parity);
//void UARTSend(unsigned char ch, unsigned char *buf, unsigned short size);
//unsigned short UARTReceive(unsigned char ch, unsigned char *buf, unsigned short size);
//unsigned short UARTReceiveLength(unsigned char ch);
//void UARTClear(unsigned char ch);
//void UARTSend(unsigned char ch, unsigned char *buf, unsigned short size);
//void UsartSend(unsigned char* data,unsigned char length);
//unsigned short UARTSendEnd(unsigned char ch);
//void Usart_Config(void);
//void USARTInit(u32 baud, unsigned short parity);
//unsigned short UARTReceive(unsigned char *buf, unsigned short size);
//unsigned short UARTReceiveLength(void);
//void UARTClear(void);
//void UARTSend(unsigned char *buf, unsigned short size);
//void UsartSend(unsigned char* data,unsigned char length);
//unsigned short UARTSendEnd(void);
//void Usart_Config(void);
//void USART1_Configuration(void);
//void DMA_Configuration(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
//void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
#endif