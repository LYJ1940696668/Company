
#include "usart3.h"
#include "misc.h"
#include "mstimer.h"


void test_write_text(void);

/*
 * 函数名：USART1_Config
 * 描述  ：USART1 GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
 
void CO2_Init(void)
{
	/*-------------定义结构体变量----------*/
	USART_InitTypeDef	USART_InitStructure; 	
	GPIO_InitTypeDef	GPIO_InitStructure;
  	NVIC_InitTypeDef    NVIC_InitStructure;
	/*--------------使能相应外设的时钟------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	
	/*-------------配置USART3_TX(PB.10)----------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	/*-------------配置USART3_RX(PB.11)----------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	

	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	    		 //CONTROL_CO2-->PF.9 端口配置, 推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOF,GPIO_Pin_9); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		 //CONTROL_TH-->PF.8 端口配置, 推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOF,GPIO_Pin_8); 

	/*------------USART3的各个参数配置----------*/
	USART_InitStructure.USART_BaudRate = 19200;		//设置USART3传输的波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;	//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//在一帧数据传输完再传一位停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;	//偶校验
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//接受发送使能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制失能（无硬件流控制）
	USART_Init(USART3,&USART_InitStructure);		//初始化USART3的寄存器
   
	 
	 
	 
	USART_Cmd(USART3,ENABLE);	//使能USART3外设；
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);       //使能接收中断

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  //设置NVIC中断分组
 
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;  //抢占优先级设置为2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure); 		
}


void send_command_CO2(void)                                              
{    
    int i=0;

	uint8_t send_command[8]={0x15,0x04,0x13,0x8B,0x00,0x01,0x46,0x70};  //读取气体浓度值指令

	for(i=0;i<=7;i++)                                   
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		USART_SendData(USART3,send_command[i]);			               //发送读取气体浓度值指令
	}  
}


u8 u2_receive[100];
u8 u2_index=0;
int 	 CO2_flag=0;
 float CO2=0,CO2_ppm=0;

void USART3_IRQHandler(void)
{
	int i;
	uint16_t CRC_temp1=0;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收中断；
	{ 	
		USART_ClearFlag(USART3,USART_IT_RXNE); 
		u2_receive[u2_index]  = 	USART_ReceiveData(USART3);//接收数据；
		u2_index++;//数据加一

		while((u2_index>1) && (u2_receive[0]!=0x15||u2_receive[1]!=0x04))
		{
			for(i = 1;i < u2_index;i++)		 			//将缓冲区中的数据往前移
			{
				u2_receive[i - 1] = u2_receive[i];						
			}
			u2_index --;
			return ;
		}



		if(u2_receive[1]==0x04)
		{
			if(u2_index<u2_receive[2]+5) 
			return ;	
			CRC_temp1=(u2_receive[u2_index-2]<<8)|u2_receive[u2_index-1];
			if(CRC_temp1==CRC16_3(u2_receive,  u2_index-2))
			{
				CO2=((u2_receive[3]*256.0)+u2_receive[4]);
				if(CO2>5000)
					CO2=5000;
				u2_index =0;
				CO2_flag=1;
			}

		}

	} 


}



void Read_CO2(void)
{

	static int step_CO2=0,sendCO2_times=0;
	static uint16_t CO2tim_t,CO2tim_r;
	switch(step_CO2)  
	{
		case 0:
			MSTimerSet(CO2tim_t,5000);	                   //定时5s；
			step_CO2++;
		break;

		case 1:
			if(MSTimerCheck(CO2tim_t))                    //5s到；
			{
				send_command_CO2();                        //发送CO2命令；
				MSTimerSet(CO2tim_r,1000);	                //定时1s，用来接收一帧数据；
				step_CO2++; 
			}
		break;

		case 2:                                          
			if(!MSTimerCheck(CO2tim_r))                  //1s不到;
			{
				if(CO2_flag==1)                        //接收到数据;
				{
					CO2_ppm = CO2;                      //返回PM2.5数值;
					step_CO2 = 0;                       //清零;
					CO2_flag = 0;                       //标志位清0；
					break;
				}
				else 
				{
					step_CO2 = 2;
					break;
				}
			}
			else
			{
				step_CO2++;                              //到case3，判断数据是否接受完;
			}
		break;

		case 3:                                            //接收到数据;
			if(CO2_flag == 1)
			{
				CO2_ppm = CO2;                          //返回PM2.5数值;
				step_CO2 = 0;                           //清零;
				CO2_flag = 0;                           //标志位清0；
			}
			else
			{
				step_CO2++;                          //到case4，判断重发次数是否小于3；
			}
		break;

		case 4:
			if(sendCO2_times<3)                    //重发次数是小于3;
			{
				sendCO2_times++;                     //重发次数+1;
				step_CO2 = 0;                          //到case3,重新发送读取命令
			}
			else
			{
				step_CO2 = 0xFF;                          //返回错误
				step_CO2 = 0;                             //清零
				sendCO2_times = 0;
			}
		break;   
		default:
			step_CO2 = 0;                             
			sendCO2_times = 0;
			CO2_flag = 0;
		break;											
	}

}


const unsigned char Con_Crc_Hi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 };
const unsigned char Con_Crc_Lo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40 };


unsigned short int CRC1 = 0;
uint16_t CRC16_3(uint8_t Buff[],  uint8_t len)
{

    uint8_t i;
	uint8_t TempIndex, CRC16Hi, CRC16Lo;
	CRC16Hi = 0xFF;
	CRC16Lo = 0xFF;
	for (i = 0; i<len; i++)
	{
		TempIndex = CRC16Hi ^ Buff[i];
		CRC16Hi = CRC16Lo ^ Con_Crc_Hi[TempIndex];
		CRC16Lo = Con_Crc_Lo[TempIndex];
	}
//	CRC = (uint8_t)CRC16Hi + CRC16Lo * 256;
	CRC1= (CRC16Hi<<8)|CRC16Lo;
//	CRC1= (CRC16Lo<<8)|CRC16Hi;
	return CRC1;
}
/*

 



 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
//int fputc(int ch, FILE *f)
//{
	/* 将Printf内容发往串口 */
/*USART_SendData(USART1, (unsigned char) ch);
	while (!(USART1->SR & USART_FLAG_TXE));
	
	return (ch);
}*/

extern uint8_t dat1[4],dat2[3],dat3[4],dat4[4],dat[44],dat5[4];
extern uint8_t send_buf[16];

void test_write_text(void)
{
	CO2_ppm_TO_ascii();
	pm2_5_TO_ascii();
	Temperature_TO_ascii();
	Humidity_TO_ascii();		

	if(send_buf[1]<=0x09)
	{
		dat[0]=0x30;
		dat[1]=send_buf[1]+0x30;    //0时-9时
	}
	else if(send_buf[1]<=0x13)
	{
		dat[0]=0x31;
		dat[1]=send_buf[1]+0x26;    //10时-19时
	}
	else
	{
		dat[0]=0x32;
		dat[1]=send_buf[1]+0x1c;    //20时-23时
	}
	dat[2]=0x3A;                //冒号
	dat[3]=0x30;
	dat[4]=0x30;	              //0分
	dat[5]=0x20;
	dat[6]=0x20;                   //空格

	dat[7]=0x43;
	dat[8]=0x4f;
	dat[9]=0x32;                //CO2
	dat[10]=0x3A;               //冒号
	dat[11]=dat1[0];
	dat[12]=dat1[1];
	dat[13]=dat1[2];
	dat[14]=dat1[3];            //CO2数值
	dat[15]=0x20;
	dat[16]=0x20;                   //空格

	dat[17]=0x50;
	dat[18]=0x4d;
	dat[19]=0x32;
	dat[20]=0x2e;
	dat[21]=0x35;               //pm2.5
	dat[22]=0x3A;               //冒号
	dat[23]=dat2[0];
	dat[24]=dat2[1];
	dat[25]=dat2[2];            //pm数值
	dat[26]=0x20;
	dat[27]=0x20;                   //空格

	dat[28]=0x54;               //T
	dat[29]=0x3a;               //冒号
	dat[30]=dat3[0];
	dat[31]=dat3[1];
	dat[32]=dat3[2];
	dat[33]=dat3[3];            //温度数值
	dat[34]=0x20;
	dat[35]=0x20;                   //空格

	dat[36]=0x48;               //H
	dat[37]=0x3a;               //冒号
	dat[38]=dat4[0];
	dat[39]=dat4[1];
	dat[40]=dat4[2];
	dat[41]=dat4[3];            //湿度数值

	dat[42]=0x0d;               //回车
	dat[43]=0x0a;             //换行

	WriteToSD_Env(dat,16);
//        printf("write env data\n");
}

