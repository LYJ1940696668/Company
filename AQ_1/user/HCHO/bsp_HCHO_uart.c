#include "bsp_HCHO_uart.h"
#include <math.h>
uint8_t g_nHCHOData[9]; //�����жϴ���������
/*
	g_nRecvFlag ��
	��ȩ������1�뷢��һ��
	���յ�һ�����ݣ���1
	����������Ҫ�ֶ���0
*/
uint8_t g_nRecvFlag;
 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����UART4Ϊ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_UART4_IRQ;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  UART4 GPIO ����,������������
  * @param  ��
  * @retval ��
  */
static void UART4_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef UART4_InitStructure;

	// �򿪴���GPIO��ʱ��
	DEBUG_UART4_GPIO_APBxClkCmd(DEBUG_UART4_GPIO_CLK, ENABLE);
	
	// �򿪴��������ʱ��
	DEBUG_UART4_APBxClkCmd(DEBUG_UART4_CLK, ENABLE);

	// ��UART4 Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_UART4_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_UART4_TX_GPIO_PORT, &GPIO_InitStructure);

  // ��UART4 Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_UART4_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_UART4_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// ���ô��ڵĹ�������
	// ���ò�����
	UART4_InitStructure.USART_BaudRate = DEBUG_UART4_BAUDRATE;
	// ���� �������ֳ�
	UART4_InitStructure.USART_WordLength = USART_WordLength_8b;
	// ����ֹͣλ
	UART4_InitStructure.USART_StopBits = USART_StopBits_1;
	// ����У��λ
	UART4_InitStructure.USART_Parity = USART_Parity_No ;
	// ����Ӳ��������
	UART4_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// ���ù���ģʽ���շ�һ��
	UART4_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_UART4x, &UART4_InitStructure);
	
	// �����ж����ȼ�����
	NVIC_Configuration();
	
	// ʹ�ܴ��ڽ����ж�
	USART_ITConfig(DEBUG_UART4x, USART_IT_RXNE, ENABLE);	
	
	// ʹ�ܴ���
	USART_Cmd(DEBUG_UART4x, ENABLE);		

  // ���������ɱ�־
	//USART_ClearFlag(USART1, USART_FLAG_TC);     
}
static void HCHO_Set_Mode(void)
{
	int i;
	
	uint8_t mode_master[9]  = {0xff,0x01,0x78,0x41,0x00,0x00,0x00,0x00,0x46};
    uint8_t mode_slave[9]   = {0xff,0x01,0x78,0x40,0x00,0x00,0x00,0x00,0x47};
	
	if(HCHO_MODE == HCHO_MODE_MASTER)
    {
        for(i=0;i<=8;i++)                                   
        {
            while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);			
            USART_SendData(UART4,mode_master[i]);		
        } 
    } 
    
    if(HCHO_MODE == HCHO_MODE_SLAVE)
    {
        for(i=0;i<=8;i++)                                   
        {
            while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);			
            USART_SendData(UART4,mode_slave[i]);		
        } 
    } 
}
/*
	HCHO��������ʼ��
	bsp_HCHO_uart4.h �ļ��궨��ѡ���ϴ�ģʽ
*/
void HCHO_Config(void)
{
	UART4_Config();
	HCHO_Set_Mode();
}

/*
	��ȩ���������ݴ���
	return :��ȩ��ֵ
*/
uint16_t HCHO_Process(void)
{
	int i;
	uint16_t HCHO_Data;
	uint8_t mode_master[9]  = {0xff,0x01,0x78,0x41,0x00,0x00,0x00,0x00,0x46};
	if(HCHO_MODE == HCHO_MODE_SLAVE)
	{
		if(g_nHCHOData[0] == 0xff)
		{
			if(g_nHCHOData[1] == 0x17)
			{
				if(g_nHCHOData[2] == 0x04)
				{
					HCHO_Data = g_nHCHOData[4]<<8;
					HCHO_Data |= g_nHCHOData[5];
					
					if( FucCheckSum(g_nHCHOData,9) == g_nHCHOData[8])
					{
						memset(g_nHCHOData,0,sizeof(g_nHCHOData));
						return HCHO_Data;
					}
				}	
			}
		}
	}
	else if(HCHO_MODE == HCHO_MODE_MASTER)
	{
		for(i=0;i<=8;i++)                                   
        {
            while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);			
            USART_SendData(UART4,mode_master[i]);		
        } 
		if(g_nHCHOData[0] == 0xff)
		{
			if(g_nHCHOData[1] == 0x17)
			{
				if(g_nHCHOData[2] == 0x04)
				{
					HCHO_Data = g_nHCHOData[4]<<8;
					HCHO_Data |= g_nHCHOData[5];
					
					if( FucCheckSum(g_nHCHOData,9) == g_nHCHOData[8])
					{
						memset(g_nHCHOData,0,sizeof(g_nHCHOData));
						return HCHO_Data;
					}
				}	
			}
		}
	}
	
}
/*
	��ȩ����������У��
*/
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


int i =0;

void UART4_IRQHandler(void)
{
	uint16_t ucTemp;
	if(USART_GetITStatus(DEBUG_UART4x,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = (uint8_t)USART_ReceiveData(DEBUG_UART4x);
        
		g_nHCHOData[i] = ucTemp;
		i++;
        
        
		if(i == 9)
		{
			i = 0;
			g_nRecvFlag = 1;
		}

		USART_SendData(DEBUG_UART4x,ucTemp);    
	}	 
}

