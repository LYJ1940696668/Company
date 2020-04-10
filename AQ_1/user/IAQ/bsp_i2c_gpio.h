#ifndef __I2C_GPIO_H
#define	__I2C_GPIO_H

#include "stm32f10x.h"
//#include "bsp_systick.h"
#include "sys.h"
#include "delay.h"
#define SDA_IN()  {GPIOG->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<20;}
#define SDA_OUT() {GPIOG->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<20;}
#define SCL_IN()  {GPIOG->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<16;}
#define SCL_OUT() {GPIOG->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<16;}

#define GPIO_PORT_I2C	GPIOG
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOG
#define I2C_SCL_PIN		GPIO_Pin_4
#define I2C_SDA_PIN		GPIO_Pin_5	

#define IIC_SCL    PGout(4)
#define READ_SCL   PGin(4)
#define IIC_SDA    PGout(5) 
#define READ_SDA   PGin(5)



//void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(uint8_t _ucByte);
uint8_t I2C_ReadByte(void);
uint8_t I2C_WaitAck(void);
uint8_t I2c_WaitSCL(void);
uint8_t I2C_Read_Byte_CKLWAIT(uint8_t ack);
void I2C_Ack(void);
void I2C_NAck(void);
void I2C_GPIO_Config(void);
u8 I2C_Read_Byte(unsigned char ack);

#endif
