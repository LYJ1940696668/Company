#include "hcho.h"


st_dev_hcho hcho;

uint8_t hcho_recv_data = 0, hcho_recv_finish_flag = 0;
uint8_t hcho_recv_recv[20] = {0};

void hcho_config(void)
{
    hcho.init           = hw_hcho_init;
    hcho.read_data      = hcho_read_data;
    hcho.set_read_mode  = hcho_set_mode;
}

/*******************************************************************************
* Function Name  : hw_hcho_init     uart parameter: 9600,n,8,1
* Description    : hcho uart 4 init
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void hw_hcho_init(void)
{
/*-------------����ṹ�����----------*/
	USART_InitTypeDef	USART_InitStructure; 	
	GPIO_InitTypeDef	GPIO_InitStructure;
  	NVIC_InitTypeDef    NVIC_InitStructure;
	/*--------------ʹ����Ӧ�����ʱ��------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

	
	/*-------------����USART4_TX(PB.10)----------*/
	GPIO_InitStructure.GPIO_Pin         = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	/*-------------����USART4_RX(PB.11)----------*/
	GPIO_InitStructure.GPIO_Pin         = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	
    GPIO_InitStructure.GPIO_Pin         = GPIO_Pin_9;	    		 //HCHO RST �˿�����, �������
    GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_9); 

	/*------------UART4�ĸ�����������----------*/
	USART_InitStructure.USART_BaudRate              = 9600;		//����USART3����Ĳ�����
	USART_InitStructure.USART_WordLength            = USART_WordLength_8b;	//8λ����λ
	USART_InitStructure.USART_StopBits              = USART_StopBits_1;	//��һ֡���ݴ������ٴ�һλֹͣλ
	USART_InitStructure.USART_Parity                = USART_Parity_No;	//żУ��
	USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;//���ܷ���ʹ��
	USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;	//Ӳ��������ʧ�ܣ���Ӳ�������ƣ�
	USART_Init(UART4,&USART_InitStructure);		                //��ʼ��USART3�ļĴ���
   
	USART_Cmd(UART4,ENABLE);	//ʹ��USART3���裻
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);       //ʹ�ܽ����ж�

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  //����NVIC�жϷ���
 
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;  //��ռ���ȼ�����Ϊ2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure); 	
}



static void hcho_set_mode(uint8_t mode)
{
    int i = 0;
	uint8_t mode_master[9]  = {0xff,0x01,0x78,0x40,0x00,0x00,0x00,0x00,0x47};
    uint8_t mode_slave[9]   = {0xff,0x01,0x78,0x41,0x00,0x00,0x00,0x00,0x46};
	
	if(mode == HCHO_MODE_MASTER)
    {
        for(i=0;i<=8;i++)                                   
        {
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);			
            USART_SendData(UART4,mode_master[i]);		
        } 
    } 
    
    if(mode == HCHO_MODE_SLAVE)
    {
        for(i=0;i<=8;i++)                                   
        {
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);			
            USART_SendData(UART4,mode_slave[i]);		
        } 
    } 
}


static uint8_t hcho_read_data(uint8_t mode)
{
    int i = 0;
    uint8_t read_cmd[9]   = {0xff,0x01,0x86,0x41,0x00,0x00,0x00,0x00,0x79};
    
    if(mode == HCHO_MODE_SLAVE)
    {
         /* raad data from buffer */
        if(hcho_recv_finish_flag)
        {
            hcho.value.ug_val = ((uint16_t)hcho_recv_recv[2] << 8) + hcho_recv_recv[3];
            hcho.value.ppb_val = ((uint16_t)hcho_recv_recv[6] << 8) + hcho_recv_recv[6];
   
            if( hcho.value.ug_val == 0 || hcho.value.ppb_val == 0)
                hcho.read_flag = 0;
            else
                hcho.read_flag = 1;
        }
        else
        {
            for(i=0;i<=8;i++)                                   
            {
                while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);			
                USART_SendData(UART4,read_cmd[i]);		
            }
            
            hcho.read_flag = 0;
        }
    }
    
    /* ��ջ���  */
    for(i=0;i<9;i++)
        hcho_recv_recv[0] = 0;
    hcho_recv_finish_flag = 0;
    hcho_recv_data = 0;
    
    return hcho.read_flag;
}

/*************************************************************************

*************************************************************************/
unsigned char FucCheckSum(unsigned char *i, unsigned char ln)
{
    unsigned char j, tempq=0;
    i+=1;
    
    for(j=0; j<(ln-2); j++)
    {
        tempq+=*i;
        i++;
    }
    
    tempq=(~tempq)+1;
    return(tempq);
}



void UART4_IRQHandler(void)
{
    int i;
	uint16_t CRC_temp1=0;
    uint8_t data = 0;
	
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//�����жϣ�
    { 	
        USART_ClearFlag(UART4,USART_IT_RXNE); 

        data = USART_ReceiveData(UART4);   //�������ݣ�
        
        if(hcho_recv_finish_flag == 0)
        {
            if(data == 0xff && hcho_recv_data == 0)
            {
                hcho_recv_recv[hcho_recv_data] = data;
            }
            else
            {
                hcho_recv_recv[++hcho_recv_data] = data;
            }
            
            if(hcho_recv_data >= 9)
                hcho_recv_finish_flag = 1;
        }
        
	}

}