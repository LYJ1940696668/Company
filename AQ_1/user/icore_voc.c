#include "icore_voc.h"

#include "mstimer.h"

st_iaq_core tvoc;

/*初始化GPIOB6和GPIOB7*/
void VOC_I2C_Init_Config(void)//3?ê??ˉ
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(IAQ_CORE_I2C_GPIO_CLK,ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = IAQ_CORE_I2C_GPIO_SCL | IAQ_CORE_I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;

    GPIO_Init(IAQ_CORE_GPIO_I2C, &GPIO_InitStructure);	
	IAQ_CORE_I2C_SCL_H;
	IAQ_CORE_I2C_SDA_H;
}

/*定义数据线SDA的方向，是输入还是输出，这是相对于主机来说是输出*/
void VOC_I2C_SDA_OUT(void)//?à??ó??÷?úà′?μê?ê?3?
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(IAQ_CORE_I2C_GPIO_CLK,ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = IAQ_CORE_I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//í?íìê?3?
	GPIO_Init(IAQ_CORE_GPIO_I2C, &GPIO_InitStructure);
}

void VOC_I2C_SDA_IN(void)//?à??óú?÷?úà′?μê?ê?è?
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	  
    GPIO_InitStructure.GPIO_Pin = IAQ_CORE_I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//é?à-ê?è?
    GPIO_Init(IAQ_CORE_GPIO_I2C, &GPIO_InitStructure);
}

void I2C_SCL_OUT(void)//?à??ó??÷?úà′?μê?ê?3?
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(IAQ_CORE_I2C_GPIO_CLK,ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = IAQ_CORE_I2C_GPIO_SCL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(IAQ_CORE_GPIO_I2C, &GPIO_InitStructure);
}

void I2C_SCL_IN(void)//?à??óú?÷?úà′?μê?ê?è?
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	  
    GPIO_InitStructure.GPIO_Pin = IAQ_CORE_I2C_GPIO_SCL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//é?à-ê?è?
    GPIO_Init(IAQ_CORE_GPIO_I2C, &GPIO_InitStructure);
}

/*********SHT2x开始启动函数，开始信号****************/
/*开始信号过程：当SCL高电平时，SDA由高电平转换为低电平。
开始状态是由主机控制的一种特殊的总线状态，指示从机
传输开始（Start 之后，BUS 总线一般被认为处于占线状态）*/
void VOC_I2C_Start(void)
{
	VOC_I2C_SDA_OUT();//STM32发送信号，传感器接收信号（SDA为输出）

	IAQ_CORE_I2C_SCL_H; //SCL设为高电平
	IAQ_CORE_I2C_SDA_H; //SDA设为高电平 
    delay_us(5); //延时  
    IAQ_CORE_I2C_SDA_L;  //SDA设为低电平
    delay_us(6); //延时
    IAQ_CORE_I2C_SCL_L;		
}

/**********SHT2x结束函数，停止信号********************/
/*结束过程：当SCL高点平时，SDA由低电平转换为高电平,停止
状态是由主机控制的一种特殊的总线状态，指示从机传输结束
（Stop 之后，BUS 总线一般被认为处于闲置状态）。*/
void VOC_I2C_Stop(void)
{
	VOC_I2C_SDA_OUT(); //
	
	IAQ_CORE_I2C_SCL_L;
	IAQ_CORE_I2C_SDA_L;
    delay_us(3);
	IAQ_CORE_I2C_SCL_H;
	delay_us(5);
	IAQ_CORE_I2C_SDA_H;
	delay_us(6);
}

/*主机产生应答信号ACK函数，有两种：有应答和无应答*/

void VOC_Make_Ack(void)
{
	IAQ_CORE_I2C_SCL_L;
	VOC_I2C_SDA_OUT();

	IAQ_CORE_I2C_SDA_L;
	delay_us(2);
	IAQ_CORE_I2C_SCL_H;
	delay_us(5);
	IAQ_CORE_I2C_SCL_L;
	delay_us(2);
}
void VOC_Make_Nack(void)
{	
	IAQ_CORE_I2C_SCL_L;
	VOC_I2C_SDA_OUT();

	IAQ_CORE_I2C_SDA_H;
	delay_us(2);
	IAQ_CORE_I2C_SCL_H;
	delay_us(5);
	IAQ_CORE_I2C_SCL_L;
	delay_us(2);
}

/*********等待应答****************/
u8 VOC_WAIT_ACK()
{
	u8 tempTime = 0;

	IAQ_CORE_I2C_SCL_H;
	delay_us(2);
	IAQ_CORE_I2C_SDA_H;
	delay_us(2);
	VOC_I2C_SDA_IN();
	IAQ_CORE_I2C_SCL_L;      //此处有些不同，调试的时候需要留意。在某人的调试手册的例程里SCL为高。
	delay_us(2);
	while(GPIO_ReadInputDataBit(IAQ_CORE_GPIO_I2C,IAQ_CORE_I2C_GPIO_SDA))
	//读取SDA信号线的状态
	{
		tempTime++;
		delay_us(1);
		if(tempTime > 10)
		{
			VOC_I2C_Stop();      //此处存在不同，在某人的调试手册的离城里，只有SCL = 0;不是stop信号。
			return NACK;
		}		
	}
	IAQ_CORE_I2C_SCL_L;
	return ACK;		
}


/**
  @brief 
  */

/**
  * @brief  该函数用于Clock Stretching（时钟延展）特性器件，判断时钟线是否已经恢复为高电平。
  * @param  void
  * @retval SCL为高电平，返回TRUE,SCL为低电平返回FALSE。
  */

u8 VOC_I2C_WAIT_SCL_HIGH(void)
{
    u8 tempTime = 0;
    
    I2C_SCL_IN();
//    delay_us(1);
    while(!GPIO_ReadInputDataBit(IAQ_CORE_GPIO_I2C,IAQ_CORE_I2C_GPIO_SCL))
    {
        tempTime++;
		delay_us(2);
		if(tempTime > 100)
		{
			I2C_SCL_OUT();      
			return FALSE;
		}		
    }
    
    I2C_SCL_OUT();
    return TRUE;
}


/*********发送一个8位字节的数据**********/
u8 VOC_I2C_WriteByte(u8 txd)
{		
	u8 i;
	VOC_I2C_SDA_OUT();
		
	IAQ_CORE_I2C_SCL_L;	//拉低时钟准备发送
		 
	for(i=0;i<8;i++)
	{
		delay_us(2);
		if((txd&0x80)>0)  		//判断最高位，1读，0写
		{
			IAQ_CORE_I2C_SDA_H;					
			delay_us(1);
		}		
		else
		{
			IAQ_CORE_I2C_SDA_L;         //从机应答，等待主机发送数据
			delay_us(1);
		}
			IAQ_CORE_I2C_SCL_H;
			delay_us(3);					//在SCL高电平期间保持数据稳定
			IAQ_CORE_I2C_SCL_L;

			txd <<= 1;				//发送下一位数据
	}
	VOC_I2C_SDA_IN();
	delay_us(2);
	IAQ_CORE_I2C_SCL_H;
	delay_us(3);
	if(GPIO_ReadInputDataBit(IAQ_CORE_GPIO_I2C,IAQ_CORE_I2C_GPIO_SDA))
	{
		IAQ_CORE_I2C_SCL_L;
		return NACK;	
	}
	else
	{
		IAQ_CORE_I2C_SCL_L;
		return ACK;
	}
}

/*******I2C读取一个8位字节************/
u8 VOC_I2C_ReadByte(void)
{
	u8 i = 0,ReadData = 0;
	VOC_I2C_SDA_IN();
		
	for(i=0;i<8;i++)
	{
		IAQ_CORE_I2C_SCL_L;
		IAQ_CORE_I2C_SDA_H;			//接收数据
		delay_us(2);
		IAQ_CORE_I2C_SCL_H;			//保持数据稳定
		delay_us(2);
		ReadData <<= 1;
	
		if(GPIO_ReadInputDataBit(IAQ_CORE_GPIO_I2C, IAQ_CORE_I2C_GPIO_SDA))
		{
			ReadData |= 0x01; 
			delay_us(2);
		}
		else
		{
			ReadData &= 0xfe;   
		}
	}
	IAQ_CORE_I2C_SCL_L;
	return ReadData;
}



/**
  * @brief  该函数用于Clock Stretching（时钟延展）特性器件，slave设备在读取时会主动拉低SCL，必须要执行等待SCL释放。
  * @param  void
  * @retval 返回读取的数据
  */
u8 VOC_I2C_ReadByte_ClkStretching(void)
{
    u8 i = 0,ReadData = 0;
	VOC_I2C_SDA_IN();
		
	for(i=0;i<8;i++)
	{
		IAQ_CORE_I2C_SCL_L;
		IAQ_CORE_I2C_SDA_H;			//接收数据
		delay_us(2);
		IAQ_CORE_I2C_SCL_H;			//保持数据稳定
		delay_us(2);
		ReadData <<= 1;
	
        /* 等待SLAVE设备释放SCL信号 */
        if(FALSE == VOC_I2C_WAIT_SCL_HIGH())
            return FALSE;
        /* 读取数据 */
		if(GPIO_ReadInputDataBit(IAQ_CORE_GPIO_I2C, IAQ_CORE_I2C_GPIO_SDA))
		{
			ReadData |= 0x01; 
			delay_us(2);
		}
		else
		{
			ReadData &= 0xfe;   
		}
	}
	IAQ_CORE_I2C_SCL_L;
	return ReadData;
}

/*********初始化SHT2x**************/
void IAQ_CORE_Init(void)
{
	VOC_I2C_Init_Config();  //I2C初始化
}

void IAQ_CORE_read(st_iaq_core *iaq)
{
    VOC_I2C_Start();
    VOC_I2C_WriteByte(IAQ_CORE_READ_ADDR | 0X01);  // 读传感器数据
     
    if(ACK == VOC_WAIT_ACK() )
    {
        /* 开始读取数据 */
        iaq->co2 = (u16)(VOC_I2C_ReadByte_ClkStretching() ) << 8;  //co2 
        iaq->co2 |= VOC_I2C_ReadByte_ClkStretching();
        
        iaq->sta = VOC_I2C_ReadByte_ClkStretching();
        
        iaq->res = VOC_I2C_ReadByte_ClkStretching();
        iaq->res = (u32)VOC_I2C_ReadByte_ClkStretching() << 16;
        iaq->res |= (u32)VOC_I2C_ReadByte_ClkStretching() << 8;
        iaq->res |= (u32)VOC_I2C_ReadByte_ClkStretching();
        
        iaq->tvoc = (u16)VOC_I2C_ReadByte_ClkStretching();
        iaq->tvoc |= VOC_I2C_ReadByte_ClkStretching();
    }
    
    VOC_I2C_Stop();
     
}









