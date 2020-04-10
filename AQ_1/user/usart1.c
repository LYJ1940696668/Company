
#include "usart1.h"
#include "misc.h"
#include "mstimer.h"
#include "delay.h"

st_PM  PM_value;

/*
 * 函数名：USART1_Config
 * 描述  ：USART1 GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void PM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //CONTROL_PM-->PF.7 端口配置, 推挽输出
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(GPIOF, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOF,GPIO_Pin_7); 
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		 //CONTROL_TH-->PF.8 端口配置, 推挽输出
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(GPIOF, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOF,GPIO_Pin_8); 
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	    		 //CONTROL_CO2-->PF.9 端口配置, 推挽输出
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(GPIOF, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOF,GPIO_Pin_9); 
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //使能接收中断
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void send_command_pm(void)                                            //putchar函数    
{      
    static int time=0;
    static uint16_t t; 
    int i=0,flag=0;
    uint8_t send[7]={0x42,0x4D,0xE2,0x00,0x00,0x01,0x71};

    for(i=0;i<=6;i++)                                   
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1,send[i]);		
    }     
}


void send_beidongcommand(void)                                            //putchar函数    
{   

    int i=0;
	uint8_t send1[7]={0x42,0x4D,0xE1,0x00,0x00,0x01,0x70};
	
	
	for(i=0;i<=6;i++)                                   
    {
	    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			
		USART_SendData(USART1,send1[i]);		
    }  
}




u8 u1_receive[100];
u8 u1_index=0;
//float  a[12];
int PM_flag=0;
u8 first_flag =0,i=0;
float pm=0,pm2_5=0;
 
void USART1_IRQHandler(void)
{
	int i;
	u16 sum_pm = 0;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)       //接收中断；
	{ 	
        USART_ClearFlag(USART1,USART_IT_RXNE); 
		u1_receive[u1_index]  = 	USART_ReceiveData(USART1);  //接收数据；
		u1_index++;                                             //数据加一
		
		while(u1_index>1 && (u1_receive[0]!=0x42 || u1_receive[1]!=0x4d))
		{
			for(i = 1;i < u1_index;i++)		 			        // 将缓冲区中的数据往前移
			{
				u1_receive[i - 1] = u1_receive[i];						
			}
			u1_index --;
			return;
		}
		if(first_flag == 0)
		{
			if(u1_index == 8)
			{					
				first_flag = 1;
				u1_index = 0;
			}
		}
		if(u1_index<=31 )
		return;


		for(i=0;i<=29;i++)
		{
			sum_pm=sum_pm+u1_receive[i];
		}	

		if((((sum_pm>>8)&0xFF)==u1_receive[30])&&((sum_pm&0xFF)==u1_receive[31]))
		{
			pm=(((u16)u1_receive[6]*256.0)+u1_receive[7]);

			/* 读取大气环境中的值，不是颗粒物对应的值 */
			PM_value.PM1_0 = (((u16)u1_receive[10]*256.0)+u1_receive[11]);
			PM_value.PM2_5 = (((u16)u1_receive[12]*256.0)+u1_receive[13]);
			PM_value.PM10  = (((u16)u1_receive[14]*256.0)+u1_receive[15]);

			u1_index =0;
			PM_flag=1;

		}
		else 
		{
			u1_index=0;
			return;
		}
	} 
}

void Read_PM(void)
{
    static int step_pm=0,sendpm_times=0;
    static uint16_t pm_t, pm_r;
    switch(step_pm)
    {

        case 0:
            MSTimerSet(pm_t,5000);	                   //定时5s；
            step_pm++;
        break;

        case 1:
            if(MSTimerCheck(pm_t))                    //5s到；
            {
                send_command_pm();                        //发送CO2命令；
                MSTimerSet(pm_r,1000);	                //定时1s，用来接收一帧数据；
                step_pm++; 
            }
        break;

        case 2:                                          
            if(!MSTimerCheck(pm_r))                  //1s不到;
            {
                if(PM_flag==1)                            //接收到数据;
                {
                    pm2_5=pm;                          //返回PM2.5数值;
                    step_pm=0;                           //清零;
                    PM_flag=0;                         //标志位清0；
                    break;
                }
                else 
                {
                    step_pm=2;
                    break;
                }

            }
            else
            {
                step_pm++;                              //到case5，判断数据是否接受完;
            }
        break;

        case 3:                                            //接收到数据;
            if(PM_flag==1)
            {
                pm2_5=pm;                            //返回PM2.5数值;
                step_pm=0;                           //清零;
                PM_flag=0;                           //标志位清0；
            }
            else
            {
                step_pm++;                          //到case4，判断重发次数是否小于3；
            }
            break;

        case 4:
            if(sendpm_times<3)                    //重发次数是小于3;
            {
                sendpm_times++;                     //重发次数+1;
                step_pm=0;                          //到case3,重新发送读取命令
            }
            else
            {
                pm2_5=0xFF;                          //返回错误
                step_pm=0;                             //清零
                sendpm_times=0;
            }
        break;

        default:
            PM_flag=0;
            step_pm=0;
            sendpm_times=0;
        break;			
    }

}



