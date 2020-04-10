#ifndef _HCHO_h
#define _HCHO_h

#include "stm32f10x.h"

#define     HCHO_MODE_MASTER    0X01
#define     HCHO_MODE_SLAVE     0X02

typedef struct
{
    uint16_t ug_val;
    uint16_t ppb_val;
}st_hcho;

typedef struct
{
    uint8_t read_mode;
    uint8_t read_flag;
    void (*init)(void);
    void(*set_read_mode)(uint8_t);
    uint8_t(*read_data)(uint8_t);
    st_hcho value;
}st_dev_hcho;




static void hw_hcho_init(void);
static void hcho_set_mode(uint8_t mode);
static uint8_t hcho_read_data(uint8_t mode);

void hcho_config(void);

extern st_dev_hcho hcho;;


#endif