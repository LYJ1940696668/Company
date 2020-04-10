#ifndef __TURN_ASCII_H__
#define __TURN_ASCII_H__

#include "stm32f10x.h"
#include "turn_ASCII.h"

void CO2_ppm_TO_ascii(void);
void pm2_5_TO_ascii(void);
void Temperature_TO_ascii(void);
void Humidity_TO_ascii(void);
void turn_ascii(void);
void read_data(void);
void turn_date (void);
#endif