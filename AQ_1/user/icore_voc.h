#ifndef _ICORE_VOC_H
#define _ICORE_VOC_H



#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
//#include <intrins.h>
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_i2c.h"
#include "delay.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define  IAQ_CORE_ADDR  0x5A        //ICORE I2C的地址
#define  IAQ_CORE_READ_ADDR  0xB5        //ICORE I2C的地址

#define IAQ_CORE_I2C_GPIO_CLK			RCC_APB2Periph_GPIOG
#define IAQ_CORE_GPIO_I2C                GPIOG

//extern unsigned long  temperature;
//unsigned long  humidity;

#define IAQ_CORE_I2C_GPIO_SCL           GPIO_Pin_4
#define IAQ_CORE_I2C_GPIO_SDA           GPIO_Pin_5

#define IAQ_CORE_I2C_SCL_H        GPIO_SetBits(IAQ_CORE_GPIO_I2C, IAQ_CORE_I2C_GPIO_SCL)
#define IAQ_CORE_I2C_SCL_L        GPIO_ResetBits(IAQ_CORE_GPIO_I2C, IAQ_CORE_I2C_GPIO_SCL)

#define IAQ_CORE_I2C_SDA_H 	     GPIO_SetBits(IAQ_CORE_GPIO_I2C, IAQ_CORE_I2C_GPIO_SDA)
#define IAQ_CORE_I2C_SDA_L        GPIO_ResetBits(IAQ_CORE_GPIO_I2C, IAQ_CORE_I2C_GPIO_SDA)

#define		ACK 	0
#define		NACK 	1

#define		FALSE 	0
#define		TRUE 	1


typedef struct
{
    u16 co2;
    u8  sta;
    u32 res;
    u16 tvoc;
}st_iaq_core;

void VOC_I2C_Init_Config(void);          //I2C初始化
void VOC_I2C_SDA_OUT(void);			    //相对于主机来说是输出
void VOC_I2C_SDA_IN(void);			    //相对于主机来说是输入
void VOC_I2C_Start(void);			    //开始信号，启动函数
void VOC_I2C_Stop(void);			        //停止信号，停止函数
void VOC_Make_Ack(void);			        //应答函数			
void VOC_Make_Nack(void);			    //非应答函数			
unsigned char VOC_WAIT_ACK(void);	    //等待应答函数		
u8 VOC_I2C_WAIT_SCL_HIGH(void);
u8 VOC_I2C_WriteByte(u8 txd);		    //主机发送一个8位字节函数
u8 VOC_I2C_ReadByte(void);			    //主机读取一个8位字节函数
u8 VOC_I2C_ReadByte_ClkStretching(void);

void  IAQ_CORE__Init(void);		            //启动SHT2x函数
float IAQ_CORE__Work_T(void);		        //测量温度函数
float IAQ_CORE__Work_H(void);			    //测量湿度函数


extern st_iaq_core tvoc;

#ifdef __cplusplus
}

#endif



#endif