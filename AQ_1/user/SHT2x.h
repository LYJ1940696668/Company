#ifndef __SHT2x_H__
#define __SHT2x_H__

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
extern uint8_t g_cBoot_StrapHour;
extern uint8_t g_cBoot_StrapMin;
extern float g_fBoot_StrapTem; 
#define  SHT21ADDR  0x80        //SHT21 I2C的地址

#define I2C_GPIO_CLK			RCC_APB2Periph_GPIOB
#define GPIO_I2C                GPIOB

//extern unsigned long  temperature;
//unsigned long  humidity;

#define I2C_GPIO_SCL           GPIO_Pin_6
#define I2C_GPIO_SDA           GPIO_Pin_7

#define I2C_SCL_H        GPIO_SetBits(GPIO_I2C, I2C_GPIO_SCL)
#define I2C_SCL_L        GPIO_ResetBits(GPIO_I2C, I2C_GPIO_SCL)

#define I2C_SDA_H 	     GPIO_SetBits(GPIO_I2C, I2C_GPIO_SDA)
#define I2C_SDA_L        GPIO_ResetBits(GPIO_I2C, I2C_GPIO_SDA)

#define		ACK 	0
#define		NACK 	1

void SHT_I2C_Init_Config(void);      //I2C初始化
void SHT_I2C_SDA_OUT(void);			 //相对于主机来说是输出
void SHT_I2C_SDA_IN(void);			 //相对于主机来说是输入
void SHT_I2C_Start(void);			 //开始信号，启动函数
void SHT_I2C_Stop(void);			 //停止信号，停止函数
void SHT_Make_Ack(void);			 //应答函数			
void SHT_Make_Nack(void);			 //非应答函数			
unsigned char SHT_WAIT_ACK(void);	 //等待应答函数				
u8 SHT_I2C_WriteByte(u8 txd);		 //主机发送一个8位字节函数
u8 SHT_I2C_ReadByte(void);			 //主机读取一个8位字节函数
void SHT21_Init(void);		     //启动SHT2x函数
float SHT21_Work_T(void);		     //测量温度函数
float SHT21_Work_H(void);			 //测量湿度函数
float Read_H(void);
float Read_T(void);
#ifdef __cplusplus
}
#endif

#endif
