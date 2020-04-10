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
/*-------------定义结构体变量----------*/
	USART_InitTypeDef	USART_InitStructure; 	
	GPIO_InitTypeDef	GPIO_InitStructure;
  	NVIC_InitTypeDef    NVIC_InitStructure;
	/*--------------使能相应外设的时钟------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);

	
	/*-------------配置USART4_TX(PB.10)----------*/
	GPIO_InitStructure.GPIO_Pin         = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	/*-------------配置USART4_RX(PB.11)----------*/
	GPIO_InitStructure.GPIO_Pin         = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	

	/*------------UART5的各个参数配置----------*/
	USART_InitStructure.USART_BaudRate              = 9600;		//设置USART5传输的波特率
	USART_InitStructure.USART_WordLength            = USART_WordLength_8b;	//8位数据位
	USART_InitStructure.USART_StopBits              = USART_StopBits_1;	//在一帧数据传输完再传一位停止位
	USART_InitStructure.USART_Parity                = USART_Parity_No;	//偶校验
	USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;//接受发送使能
	USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;	//硬件流控制失能（无硬件流控制）
	USART_Init(UART5,&USART_InitStructure);		                //初始化USART3的寄存器
   
	USART_Cmd(UART5,ENABLE);	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);       //使能接收中断

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  //设置NVIC中断分组
 
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级设置为2
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
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)        //接收中断；
	{ 	
		c_Temp = USART_ReceiveData(UART5);
//		USART_SendData(UART5,c_Temp);
		Recv_Data(c_Temp);
		comm_rev_buff[comm_rev_cnt++] = USART_ReceiveData(UART5);//接收数据；
		USART_ClearFlag(UART5,USART_IT_RXNE); 
    }
}




int fputc(int ch, FILE *f)
{ 
    USART_SendData(UART5, (uint8_t) ch); 
    while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);  
    return ch;
}