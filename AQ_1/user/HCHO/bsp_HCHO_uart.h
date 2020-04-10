#ifndef __UART4_H
#define	__UART4_H


#include "stm32f10x.h"
#include <stdio.h>
/*������ģʽ����
	HCHO_MODE_MASTER	��Ƭ��ѯ�ʣ��������ϴ�
	HCHO_MODE_SLAVE		������������һ���ϴ�һ��
*/
#define 	HCHO_MODE	HCHO_MODE_SLAVE	
#define     HCHO_MODE_MASTER    0X01
#define     HCHO_MODE_SLAVE     0X02

// ����4-UART4
#define  DEBUG_UART4x                   UART4
#define  DEBUG_UART4_CLK                RCC_APB1Periph_UART4
#define  DEBUG_UART4_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_UART4_BAUDRATE           9600

// USART GPIO ���ź궨��
#define  DEBUG_UART4_GPIO_CLK           (RCC_APB2Periph_GPIOC)
#define  DEBUG_UART4_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_UART4_TX_GPIO_PORT         GPIOC   
#define  DEBUG_UART4_TX_GPIO_PIN          GPIO_Pin_10
#define  DEBUG_UART4_RX_GPIO_PORT       GPIOC
#define  DEBUG_UART4_RX_GPIO_PIN        GPIO_Pin_11

#define  DEBUG_UART4_IRQ                UART4_IRQn
#define  DEBUG_UART4_IRQHandler         UART4_IRQHandler


void HCHO_Config(void);
uint16_t HCHO_Process(void);
unsigned char FucCheckSum(unsigned char *i, unsigned char ln);
#endif /* __USART_H */
