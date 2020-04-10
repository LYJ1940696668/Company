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
#define  SHT21ADDR  0x80        //SHT21 I2C�ĵ�ַ

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

void SHT_I2C_Init_Config(void);      //I2C��ʼ��
void SHT_I2C_SDA_OUT(void);			 //�����������˵�����
void SHT_I2C_SDA_IN(void);			 //�����������˵������
void SHT_I2C_Start(void);			 //��ʼ�źţ���������
void SHT_I2C_Stop(void);			 //ֹͣ�źţ�ֹͣ����
void SHT_Make_Ack(void);			 //Ӧ����			
void SHT_Make_Nack(void);			 //��Ӧ����			
unsigned char SHT_WAIT_ACK(void);	 //�ȴ�Ӧ����				
u8 SHT_I2C_WriteByte(u8 txd);		 //��������һ��8λ�ֽں���
u8 SHT_I2C_ReadByte(void);			 //������ȡһ��8λ�ֽں���
void SHT21_Init(void);		     //����SHT2x����
float SHT21_Work_T(void);		     //�����¶Ⱥ���
float SHT21_Work_H(void);			 //����ʪ�Ⱥ���
float Read_H(void);
float Read_T(void);
#ifdef __cplusplus
}
#endif

#endif
