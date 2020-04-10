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
	/*-------------定义结构体变量----------*/
	USART_InitTypeDef	USART_InitStructure; 	
	GPIO_InitTypeDef	GPIO_InitStructure;
//	DMA_InitTypeDef     DMA_InitStructure; 
  	NVIC_InitTypeDef    NVIC_InitStructure;
	/*--------------使能相应外设的时钟------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Configure PC8 and PC9 in output pushpull mode */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_2;//STATE,低电平有效
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推免输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//SET,低电平有效
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推免输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/***************配置PA8*****************/
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//RST高电平有效，即有事件发生
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA,&GPIO_InitStructure);

	
	/*-------------配置USART1_TX(PA.2)----------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	/*-------------配置USART1_RX(Pa.3)----------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);      // 使能接收
	



	USART_InitStructure.USART_BaudRate = 2400;		//设置USART传输的波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;	//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//在一帧数据传输完再传一位停止位
	USART_InitStructure.USART_Parity = USART_Parity_Odd;	//偶校验
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//接受发送使能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制失能（无硬件流控制）
	USART_Init(USART2,&USART_InitStructure);		//初始化USART2的寄存器
	USART_Cmd(USART2,ENABLE);	//使能USART2外设；
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);       //使能接收中断
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
							// 找帧头
			for(i = 1;i < u5_recount;i ++)		 			// 将缓冲区中的数据往前移
			{
				u5_receive[i - 1] = u5_receive[i];						
			}
			u5_recount --;
		}  

		if(u5_recount <= 6)		
		{											 	// 没有找到帧头，直接返回
			return;
		}  

		if(u5_recount<=(u5_receive[9]+11))
			return;
		if( u5_receive[u5_receive[9]+11]!=0x16 )//判断帧尾
		{
			while(u5_recount > 6 && u5_receive[0] != 0x68 && u5_receive[7]!=0x68)
			{
								// 找帧头
				for(i = 1;i < u5_recount;i ++)		 			// 将缓冲区中的数据往前移
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

	data_buf[0]=(int)CO2_ppm>>8; //co2高字节
	data_buf[1]=(int)CO2_ppm & 0xFF;//co2低字节

	temp=SHT21_Work_T();
	data_buf[2]=temp;          //温度高字节
	data_buf[3]=(int)((temp-data_buf[2])*10);  //温度低字节

	temp_1=SHT21_Work_H();
	data_buf[4]=temp_1;         //湿度高字节
	data_buf[5]=(int)((temp_1-data_buf[4])*10);  //湿度低字节
	
	data_buf[6]=(int)pm2_5>>8;      //pm2.5高字节
	data_buf[7]=(int)pm2_5&0xFF;    //pm2.5低字节
	data_buf[8]=0x00;

	Send_error_date();

}



/* 校验和 */
uint8_t send_buf[16]={0};        
void Send_date(void)
{
	uint8_t i=0; 
	rtc_get();
	send_buf[0]=timer.min;           //分
	send_buf[1]=timer.hour;          //时
	send_buf[2]=timer.w_date;        //日
	send_buf[3]=timer.w_month;       //月
	send_buf[4]=timer.w_year&0xFF;    // 年低字节
	send_buf[5]=timer.w_year>>8;      //年高字节
	//CO2浓度值
	send_buf[6]=data_buf[1];   //CO2低字节
	send_buf[7]=data_buf[0];   //CO2高字节
	//温度
	send_buf[8]=data_buf[2];   //温度整数
	send_buf[9]=data_buf[3];   //温度小数
	//湿度
	send_buf[10]=data_buf[4];   //湿度整数
	send_buf[11]=data_buf[5];   //湿度小数
	//PM2.5
	send_buf[12]=data_buf[7];   //PM2.5低字节
	send_buf[13]=data_buf[6];   //PM2.5高字节
	send_buf[14]=data_buf[8];   //00
	for(i=0;i<=15;i++)
	{
		send_buf[15]+=send_buf[i];   //校验和
	}
		 
}

uint8_t send_errorbuf[16]={0};
void Send_error_date(void)
{
	uint8_t i=0; 
	rtc_get();
    
    send_errorbuf[0]=~(timer.min+0x11);           //分
    send_errorbuf[1]=~(timer.hour+0x22);          //时
    send_errorbuf[2]=~(timer.w_date+0x33);        //日
    send_errorbuf[3]=~(timer.w_month+0x44);       //月
    send_errorbuf[4]=~((timer.w_year&0xFF)+0x55);    // 年低字节
    send_errorbuf[5]=~((timer.w_year>>8)+0x66);      //年高字节
    //CO2浓度值
    send_errorbuf[6]=~(data_buf[1]+0x77);   //CO2低字节
    send_errorbuf[7]=~(data_buf[0]+0x88);   //CO2高字节
    //温度
    send_errorbuf[8]=~(data_buf[2]+0x99);   //温度整数
    send_errorbuf[9]=~(data_buf[3]+0xAA);   //温度小数
    //湿度
    send_errorbuf[10]=~(data_buf[4]+0xBB);   //湿度整数
    send_errorbuf[11]=~(data_buf[5]+0xCC);   //湿度小数
    //PM2.5
    send_errorbuf[12]=~(data_buf[7]+0xDD);   //PM2.5低字节
    send_errorbuf[13]=~(data_buf[6]+0xEE);   //PM2.5高字节
    send_errorbuf[14]=~(data_buf[8]+0xFF);   //00
    for(i=0;i<=15;i++)
    {
    send_errorbuf[15]+=send_errorbuf[i];   //校验和
    }
		 
}

