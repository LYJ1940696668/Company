
#include "usart1.h"
#include "misc.h"
#include "mstimer.h"
#include "delay.h"

st_PM  PM_value;

/*
 * ��������USART1_Config
 * ����  ��USART1 GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
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
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //CONTROL_PM-->PF.7 �˿�����, �������
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(GPIOF, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOF,GPIO_Pin_7); 
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		 //CONTROL_TH-->PF.8 �˿�����, �������
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(GPIOF, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOF,GPIO_Pin_8); 
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	    		 //CONTROL_CO2-->PF.9 �˿�����, �������
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
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //ʹ�ܽ����ж�
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void send_command_pm(void)                                            //putchar����    
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


void send_beidongcommand(void)                                            //putchar����    
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

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)       //�����жϣ�
	{ 	
        USART_ClearFlag(USART1,USART_IT_RXNE); 
		u1_receive[u1_index]  = 	USART_ReceiveData(USART1);  //�������ݣ�
		u1_index++;                                             //���ݼ�һ
		
		while(u1_index>1 && (u1_receive[0]!=0x42 || u1_receive[1]!=0x4d))
		{
			for(i = 1;i < u1_index;i++)		 			        // ���������е�������ǰ��
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

			/* ��ȡ���������е�ֵ�����ǿ������Ӧ��ֵ */
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
            MSTimerSet(pm_t,5000);	                   //��ʱ5s��
            step_pm++;
        break;

        case 1:
            if(MSTimerCheck(pm_t))                    //5s����
            {
                send_command_pm();                        //����CO2���
                MSTimerSet(pm_r,1000);	                //��ʱ1s����������һ֡���ݣ�
                step_pm++; 
            }
        break;

        case 2:                                          
            if(!MSTimerCheck(pm_r))                  //1s����;
            {
                if(PM_flag==1)                            //���յ�����;
                {
                    pm2_5=pm;                          //����PM2.5��ֵ;
                    step_pm=0;                           //����;
                    PM_flag=0;                         //��־λ��0��
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
                step_pm++;                              //��case5���ж������Ƿ������;
            }
        break;

        case 3:                                            //���յ�����;
            if(PM_flag==1)
            {
                pm2_5=pm;                            //����PM2.5��ֵ;
                step_pm=0;                           //����;
                PM_flag=0;                           //��־λ��0��
            }
            else
            {
                step_pm++;                          //��case4���ж��ط������Ƿ�С��3��
            }
            break;

        case 4:
            if(sendpm_times<3)                    //�ط�������С��3;
            {
                sendpm_times++;                     //�ط�����+1;
                step_pm=0;                          //��case3,���·��Ͷ�ȡ����
            }
            else
            {
                pm2_5=0xFF;                          //���ش���
                step_pm=0;                             //����
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



