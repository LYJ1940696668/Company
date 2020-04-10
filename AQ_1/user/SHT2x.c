#include "SHT2x.h"
#include "mstimer.h"
#include "calendar.h"
/*��ʼ��GPIOB6��GPIOB7*/
void SHT_I2C_Init_Config(void)//3?��??��
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = I2C_GPIO_SCL | I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��?������?3?
    GPIO_Init(GPIO_I2C, &GPIO_InitStructure);	
	I2C_SDA_H;
	I2C_SCL_H;
}

/*����������SDA�ķ��������뻹����������������������˵�����*/
void SHT_I2C_SDA_OUT(void)//?��??��??��?������?�̨�?��?3?
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��?������?3?
	GPIO_Init(GPIO_I2C, &GPIO_InitStructure);
}

void SHT_I2C_SDA_IN(void)//?��??����?��?������?�̨�?��?��?
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	  
    GPIO_InitStructure.GPIO_Pin = I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//��?��-��?��?
    GPIO_Init(GPIO_I2C, &GPIO_InitStructure);
}

/*********SHT2x��ʼ������������ʼ�ź�****************/
/*��ʼ�źŹ��̣���SCL�ߵ�ƽʱ��SDA�ɸߵ�ƽת��Ϊ�͵�ƽ��
��ʼ״̬�����������Ƶ�һ�����������״̬��ָʾ�ӻ�
���俪ʼ��Start ֮��BUS ����һ�㱻��Ϊ����ռ��״̬��*/
void SHT_I2C_Start(void)
{
	SHT_I2C_SDA_OUT();//STM32�����źţ������������źţ�SDAΪ�����

	I2C_SCL_H; //SCL��Ϊ�ߵ�ƽ
	I2C_SDA_H; //SDA��Ϊ�ߵ�ƽ 
    delay_us(5); //��ʱ  
    I2C_SDA_L;  //SDA��Ϊ�͵�ƽ
    delay_us(6); //��ʱ
    I2C_SCL_L;		
}

/**********SHT2x����������ֹͣ�ź�********************/
/*�������̣���SCL�ߵ�ƽʱ��SDA�ɵ͵�ƽת��Ϊ�ߵ�ƽ,ֹͣ
״̬�����������Ƶ�һ�����������״̬��ָʾ�ӻ��������
��Stop ֮��BUS ����һ�㱻��Ϊ��������״̬����*/
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

/*��������Ӧ���ź�ACK�����������֣���Ӧ�����Ӧ��*/

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

/*********�ȴ�Ӧ��****************/
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
	//��ȡSDA�ź��ߵ�״̬
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

/*********����һ��8λ�ֽڵ�����**********/
u8 SHT_I2C_WriteByte(u8 txd)
{		
	u8 i;
	SHT_I2C_SDA_OUT();
		
	I2C_SCL_L;	//����ʱ��׼������
		 
	for(i=0;i<8;i++)
	{
		delay_us(2);
		if((txd&0x80)>0)  		//�ж����λ��1����0д
		{
			I2C_SDA_H;					
			delay_us(1);
		}		
		else
		{
			I2C_SDA_L;         //�ӻ�Ӧ�𣬵ȴ�������������
			delay_us(1);
		}
		I2C_SCL_H;
		delay_us(3);					//��SCL�ߵ�ƽ�ڼ䱣�������ȶ�
		I2C_SCL_L;

		txd <<= 1;				//������һλ����
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

/*******I2C��ȡһ��8λ�ֽ�************/
u8 SHT_I2C_ReadByte(void)
{
	u8 i = 0,ReadData = 0;
	SHT_I2C_SDA_IN();
		
	for(i=0;i<8;i++)
	{
		I2C_SCL_L;
		I2C_SDA_H;			//��������
		delay_us(2);
		I2C_SCL_H;			//���������ȶ�
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

/*********��ʼ��SHT2x**************/
void SHT21_Init(void)
{
	SHT_I2C_Init_Config();  //I2C��ʼ��
	SHT_I2C_Start();		//����I2C
	SHT_I2C_WriteByte(SHT21ADDR&0xfe); //дI2C�ĵ�ַ��
	SHT_I2C_WriteByte(0xfe);//��λ   
	SHT_I2C_Stop();//ֹͣI2C
}

//===================д�Ĵ����ĺ���======================
//void Set_Resolution(void)
//{
//	SHT_I2C_Start();
//	if(SHT_I2C_WriteByte(SHT21ADDR&0xfe)==ACK)		//D��I2C��?��??��
//	{
//		if(SHT_I2C_WriteByte(0xe7)==ACK)			//D��??��??��
//		{
//			if(SHT_I2C_WriteByte(0x83)==ACK);			//����????��??�¦�?��?��??��
//		}
//	}
//	SHT_I2C_Stop();
//}
//float  t[12],i=0;
float  temperature;
int temperature_flag=0;
//==================SHT21�����¶Ⱥ���==================
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
	SHT_I2C_WriteByte(SHT21ADDR|0x01);//��֤ʹI2C����
	MSB = SHT_I2C_ReadByte();
	SHT_Make_Ack();
	LSB = SHT_I2C_ReadByte();
	SHT_Make_Ack();
	SHT_I2C_ReadByte();
	SHT_Make_Nack();
	SHT_I2C_Stop();
	delay_ms(100);
	LSB &= 0xfc;//����������ת��Ϊ�������ݵ�ʱ��Ҫ��LSB�������λ��Ϊ0
	temp = MSB*256+LSB;
	temperature = ((175.72)*((float)temp)/65536-46.85);//-1.3;
	temperature_flag=1;
	return temperature;
			
}
float  humidity;
int humidity_flag=0;
//===================SHT21����ʪ�Ⱥ���====================
float SHT21_Work_H(void)
{
	u8 MSB,LSB;
	float temp = 0;

	SHT_I2C_Start();
	SHT_I2C_WriteByte(0x80);
	SHT_I2C_WriteByte(0xf5);//�����¶ȵ�command
	SHT_I2C_Stop();
	delay_ms(85);		//������ʱ					
	SHT_I2C_Start();
	SHT_I2C_WriteByte(SHT21ADDR|0x01);// I2C address + read
	MSB = SHT_I2C_ReadByte();			//data MSB
	SHT_Make_Ack();								//ack
	LSB = SHT_I2C_ReadByte();			//data LSB
	SHT_Make_Ack();								//ack
	SHT_I2C_ReadByte();						//check sum  Nack
	SHT_I2C_Stop();	//stop i2c
	delay_ms(100);
	LSB &= 0xfc;//����������ת��Ϊ�������ݵ�ʱ��Ҫ��LSB�������λ��Ϊ0
	temp = MSB*256+LSB;			
	humidity = (125)*((float)temp)/65536-6;
	humidity_flag=1;
	return humidity;
}

float Humidity,Temperature;
extern tm timer;
/*
	��¼�տ���ʱ��ʱ�䣬���ڽ����¶�ƫ��
*/
uint8_t g_cBoot_StrapHour = 0;
uint8_t g_cBoot_StrapMin = 0;
float g_fBoot_StrapTem = 0;
/**
  * @brief	���㿪��ʱ��
  * @param	��
  * @retval	����ʱ��
  */
//10:59 start 11:20
static uint8_t Boot_StrapTime(void)
{
	uint8_t Time,Hour,Min;
	
	Hour = timer.hour - g_cBoot_StrapHour;
	Time = Hour*100 + timer.min - g_cBoot_StrapMin - 40*Hour ;
	
	return Time;
}
/* ÿ1s��ȡһ������ */
float Read_T(void)
{
    static int step_T=0,ttim_t;
	rtc_get();
    switch(step_T)
    {
        case 0:
            MSTimerSet(ttim_t,1000);	       //��ʱ5s;
            step_T++;
        break;

        case 1:
            if(MSTimerCheck(ttim_t))        //5s��
            {
                SHT21_Work_T();
				if(g_fBoot_StrapTem <= 20)
				{
					if((temperature_flag == 1) && (Boot_StrapTime() <= 10))
					{
						Temperature = temperature - 0.2*Boot_StrapTime();  //�����¶�
					}
					if((temperature_flag == 1) 
						&& (Boot_StrapTime() > 10)
						&& (Boot_StrapTime() <= 30))
					{
						Temperature = temperature - 0.25*(Boot_StrapTime() - 10);  //�����¶�
					}
					if((temperature_flag == 1) && (Boot_StrapTime() > 30))
					{
						Temperature = temperature - 5.0;  //�����¶�
					}
				}
				else
				{
					if((temperature_flag == 1) && (Boot_StrapTime() <= 10))
					{
						Temperature = temperature - 0.2*Boot_StrapTime(); //�����¶�
					}
					if((temperature_flag == 1) 
						&& (Boot_StrapTime() > 10)
						&& (Boot_StrapTime() <= 30))
					{
						Temperature = temperature - 2 -0.1*(Boot_StrapTime() - 10);  //�����¶�
					}
					if((temperature_flag == 1) && (Boot_StrapTime() > 30))
					{
						Temperature = temperature - 4.0;  //�����¶�
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

/* ÿ1s��ȡһ������ */
float Read_H(void)
{

    static int step_H=0,htim_t;
    switch(step_H)
    {
        case 0:
            MSTimerSet(htim_t,1000);	       //��ʱ5s;
            step_H++;
        break;

        case 1:
            if(MSTimerCheck(htim_t))        //5s��
            {
                SHT21_Work_H();
                if(humidity_flag==1)
					Humidity=humidity;     //����ʪ��
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
