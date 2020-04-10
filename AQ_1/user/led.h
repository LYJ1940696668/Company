#ifndef __LED_H__
#define __LED_H__
#include "stm32f10x.h"
#include "led.h"

typedef struct _gpio
{
    uint32_t RCC_APB2Periph;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} gpio_t;

#define countof(a)  (sizeof(a)/sizeof((a)[0]))
#define GPIO_PIN(port, pin) {RCC_APB2Periph_##port, port, GPIO_Pin_##pin}


#define 	RUNING_LED			2
#define 	UART2_RECV_LED		4
#define 	UART2_SEND_LED		5

void LED_Init(void);
void LED_Run(void);
void LED_Toggle(void);
void LED_Set_GREEN(void);
void LED_Reset_GREEN(void);
void carrier_receive(void);
void carrier_transmit(void);


void hw_led_init(void);
void hw_led_on(uint32_t n);
void hw_led_off(uint32_t n);


#endif	