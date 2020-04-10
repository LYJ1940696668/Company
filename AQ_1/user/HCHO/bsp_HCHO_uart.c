#include "bsp_HCHO_uart.h"
#include <math.h>
uint8_t g_nHCHOData[9]; //串口中断传感器数组
/*
	g_nRecvFlag ：
	甲醛传感器1秒发送一次
	接收到一组数据：置1
	读完数据需要手动置0
*/
uint8_t g_nRecvFlag;
 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置UART4为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_UART4_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  UART4 GPIO 配置,工作参数配置
  * @param  无
  * @retval 无
  */
static void UART4_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef UART4_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_UART4_GPIO_APBxClkCmd(DEBUG_UART4_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_UART4_APBxClkCmd(DEBUG_UART4_CLK, ENABLE);

	// 将UART4 Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_UART4_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_UART4_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将UART4 Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_UART4_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_UART4_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	UART4_InitStructure.USART_BaudRate = DEBUG_UART4_BAUDRATE;
	// 配置 针数据字长
	UART4_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	UART4_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	UART4_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	UART4_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	UART4_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_UART4x, &UART4_InitStructure);
	
	// 串口中断优先级配置
	NVIC_Configuration();
	
	// 使能串口接收中断
	USART_ITConfig(DEBUG_UART4x, USART_IT_RXNE, ENABLE);	
	
	// 使能串口
	USART_Cmd(DEBUG_UART4x, ENABLE);		

  // 清除发送完成标志
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
	HCHO传感器初始化
	bsp_HCHO_uart4.h 文件宏定义选择上传模式
*/
void HCHO_Config(void)
{
	UART4_Config();
	HCHO_Set_Mode();
}

/*
	甲醛传感器数据处理
	return :甲醛数值
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
	甲醛传感器数据校验
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

