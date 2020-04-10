#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "SHT2x.h"
#include "calendar.h"
#include "led.h"
//unsigned char UART1_RX_BUF[UART1_RX_BUF_LEN]={0};
//unsigned char  *UART1_RX_PTR;
//unsigned char  *UART1_RD_PTR;
void USART_Configuration(void)
{
	/*-------------����ṹ�����----------*/
	USART_InitTypeDef	USART_InitStructure; 	
	GPIO_InitTypeDef	GPIO_InitStructure;
//	DMA_InitTypeDef     DMA_InitStructure; 
  	NVIC_InitTypeDef    NVIC_InitStructure;
	/*--------------ʹ����Ӧ�����ʱ��------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Configure PC8 and PC9 in output pushpull mode */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_2;//STATE,�͵�ƽ��Ч
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//SET,�͵�ƽ��Ч
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/***************����PA8*****************/
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//RST�ߵ�ƽ��Ч�������¼�����
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA,&GPIO_InitStructure);

	
	/*-------------����USART1_TX(PA.2)----------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	/*-------------����USART1_RX(Pa.3)----------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);      // ʹ�ܽ���
	



	USART_InitStructure.USART_BaudRate = 2400;		//����USART����Ĳ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;	//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//��һ֡���ݴ������ٴ�һλֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Odd;	//żУ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ܷ���ʹ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//Ӳ��������ʧ�ܣ���Ӳ�������ƣ�
	USART_Init(USART2,&USART_InitStructure);		//��ʼ��USART2�ļĴ���
	USART_Cmd(USART2,ENABLE);	//ʹ��USART2���裻
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);       //ʹ�ܽ����ж�
//	USART_ClearFlag(USART2,USART_IT_RXNE);
  
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 
 
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure); 		
	
}



uint8_t Rxbuf[1024];
u8 Rx_len=0;
u8 u5_receive[100];  //100bytes buffer
int u5_recount = 0;
int flag_receive=0;
void USART2_IRQHandler(void)
{
	u8 i=0,cs=0;

   if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);

		u5_receive[u5_recount]=USART_ReceiveData(USART2);
		u5_recount ++;

		while(u5_recount > 6 && u5_receive[0] != 0x68 && u5_receive[7]!=0x68)
		{
							// ��֡ͷ
			for(i = 1;i < u5_recount;i ++)		 			// ���������е�������ǰ��
			{
				u5_receive[i - 1] = u5_receive[i];						
			}
			u5_recount --;
		}  

		if(u5_recount <= 6)		
		{											 	// û���ҵ�֡ͷ��ֱ�ӷ���
			return;
		}  

		if(u5_recount<=(u5_receive[9]+11))
			return;
		if( u5_receive[u5_receive[9]+11]!=0x16 )//�ж�֡β
		{
			while(u5_recount > 6 && u5_receive[0] != 0x68 && u5_receive[7]!=0x68)
			{
								// ��֡ͷ
				for(i = 1;i < u5_recount;i ++)		 			// ���������е�������ǰ��
				{
					u5_receive[i - 1] = u5_receive[i];						
				}
				u5_recount --;
			}
			return;
		}
		for(i=0;i<(u5_receive[9]+10);i++)
		{
			cs+=u5_receive[i];
		}
		if(cs==(u5_receive[u5_receive[9]+10]))
		{
			memcpy(Rxbuf,u5_receive,u5_recount);
			Rx_len=u5_recount;
			u5_recount=0;
			flag_receive=1;
		}
	}	
//	carrier_receive();
}


uint8_t data_buf[9]={0};
extern float Temperature; 
extern float Humidity;
extern float pm2_5,CO2_ppm,CO2,pm;
static unsigned char min;
extern tm timer;


void data(void)
{
	float temp=0,temp_1=0;

	data_buf[0]=(int)CO2_ppm>>8; //co2���ֽ�
	data_buf[1]=(int)CO2_ppm & 0xFF;//co2���ֽ�

	temp=SHT21_Work_T();
	data_buf[2]=temp;          //�¶ȸ��ֽ�
	data_buf[3]=(int)((temp-data_buf[2])*10);  //�¶ȵ��ֽ�

	temp_1=SHT21_Work_H();
	data_buf[4]=temp_1;         //ʪ�ȸ��ֽ�
	data_buf[5]=(int)((temp_1-data_buf[4])*10);  //ʪ�ȵ��ֽ�
	
	data_buf[6]=(int)pm2_5>>8;      //pm2.5���ֽ�
	data_buf[7]=(int)pm2_5&0xFF;    //pm2.5���ֽ�
	data_buf[8]=0x00;

	Send_error_date();

}



/* У��� */
uint8_t send_buf[16]={0};        
void Send_date(void)
{
	uint8_t i=0; 
	rtc_get();
	send_buf[0]=timer.min;           //��
	send_buf[1]=timer.hour;          //ʱ
	send_buf[2]=timer.w_date;        //��
	send_buf[3]=timer.w_month;       //��
	send_buf[4]=timer.w_year&0xFF;    // ����ֽ�
	send_buf[5]=timer.w_year>>8;      //����ֽ�
	//CO2Ũ��ֵ
	send_buf[6]=data_buf[1];   //CO2���ֽ�
	send_buf[7]=data_buf[0];   //CO2���ֽ�
	//�¶�
	send_buf[8]=data_buf[2];   //�¶�����
	send_buf[9]=data_buf[3];   //�¶�С��
	//ʪ��
	send_buf[10]=data_buf[4];   //ʪ������
	send_buf[11]=data_buf[5];   //ʪ��С��
	//PM2.5
	send_buf[12]=data_buf[7];   //PM2.5���ֽ�
	send_buf[13]=data_buf[6];   //PM2.5���ֽ�
	send_buf[14]=data_buf[8];   //00
	for(i=0;i<=15;i++)
	{
		send_buf[15]+=send_buf[i];   //У���
	}
		 
}

uint8_t send_errorbuf[16]={0};
void Send_error_date(void)
{
	uint8_t i=0; 
	rtc_get();
    
    send_errorbuf[0]=~(timer.min+0x11);           //��
    send_errorbuf[1]=~(timer.hour+0x22);          //ʱ
    send_errorbuf[2]=~(timer.w_date+0x33);        //��
    send_errorbuf[3]=~(timer.w_month+0x44);       //��
    send_errorbuf[4]=~((timer.w_year&0xFF)+0x55);    // ����ֽ�
    send_errorbuf[5]=~((timer.w_year>>8)+0x66);      //����ֽ�
    //CO2Ũ��ֵ
    send_errorbuf[6]=~(data_buf[1]+0x77);   //CO2���ֽ�
    send_errorbuf[7]=~(data_buf[0]+0x88);   //CO2���ֽ�
    //�¶�
    send_errorbuf[8]=~(data_buf[2]+0x99);   //�¶�����
    send_errorbuf[9]=~(data_buf[3]+0xAA);   //�¶�С��
    //ʪ��
    send_errorbuf[10]=~(data_buf[4]+0xBB);   //ʪ������
    send_errorbuf[11]=~(data_buf[5]+0xCC);   //ʪ��С��
    //PM2.5
    send_errorbuf[12]=~(data_buf[7]+0xDD);   //PM2.5���ֽ�
    send_errorbuf[13]=~(data_buf[6]+0xEE);   //PM2.5���ֽ�
    send_errorbuf[14]=~(data_buf[8]+0xFF);   //00
    for(i=0;i<=15;i++)
    {
    send_errorbuf[15]+=send_errorbuf[i];   //У���
    }
		 
}

