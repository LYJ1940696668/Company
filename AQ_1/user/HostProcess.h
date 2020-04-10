#ifndef __HOST_H
#define __HOST_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define DEF_HOST_USART UART5

enum FunctionCode
{
	SET_DEVICE_MODEL = 0x01,//�����豸��
	READ_DEVICE_MODEL,
	SET_DEVICE_ID,//ID number
	READ_DEVICE_ID,
	SET_DEVICE_FUNC,//�����豸����
	READ_DEVICE_FUNC,
	SET_DEVICE_TIME,//����ʱ��
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
#endif