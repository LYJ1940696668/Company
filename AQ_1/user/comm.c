#include "comm.h"
#include "HostProcess.h"

/*******************************************************************************
* Function Name  : hw_hcho_init     uart parameter: 9600,n,8,1
* Description    : hcho uart 4 init
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void comm_init(void)
{
/*-------------����ṹ�����----------*/
	USART_InitTypeDef	USART_InitStructure; 	
	GPIO_InitTypeDef	GPIO_InitStructure;
  	NVIC_InitTypeDef    NVIC_InitStructure;
	/*--------------ʹ����Ӧ�����ʱ��------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);

	
	/*-------------����USART4_TX(PB.10)----------*/
	GPIO_InitStructure.GPIO_Pin         = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	/*-------------����USART4_RX(PB.11)----------*/
	GPIO_InitStructure.GPIO_Pin         = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	

	/*------------UART5�ĸ�����������----------*/
	USART_InitStructure.USART_BaudRate              = 9600;		//����USART5����Ĳ�����
	USART_InitStructure.USART_WordLength            = USART_WordLength_8b;	//8λ����λ
	USART_InitStructure.USART_StopBits              = USART_StopBits_1;	//��һ֡���ݴ������ٴ�һλֹͣλ
	USART_InitStructure.USART_Parity                = USART_Parity_No;	//żУ��
	USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;//���ܷ���ʹ��
	USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;	//Ӳ��������ʧ�ܣ���Ӳ�������ƣ�
	USART_Init(UART5,&USART_InitStructure);		                //��ʼ��USART3�ļĴ���
   
	USART_Cmd(UART5,ENABLE);	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);       //ʹ�ܽ����ж�

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  //����NVIC�жϷ���
 
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�����Ϊ2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure); 	
}

void comm_send_byte(u8 ch)
{
    while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
    USART_SendData(UART5, ch);		
}

void comm_send_buff(u8 *buf)
{
    u8 len, i;
    len = sizeof(buf);
    while(*buf)
    {
        while(!USART_GetFlagStatus(UART5,USART_FLAG_TXE));
        USART_SendData(UART5, *buf);	
        while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
        buf++;
    }
}

u8 comm_rev_buff[256] = {0};
u8 comm_rev_cnt = 0;

void UART5_IRQHandler(void)
{
	int i;
	u16 sum_pm=0;
	uint8_t c_Temp;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)        //�����жϣ�
	{ 	
		c_Temp = USART_ReceiveData(UART5);
//		USART_SendData(UART5,c_Temp);
		Recv_Data(c_Temp);
		comm_rev_buff[comm_rev_cnt++] = USART_ReceiveData(UART5);//�������ݣ�
		USART_ClearFlag(UART5,USART_IT_RXNE); 
    }
}




int fputc(int ch, FILE *f)
{ 
    USART_SendData(UART5, (uint8_t) ch); 
    while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);  
    return ch;
}