
#include "Protol_3762.h"
#include "GDW376_2.h"
#include "usart2.h"
#include "usart3.h"
#include "mstimer.h"
#include "led.h"
#include "sd_spi.h"
#include "RTC.h"
#include "calendar.h"
//uint8_t HJ_addr[6]={0x36,0x77,0x44,0x00,0x10,0x10};
uint8_t HJ_addr[6];
uint8_t Mor_add[6]={0xAB,0x89,0x67,0x45,0x23,0x01};
uint8_t Sendbuf_3762[2048]={0};
uint16_t GDW3762ReLen;                //解帧后数组的长度
uint8_t Analyze_Rxbuf[20];

uint16_t SendSize_3762 = 0;
int flag_transmit=0;
extern uint8_t send_buf[16];
extern uint8_t send_errorbuf[16];
extern char read_dat[44];
//extern uint8_t send_buf[10];


uint16_t RxLen_3762 = 0;  //接收到帧的大小
uint16_t Monitoring_3762(void)
{
	uint16_t i=0;
	SendSize_3762=31;	
    
    ReadFromSD_address(read_dat,16);	//SD卡读地址	

	HJ_addr[0]=((read_dat[0]-0x30)<<4)+(read_dat[1]-0x30);
	HJ_addr[1]=((read_dat[2]-0x30)<<4)+(read_dat[3]-0x30);
	HJ_addr[2]=((read_dat[4]-0x30)<<4)+(read_dat[5]-0x30);
	HJ_addr[3]=((read_dat[6]-0x30)<<4)+(read_dat[7]-0x30);
	HJ_addr[4]=((read_dat[8]-0x30)<<4)+(read_dat[9]-0x30);
	HJ_addr[5]=((read_dat[10]-0x30)<<4)+(read_dat[11]-0x30);    //把sd卡的数据转化成16进制
	read_dat[0]=0;
	
	Sendbuf_3762[PHead]=0x68;     //帧头	  //0

	//环境表地址
	for(i=0;i<=5;i++)						  //1-6
	{
	 Sendbuf_3762[i+1]=HJ_addr[i];	 //
	}
	Sendbuf_3762[7]=0x68;
	Sendbuf_3762[8]=0x91;
	Sendbuf_3762[9]=0x14;
	Sendbuf_3762[10]=0x33;
	Sendbuf_3762[11]=0x33;
	Sendbuf_3762[12]=0x33;
	Sendbuf_3762[13]=0x33;
	for(i=0;i<16;i++)
	{
//	Sendbuf_3762[i+14]=send_buf[i];
        Sendbuf_3762[i+14]=send_errorbuf[i];
	}
	for(i=0;i<=(SendSize_3762-2);i++)			  
	{
		Sendbuf_3762[30] += Sendbuf_3762[i];	 //61
	}
	Sendbuf_3762[31]=0x16;


	for(i=0;i<=31;i++)                                   
    {
	    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//串口中断是否完成
		USART_SendData(USART2,Sendbuf_3762[i]);
	}
    Sendbuf_3762[30]=0;
    flag_transmit=1;
    return SendSize_3762;
}



uint16_t star_add(void)
{
	int i=0;
	SendSize_3762=18;
    
    ReadFromSD_address(read_dat,16);	  //SD卡读地址	
    
	HJ_addr[0]=((read_dat[0]-0x30)<<4)+(read_dat[1]-0x30);
	HJ_addr[1]=((read_dat[2]-0x30)<<4)+(read_dat[3]-0x30);
	HJ_addr[2]=((read_dat[4]-0x30)<<4)+(read_dat[5]-0x30);
	HJ_addr[3]=((read_dat[6]-0x30)<<4)+(read_dat[7]-0x30);
	HJ_addr[4]=((read_dat[8]-0x30)<<4)+(read_dat[9]-0x30);
	HJ_addr[5]=((read_dat[10]-0x30)<<4)+(read_dat[11]-0x30);    //把sd卡的数据转化成16进制
	read_dat[0]=0;
	
	Sendbuf_3762[PHead]=0x68;     //帧头	  //0

	Sendbuf_3762[1]=HJ_addr[0];
	Sendbuf_3762[2]=HJ_addr[1];
	Sendbuf_3762[3]=HJ_addr[2];
	Sendbuf_3762[4]=HJ_addr[3];
	Sendbuf_3762[5]=HJ_addr[4];
	Sendbuf_3762[6]=HJ_addr[5];                    //表地址
	Sendbuf_3762[7]=0x68;
	Sendbuf_3762[8]=0x93;//从站发出应答帧，从站正确应答，无后续数据帧，读通信数据
	Sendbuf_3762[9]=0x06;
	Sendbuf_3762[10]=HJ_addr[0];
	Sendbuf_3762[11]=HJ_addr[1];
	Sendbuf_3762[12]=HJ_addr[2];
	Sendbuf_3762[13]=HJ_addr[3];
	Sendbuf_3762[14]=HJ_addr[4];
	Sendbuf_3762[15]=HJ_addr[5];                  //表地址

	Sendbuf_3762[16]=0;
	for(i=0;i<(SendSize_3762-2);i++)			  
	{
		Sendbuf_3762[16] += Sendbuf_3762[i];	 //校验和
	}
	Sendbuf_3762[17]=0x16;                   //帧尾
	

	for(i=0;i<SendSize_3762;i++)                                   
    {
	    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
		USART_SendData(USART2,Sendbuf_3762[i]);

	}

	 flag_transmit=1;

}


uint16_t Back_time(void)
{
	uint16_t i=0;
	SendSize_3762=17;	
    
    ReadFromSD_address(read_dat,16);	//SD卡读地址	
    
	HJ_addr[0]=((read_dat[0]-0x30)<<4)+(read_dat[1]-0x30);
	HJ_addr[1]=((read_dat[2]-0x30)<<4)+(read_dat[3]-0x30);
	HJ_addr[2]=((read_dat[4]-0x30)<<4)+(read_dat[5]-0x30);
	HJ_addr[3]=((read_dat[6]-0x30)<<4)+(read_dat[7]-0x30);
	HJ_addr[4]=((read_dat[8]-0x30)<<4)+(read_dat[9]-0x30);
	HJ_addr[5]=((read_dat[10]-0x30)<<4)+(read_dat[11]-0x30);    //把sd卡的数据转化成16进制
	read_dat[0]=0;
	
	
	
	Sendbuf_3762[PHead]=0x68;     //帧头	  //0


	//环境表地址
	for(i=0;i<=5;i++)						  //1-6
	{
        Sendbuf_3762[i+1]=HJ_addr[i];	 //
	}
	Sendbuf_3762[7]=0x68;
	Sendbuf_3762[8]=0x91;
	Sendbuf_3762[9]=0x05;
	Sendbuf_3762[10]=0x33;
	Sendbuf_3762[11]=0x33;
	Sendbuf_3762[12]=0x33;
	Sendbuf_3762[13]=0x33;
    Sendbuf_3762[14]=0x34;
	for(i=0;i<(SendSize_3762-2);i++)			  
	{
		Sendbuf_3762[15] += Sendbuf_3762[i];	 //61
	}
    
	Sendbuf_3762[16]=0x16;


	for(i=0;i<=16;i++)                                   
    {
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//串口中断是否完成
        USART_SendData(USART2,Sendbuf_3762[i]);
    }
    Sendbuf_3762[15]=0;

    return SendSize_3762;
}

extern u8 Rx_len;
extern uint8_t Rxbuf[];

int BCD_Decimal(uint8_t bcd)
{
	int Decimal;
	Decimal=((int)(bcd>>4))*10+(int)(bcd&0x0f);
	return Decimal;
}

int mm,hh,DD,MM,YY;

void environment_list (void)
{
    if(Rx_len!=0) 
    {
        Rx_len=0;
    } 
    else 
        return;
    
	switch(Rxbuf[8])
	{
		case 0x13:
			star_add();                //上电载波获取表地址
		break;
		case 0x11:
			if((Rxbuf[1]==HJ_addr[0])&&(Rxbuf[2]==HJ_addr[1])&&(Rxbuf[3]==HJ_addr[2])&&(Rxbuf[4]==HJ_addr[3])&&(Rxbuf[5]==HJ_addr[4])&&(Rxbuf[6]==HJ_addr[5]))
			{
				switch(Rxbuf[9])
				{
					case 0x04:
						data();                    //组帧
						Monitoring_3762();         //把数据通过串口2发送给载波
					break;

					case 0x0A:
						mm = BCD_Decimal(Rxbuf[14]-0x33);
						hh = BCD_Decimal(Rxbuf[15]-0x33);
						DD = BCD_Decimal(Rxbuf[16]-0x33);
						MM = BCD_Decimal(Rxbuf[17]-0x33);
						YY = BCD_Decimal(Rxbuf[19]-0x33)+BCD_Decimal(Rxbuf[18]-0x33)*100;

						rtc_set(YY,MM,DD,hh,mm,0);
						Back_time();
					break;
			}
		}
		break;
		default:
		break;			 
	}
}





