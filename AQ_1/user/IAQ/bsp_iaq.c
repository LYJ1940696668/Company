#include "bsp_iaq.h"

/*
	IAQ 传感器初始化
*/
void IAQ_CORE_Init(void)
{
	I2C_GPIO_Config();
}

/*
	IAQ传感器数据获取
	First functional reading after start up:5 minutes(datasheet page 3)
*/
void IAQ_CORE_Read(st_iaq_core *iaq)
{
	I2C_Start();
	I2C_SendByte(IAQ_CORE_READ_ADDR);
	if( I2C_WaitAck() == ACK )
	{
		//Prediction (CO2 eq. ppm)
		iaq->co2 = (u16)I2C_Read_Byte_CKLWAIT(1) << 8; 
		iaq->co2 |= I2C_Read_Byte_CKLWAIT(1);	
		
		//status:
		//	0x00:OK
		//	0x10: RUNIN (module in warm up phase)
		//	0x01: BUSY (re-read multi byte data!)
		//	0x80: ERROR (if constant: replace sensor)
		//
		iaq->sta = I2C_Read_Byte_CKLWAIT(1); 
		
		//Sensor resistance [Ohm]
		iaq->res = (u32)I2C_Read_Byte_CKLWAIT(1);//
		iaq->res = (u32)I2C_Read_Byte_CKLWAIT(1) << 16;
		iaq->res |= (u32)I2C_Read_Byte_CKLWAIT(1) << 8;
		iaq->res |= (u32)I2C_Read_Byte_CKLWAIT(1);
		
		//Prediction (TVOC eq. ppb)
		iaq->tvoc = (u16)I2C_Read_Byte_CKLWAIT(1) << 8;
		iaq->tvoc |= (u16)I2C_Read_Byte_CKLWAIT(0);
	}

	I2C_Stop();
}
