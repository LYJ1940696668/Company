#include "icore_voc.h"

#include "mstimer.h"

st_iaq_core tvoc;

/*��ʼ��GPIOB6��GPIOB7*/
void VOC_I2C_Init_Config(void)//3?��??��
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

/*����������SDA�ķ��������뻹����������������������˵�����*/
void VOC_I2C_SDA_OUT(void)//?��??��??��?������?�̨�?��?3?
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(IAQ_CORE_I2C_GPIO_CLK,ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = IAQ_CORE_I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��?������?3?
	GPIO_Init(IAQ_CORE_GPIO_I2C, &GPIO_InitStructure);
}

void VOC_I2C_SDA_IN(void)//?��??����?��?������?�̨�?��?��?
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	  
    GPIO_InitStructure.GPIO_Pin = IAQ_CORE_I2C_GPIO_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//��?��-��?��?
    GPIO_Init(IAQ_CORE_GPIO_I2C, &GPIO_InitStructure);
}

void I2C_SCL_OUT(void)//?��??��??��?������?�̨�?��?3?
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(IAQ_CORE_I2C_GPIO_CLK,ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = IAQ_CORE_I2C_GPIO_SCL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(IAQ_CORE_GPIO_I2C, &GPIO_InitStructure);
}

void I2C_SCL_IN(void)//?��??����?��?������?�̨�?��?��?
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	  
    GPIO_InitStructure.GPIO_Pin = IAQ_CORE_I2C_GPIO_SCL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//��?��-��?��?
    GPIO_Init(IAQ_CORE_GPIO_I2C, &GPIO_InitStructure);
}

/*********SHT2x��ʼ������������ʼ�ź�****************/
/*��ʼ�źŹ��̣���SCL�ߵ�ƽʱ��SDA�ɸߵ�ƽת��Ϊ�͵�ƽ��
��ʼ״̬�����������Ƶ�һ�����������״̬��ָʾ�ӻ�
���俪ʼ��Start ֮��BUS ����һ�㱻��Ϊ����ռ��״̬��*/
void VOC_I2C_Start(void)
{
	VOC_I2C_SDA_OUT();//STM32�����źţ������������źţ�SDAΪ�����

	IAQ_CORE_I2C_SCL_H; //SCL��Ϊ�ߵ�ƽ
	IAQ_CORE_I2C_SDA_H; //SDA��Ϊ�ߵ�ƽ 
    delay_us(5); //��ʱ  
    IAQ_CORE_I2C_SDA_L;  //SDA��Ϊ�͵�ƽ
    delay_us(6); //��ʱ
    IAQ_CORE_I2C_SCL_L;		
}

/**********SHT2x����������ֹͣ�ź�********************/
/*�������̣���SCL�ߵ�ƽʱ��SDA�ɵ͵�ƽת��Ϊ�ߵ�ƽ,ֹͣ
״̬�����������Ƶ�һ�����������״̬��ָʾ�ӻ��������
��Stop ֮��BUS ����һ�㱻��Ϊ��������״̬����*/
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

/*��������Ӧ���ź�ACK�����������֣���Ӧ�����Ӧ��*/

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

/*********�ȴ�Ӧ��****************/
u8 VOC_WAIT_ACK()
{
	u8 tempTime = 0;

	IAQ_CORE_I2C_SCL_H;
	delay_us(2);
	IAQ_CORE_I2C_SDA_H;
	delay_us(2);
	VOC_I2C_SDA_IN();
	IAQ_CORE_I2C_SCL_L;      //�˴���Щ��ͬ�����Ե�ʱ����Ҫ���⡣��ĳ�˵ĵ����ֲ��������SCLΪ�ߡ�
	delay_us(2);
	while(GPIO_ReadInputDataBit(IAQ_CORE_GPIO_I2C,IAQ_CORE_I2C_GPIO_SDA))
	//��ȡSDA�ź��ߵ�״̬
	{
		tempTime++;
		delay_us(1);
		if(tempTime > 10)
		{
			VOC_I2C_Stop();      //�˴����ڲ�ͬ����ĳ�˵ĵ����ֲ������ֻ��SCL = 0;����stop�źš�
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
  * @brief  �ú�������Clock Stretching��ʱ����չ�������������ж�ʱ�����Ƿ��Ѿ��ָ�Ϊ�ߵ�ƽ��
  * @param  void
  * @retval SCLΪ�ߵ�ƽ������TRUE,SCLΪ�͵�ƽ����FALSE��
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


/*********����һ��8λ�ֽڵ�����**********/
u8 VOC_I2C_WriteByte(u8 txd)
{		
	u8 i;
	VOC_I2C_SDA_OUT();
		
	IAQ_CORE_I2C_SCL_L;	//����ʱ��׼������
		 
	for(i=0;i<8;i++)
	{
		delay_us(2);
		if((txd&0x80)>0)  		//�ж����λ��1����0д
		{
			IAQ_CORE_I2C_SDA_H;					
			delay_us(1);
		}		
		else
		{
			IAQ_CORE_I2C_SDA_L;         //�ӻ�Ӧ�𣬵ȴ�������������
			delay_us(1);
		}
			IAQ_CORE_I2C_SCL_H;
			delay_us(3);					//��SCL�ߵ�ƽ�ڼ䱣�������ȶ�
			IAQ_CORE_I2C_SCL_L;

			txd <<= 1;				//������һλ����
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

/*******I2C��ȡһ��8λ�ֽ�************/
u8 VOC_I2C_ReadByte(void)
{
	u8 i = 0,ReadData = 0;
	VOC_I2C_SDA_IN();
		
	for(i=0;i<8;i++)
	{
		IAQ_CORE_I2C_SCL_L;
		IAQ_CORE_I2C_SDA_H;			//��������
		delay_us(2);
		IAQ_CORE_I2C_SCL_H;			//���������ȶ�
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
  * @brief  �ú�������Clock Stretching��ʱ����չ������������slave�豸�ڶ�ȡʱ����������SCL������Ҫִ�еȴ�SCL�ͷš�
  * @param  void
  * @retval ���ض�ȡ������
  */
u8 VOC_I2C_ReadByte_ClkStretching(void)
{
    u8 i = 0,ReadData = 0;
	VOC_I2C_SDA_IN();
		
	for(i=0;i<8;i++)
	{
		IAQ_CORE_I2C_SCL_L;
		IAQ_CORE_I2C_SDA_H;			//��������
		delay_us(2);
		IAQ_CORE_I2C_SCL_H;			//���������ȶ�
		delay_us(2);
		ReadData <<= 1;
	
        /* �ȴ�SLAVE�豸�ͷ�SCL�ź� */
        if(FALSE == VOC_I2C_WAIT_SCL_HIGH())
            return FALSE;
        /* ��ȡ���� */
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

/*********��ʼ��SHT2x**************/
void IAQ_CORE_Init(void)
{
	VOC_I2C_Init_Config();  //I2C��ʼ��
}

void IAQ_CORE_read(st_iaq_core *iaq)
{
    VOC_I2C_Start();
    VOC_I2C_WriteByte(IAQ_CORE_READ_ADDR | 0X01);  // ������������
     
    if(ACK == VOC_WAIT_ACK() )
    {
        /* ��ʼ��ȡ���� */
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









