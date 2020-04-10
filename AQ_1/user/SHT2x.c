#include "SHT2x.h"
#include "mstimer.h"
#include "calendar.h"
/*初始化GPIOB6和GPIOB7*/
void SHT_I2C_Init_Config(void)//3?ê??ˉ
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = I2C_GPIO_SCL | I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//í?íìê?3?
    GPIO_Init(GPIO_I2C, &GPIO_InitStructure);	
	I2C_SDA_H;
	I2C_SCL_H;
}

/*定义数据线SDA的方向，是输入还是输出，这是相对于主机来说是输出*/
void SHT_I2C_SDA_OUT(void)//?à??ó??÷?úà′?μê?ê?3?
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//í?íìê?3?
	GPIO_Init(GPIO_I2C, &GPIO_InitStructure);
}

void SHT_I2C_SDA_IN(void)//?à??óú?÷?úà′?μê?ê?è?
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	  
    GPIO_InitStructure.GPIO_Pin = I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//é?à-ê?è?
    GPIO_Init(GPIO_I2C, &GPIO_InitStructure);
}

/*********SHT2x开始启动函数，开始信号****************/
/*开始信号过程：当SCL高电平时，SDA由高电平转换为低电平。
开始状态是由主机控制的一种特殊的总线状态，指示从机
传输开始（Start 之后，BUS 总线一般被认为处于占线状态）*/
void SHT_I2C_Start(void)
{
	SHT_I2C_SDA_OUT();//STM32发送信号，传感器接收信号（SDA为输出）

	I2C_SCL_H; //SCL设为高电平
	I2C_SDA_H; //SDA设为高电平 
    delay_us(5); //延时  
    I2C_SDA_L;  //SDA设为低电平
    delay_us(6); //延时
    I2C_SCL_L;		
}

/**********SHT2x结束函数，停止信号********************/
/*结束过程：当SCL高点平时，SDA由低电平转换为高电平,停止
状态是由主机控制的一种特殊的总线状态，指示从机传输结束
（Stop 之后，BUS 总线一般被认为处于闲置状态）。*/
void SHT_I2C_Stop(void)
{
	SHT_I2C_SDA_OUT(); //
	
	I2C_SCL_L;
	I2C_SDA_L;
	I2C_SCL_H;
	delay_us(5);
	I2C_SDA_H;
	delay_us(6);
}

/*主机产生应答信号ACK函数，有两种：有应答和无应答*/

void SHT_Make_Ack(void)
{
	I2C_SCL_L;
	SHT_I2C_SDA_OUT();

	I2C_SDA_L;
	delay_us(2);
	I2C_SCL_H;
	delay_us(5);
	I2C_SCL_L;
	delay_us(2);
}
void SHT_Make_Nack(void)
{	
	I2C_SCL_L;
	SHT_I2C_SDA_OUT();

	I2C_SDA_H;
	delay_us(2);
	I2C_SCL_H;
	delay_us(5);
	I2C_SCL_L;
	delay_us(2);
}

/*********等待应答****************/
u8 SHT_WAIT_ACK()
{
	u8 tempTime = 0;

	I2C_SCL_H;
	delay_us(2);
	I2C_SDA_H;
	delay_us(2);
	SHT_I2C_SDA_IN();
	I2C_SCL_L;
	delay_us(2);
	while(GPIO_ReadInputDataBit(GPIO_I2C,I2C_GPIO_SDA))
	//读取SDA信号线的状态
	{
		tempTime++;
		delay_us(1);
		if(tempTime > 10)
		{
			SHT_I2C_Stop();
			return NACK;
		}		
	}
	I2C_SCL_L;
	return ACK;		
}

/*********发送一个8位字节的数据**********/
u8 SHT_I2C_WriteByte(u8 txd)
{		
	u8 i;
	SHT_I2C_SDA_OUT();
		
	I2C_SCL_L;	//拉低时钟准备发送
		 
	for(i=0;i<8;i++)
	{
		delay_us(2);
		if((txd&0x80)>0)  		//判断最高位，1读，0写
		{
			I2C_SDA_H;					
			delay_us(1);
		}		
		else
		{
			I2C_SDA_L;         //从机应答，等待主机发送数据
			delay_us(1);
		}
		I2C_SCL_H;
		delay_us(3);					//在SCL高电平期间保持数据稳定
		I2C_SCL_L;

		txd <<= 1;				//发送下一位数据
	}
	SHT_I2C_SDA_IN();
	delay_us(2);
	I2C_SCL_H;
	delay_us(3);
	if(GPIO_ReadInputDataBit(GPIO_I2C,I2C_GPIO_SDA))
	{
		I2C_SCL_L;
		return NACK;	
	}
	else
	{
		I2C_SCL_L;
		return ACK;
	}
}

/*******I2C读取一个8位字节************/
u8 SHT_I2C_ReadByte(void)
{
	u8 i = 0,ReadData = 0;
	SHT_I2C_SDA_IN();
		
	for(i=0;i<8;i++)
	{
		I2C_SCL_L;
		I2C_SDA_H;			//接收数据
		delay_us(2);
		I2C_SCL_H;			//保持数据稳定
		delay_us(2);
		ReadData <<= 1;
	
		if(GPIO_ReadInputDataBit(GPIO_I2C,I2C_GPIO_SDA))
		{
			ReadData |= 0x01; 
			delay_us(2);
		}
		else
		{
			ReadData &= 0xfe;   
		}
	}
	I2C_SCL_L;
	return ReadData;
}

/*********初始化SHT2x**************/
void SHT21_Init(void)
{
	SHT_I2C_Init_Config();  //I2C初始化
	SHT_I2C_Start();		//启动I2C
	SHT_I2C_WriteByte(SHT21ADDR&0xfe); //写I2C的地址·
	SHT_I2C_WriteByte(0xfe);//软复位   
	SHT_I2C_Stop();//停止I2C
}

//===================写寄存器的函数======================
//void Set_Resolution(void)
//{
//	SHT_I2C_Start();
//	if(SHT_I2C_WriteByte(SHT21ADDR&0xfe)==ACK)		//D′I2Cμ?μ??·
//	{
//		if(SHT_I2C_WriteByte(0xe7)==ACK)			//D′??′??÷
//		{
//			if(SHT_I2C_WriteByte(0x83)==ACK);			//éè????′??÷μ?·?±??ê
//		}
//	}
//	SHT_I2C_Stop();
//}
//float  t[12],i=0;
float  temperature;
int temperature_flag=0;
//==================SHT21测量温度函数==================
float SHT21_Work_T(void)
{
	u8 MSB,LSB;
	float temp = 0;

	SHT_I2C_Start();
	SHT_I2C_WriteByte(0x80);
	SHT_I2C_WriteByte(0xf3);
	SHT_I2C_Stop();
	delay_ms(85);
	SHT_I2C_Start();
	SHT_I2C_WriteByte(SHT21ADDR|0x01);//保证使I2C工作
	MSB = SHT_I2C_ReadByte();
	SHT_Make_Ack();
	LSB = SHT_I2C_ReadByte();
	SHT_Make_Ack();
	SHT_I2C_ReadByte();
	SHT_Make_Nack();
	SHT_I2C_Stop();
	delay_ms(100);
	LSB &= 0xfc;//将测量数据转换为物理数据的时候，要将LSB的最后两位置为0
	temp = MSB*256+LSB;
	temperature = ((175.72)*((float)temp)/65536-46.85);//-1.3;
	temperature_flag=1;
	return temperature;
			
}
float  humidity;
int humidity_flag=0;
//===================SHT21测量湿度函数====================
float SHT21_Work_H(void)
{
	u8 MSB,LSB;
	float temp = 0;

	SHT_I2C_Start();
	SHT_I2C_WriteByte(0x80);
	SHT_I2C_WriteByte(0xf5);//测量温度的command
	SHT_I2C_Stop();
	delay_ms(85);		//测量延时					
	SHT_I2C_Start();
	SHT_I2C_WriteByte(SHT21ADDR|0x01);// I2C address + read
	MSB = SHT_I2C_ReadByte();			//data MSB
	SHT_Make_Ack();								//ack
	LSB = SHT_I2C_ReadByte();			//data LSB
	SHT_Make_Ack();								//ack
	SHT_I2C_ReadByte();						//check sum  Nack
	SHT_I2C_Stop();	//stop i2c
	delay_ms(100);
	LSB &= 0xfc;//将测量数据转换为物理数据的时候，要将LSB的最后两位置为0
	temp = MSB*256+LSB;			
	humidity = (125)*((float)temp)/65536-6;
	humidity_flag=1;
	return humidity;
}

float Humidity,Temperature;
extern tm timer;
/*
	记录刚开机时的时间，用于矫正温度偏差
*/
uint8_t g_cBoot_StrapHour = 0;
uint8_t g_cBoot_StrapMin = 0;
float g_fBoot_StrapTem = 0;
/**
  * @brief	计算开机时间
  * @param	无
  * @retval	开机时间
  */
//10:59 start 11:20
static uint8_t Boot_StrapTime(void)
{
	uint8_t Time,Hour,Min;
	
	Hour = timer.hour - g_cBoot_StrapHour;
	Time = Hour*100 + timer.min - g_cBoot_StrapMin - 40*Hour ;
	
	return Time;
}
/* 每1s读取一次数据 */
float Read_T(void)
{
    static int step_T=0,ttim_t;
	rtc_get();
    switch(step_T)
    {
        case 0:
            MSTimerSet(ttim_t,1000);	       //定时5s;
            step_T++;
        break;

        case 1:
            if(MSTimerCheck(ttim_t))        //5s到
            {
                SHT21_Work_T();
				if(g_fBoot_StrapTem <= 20)
				{
					if((temperature_flag == 1) && (Boot_StrapTime() <= 10))
					{
						Temperature = temperature - 0.2*Boot_StrapTime();  //返回温度
					}
					if((temperature_flag == 1) 
						&& (Boot_StrapTime() > 10)
						&& (Boot_StrapTime() <= 30))
					{
						Temperature = temperature - 0.25*(Boot_StrapTime() - 10);  //返回温度
					}
					if((temperature_flag == 1) && (Boot_StrapTime() > 30))
					{
						Temperature = temperature - 5.0;  //返回温度
					}
				}
				else
				{
					if((temperature_flag == 1) && (Boot_StrapTime() <= 10))
					{
						Temperature = temperature - 0.2*Boot_StrapTime(); //返回温度
					}
					if((temperature_flag == 1) 
						&& (Boot_StrapTime() > 10)
						&& (Boot_StrapTime() <= 30))
					{
						Temperature = temperature - 2 -0.1*(Boot_StrapTime() - 10);  //返回温度
					}
					if((temperature_flag == 1) && (Boot_StrapTime() > 30))
					{
						Temperature = temperature - 4.0;  //返回温度
					}
				}	

                temperature_flag = 0;
                step_T = 0;
            }

        break;
            
        default:
            temperature_flag=0;
            step_T=0;
        break;
    }		 
}

 float Humidity,Temperature;

/* 每1s读取一次数据 */
float Read_H(void)
{

    static int step_H=0,htim_t;
    switch(step_H)
    {
        case 0:
            MSTimerSet(htim_t,1000);	       //定时5s;
            step_H++;
        break;

        case 1:
            if(MSTimerCheck(htim_t))        //5s到
            {
                SHT21_Work_H();
                if(humidity_flag==1)
					Humidity=humidity;     //返回湿度
                humidity_flag=0;
                step_H=0;
            }

        break;
            
        default:
            humidity_flag=0;
            step_H=0;
        break;

    }

}
