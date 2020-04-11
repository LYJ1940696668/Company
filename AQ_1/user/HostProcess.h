#ifndef __HOST_H
#define __HOST_H

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define DEF_HOST_USART UART5
//0x0805 9f64
/*
	0x0807 E000 - 0x0807 E7FF
	0x0807 E800 - 0x0807 EFFF
	0x0807 F000 - 0x0807 F7FF
	0x0807 F800 - 0x0807 FFFF
*/
#define STARTADDR 		0x0807E000
#define MODULE_FLASH 	0//设备号偏移地址
#define ID_FLASH		(MODULE_FLASH + 0x0800)//ID偏移地址

enum FunctionCode
{
	SET_DEVICE_MODEL = 0x01,//设置设备号
	READ_DEVICE_MODEL,
	SET_DEVICE_ID,//ID number
	READ_DEVICE_ID,
	SET_DEVICE_FUNC,//设置设备功能
	READ_DEVICE_FUNC,
	SET_DEVICE_TIME,//设置时间
	READ_DEVICE_TIME,
	READ_DEVICE_SENSE,//sense data
	SET_DEVICE_CARR_ADDR = 0x10,//Carrier communication address
	READ_DEVICE_CARR_ADDR,
	SET_DEVICE_485_ADDR,//485 address
	READ_DEVICE_485_ADDR,
	SET_DEVICE_485_BAUDRATE,//BaudRate
	READ_DEVICE_485_RAUNRATE,
};

extern int g_nDataProComFlag;
extern uint8_t g_cDev_ArTi[8];
void Recv_Data(uint8_t ch);
void Host_Code(void);
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum);
void WirteFlashData(uint32_t WriteAddress,uint8_t data[],int num);
#endif
