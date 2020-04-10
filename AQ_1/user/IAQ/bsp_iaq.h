#ifndef __IAQ_H
#define __IAQ_H

#include "stm32f10x.h"
#include "bsp_i2c_gpio.h"

#define  IAQ_CORE_ADDR          0x5A
#define  IAQ_CORE_READ_ADDR     0xB5

#define ACK 0
#define NACK 1

typedef struct
{
    uint16_t co2;
    uint8_t  sta;
    uint32_t res;
    uint16_t tvoc;
}st_iaq_core;

void IAQ_CORE_Init(void);
void IAQ_CORE_Read(st_iaq_core *iaq);





#endif
