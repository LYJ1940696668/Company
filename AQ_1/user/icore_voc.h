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

#define  IAQ_CORE_ADDR  0x5A        //ICORE I2C�ĵ�ַ
#define  IAQ_CORE_READ_ADDR  0xB5        //ICORE I2C�ĵ�ַ

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

void VOC_I2C_Init_Config(void);          //I2C��ʼ��
void VOC_I2C_SDA_OUT(void);			    //�����������˵�����
void VOC_I2C_SDA_IN(void);			    //�����������˵������
void VOC_I2C_Start(void);			    //��ʼ�źţ���������
void VOC_I2C_Stop(void);			        //ֹͣ�źţ�ֹͣ����
void VOC_Make_Ack(void);			        //Ӧ����			
void VOC_Make_Nack(void);			    //��Ӧ����			
unsigned char VOC_WAIT_ACK(void);	    //�ȴ�Ӧ����		
u8 VOC_I2C_WAIT_SCL_HIGH(void);
u8 VOC_I2C_WriteByte(u8 txd);		    //��������һ��8λ�ֽں���
u8 VOC_I2C_ReadByte(void);			    //������ȡһ��8λ�ֽں���
u8 VOC_I2C_ReadByte_ClkStretching(void);

void  IAQ_CORE__Init(void);		            //����SHT2x����
float IAQ_CORE__Work_T(void);		        //�����¶Ⱥ���
float IAQ_CORE__Work_H(void);			    //����ʪ�Ⱥ���


extern st_iaq_core tvoc;

#ifdef __cplusplus
}

#endif



#endif