#include "bsp_i2c_gpio.h"
//IIC��ʼ
void I2C_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(3);
 	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=0;

}
//IIC����
void I2C_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;
 	delay_us(3);
	IIC_SCL=1; 
	IIC_SDA=1;
	delay_us(3);	
}
//IIC����һ��8λ�ֽڵ�����
void I2C_SendByte(uint8_t txd)
{
	u8 t;   
	SDA_OUT(); 
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {
		if(txd&0x80)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1;	  
		delay_us(2); 
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		
		delay_us(2);
    }	 
	
}
/*
��ȡһ���ֽ�
ack��
	0 ����ack 
	1����nack
*/
u8 I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
	IIC_SDA = 1;
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)
		{
			receive |= 0x01;
			delay_us(1);
		}
		else
		{
			receive &= 0xfe;
		}
		delay_us(2); 
    }					 
    if (!ack)
        I2C_NAck();//����nACK
    else
        I2C_Ack(); //����ACK   
    return receive;
}
/*
�ú�������Clock stretching��ʱ����չ������������
master�ڶ�ȡ����ʱ����Ҫ�ȴ�slave�ͷ�SCL
ack��
	0 ����ack 
	1����nack
*/
uint8_t I2C_Read_Byte_CKLWAIT(uint8_t ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
	IIC_SDA = 1;
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
		if(I2c_WaitSCL() == 1)
			return 1;
        if(READ_SDA)
		{
			receive |= 0x01;
			delay_us(1);
		}
		else
		{
			receive &= 0xfe;
		}
		delay_us(2);
    }					 
    if (!ack)
        I2C_NAck();//����nACK
    else
        I2C_Ack(); //����ACK   
    return receive;
}

/*
���ڵȴ�slave Ack
return��0��ʾ��ȷӦ��  1��ʾ��������Ӧ
*/
uint8_t I2C_WaitAck(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;delay_us(3);  
	IIC_SCL=1;delay_us(3);
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>1750)
		{
			IIC_SCL=0;
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  

}

/*
�ú�������Clock stretching��ʱ����չ������������
master�ڶ�ȡ����ʱ����Ҫ�ȴ�slave�ͷ�SCL
return ��
	0:slave �ͷ�SCL
	1:slave û���ͷ�SCL
*/
uint8_t I2c_WaitSCL(void)
{
	uint16_t ucErrTime=1900;
	SCL_IN();
	while(!READ_SCL)
	{
		ucErrTime --;
		if(ucErrTime == 0)
		{
			SCL_OUT();
			return 1;
		}
	}
	SCL_OUT();
	return 0;
}
/*
master���� Ack �ź�
*/
void I2C_Ack(void)
{
	SDA_OUT();
	IIC_SCL=0;	
	IIC_SDA=0;
	delay_us(3);
	IIC_SCL=1;
	delay_us(3);
	IIC_SCL=0;

}
/*
master ���� NAck �ź�
*/
void I2C_NAck(void)
{
	SDA_OUT();
	IIC_SCL=0;
	IIC_SDA=1;
	delay_us(3);
	IIC_SCL=1;
	delay_us(3);
	IIC_SCL=0;
}
/*
IIC ���ų�ʼ��
*/
void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_I2C_PORT, ENABLE);

	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIO_PORT_I2C,I2C_SCL_PIN|I2C_SDA_PIN);
	SDA_OUT();
	IIC_SDA = 1;
	IIC_SCL = 1;
}
