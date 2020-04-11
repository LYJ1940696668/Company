#include "HostProcess.h"
#include "stm32f10x.h"
#include "calendar.h"

#include "bsp_iaq.h"
#include "usart1.h"

volatile FLASH_Status FLASHStatus = FLASH_BUSY;

uint8_t g_nDataLen = 0;//���յ������ݳ���version + func + data
int g_nDataProComFlag = 0;//������һ֡���ݱ�־λ data process complete flag
int g_cFuncCode = 0;//�洢���������е�funcλ
uint8_t g_cDataBuffer[20];//�������ݴ洢����
uint8_t g_cVersion = 0;//�洢����������versionλ
static int s_iHeadFlag = 0;//���հ�ͷ��־λ
static int s_iLenFlag = 0;//���������ݳ��ȱ�־λ����LEN��ʼ���㣩
static int s_ichlen = 0;//���յ��������ݳ��ȱ�־λ���Ӱ�ͷ��ʼ���㣩
static int i = 0;//ѭ������
//�洢��λ���������飬�����ж�funcλ�Ƿ���ȷ
static uint8_t s_cFuncCodeBuff[15] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,
									  0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15};
/*send to host*/
//�ش�OK���飬����funcλ��versionλ������Download_Host()�����и���
uint8_t g_cResponse[10] = {0x53,0x53,0x07,0x10,0x81,0x4f,0x4b,0x2b,0x45,0x44};
uint16_t g_iTemp = 0;//�¶�
uint16_t g_iHumi = 0;//ʪ��
uint16_t g_iPM2 = 0;//PM2.5
uint16_t g_iPM1 = 0;//PM10;
uint16_t g_iCO2 = 0;//������̼
uint16_t g_iHCHO = 0;//����
uint16_t g_iVOC = 0;//VOC
uint8_t g_cSenseBuff[14] = {0x01,0x48,0x01,0x63,0x01,0xf4,0x02,//�������ϴ�����
							0x2b,0x07,0xd0,0x05,0xdc,0x05,0xdc};
/*receive from host send to host*/
uint8_t g_cDev_Module[10];//�豸��
uint8_t g_cDev_Module_Flash[20]; //��flash��� 0x0807 f800 --- 0x0807 f814
uint8_t g_cDev_ID[21];//���к�
uint8_t g_cDev_ID_Flash[42];//��flash��� 0x0807 f830 --- 0x0807 f85a
uint8_t g_cDev_Func[2];//�豸����
uint8_t g_cDev_Area[1];//ʱ��
uint8_t g_cDev_Time[7];//ʱ��
uint8_t g_cDev_ArTi[8];//Area+Time
uint8_t g_cDev_Carr_Addr[12];//�ز���ַ
uint8_t g_cDev_Carr_Addr_Flash[24];//��flash��� 0x0807 fa00 --- 0x0807 fa18
uint8_t g_cDev_485_Addr[2];//485��ַ
uint8_t g_cDev_485_Rate[4];//485������
uint16_t Year = 0;
/*static function*/

static bool DataContrast(unsigned char ch,unsigned char *pbuff,int nlen);

static void Download_Host(uint8_t func,
						uint8_t version,
						uint8_t *Dev_buff,
						int len);
static void Updata_Host(uint8_t func,
						uint8_t version,
						uint8_t cmd_len,
						uint8_t *pDev_buff,
						int data_len);
uint8_t DataLen;//���յ��ĳ���version + func + data
/**
  * @brief	��ʼ��Э�����õ��ı���������
  * @param	
  * @retval	
  */
static void InitRecvState(void)
{
	g_nDataLen = 0;
	memset(g_cDataBuffer,0,sizeof(g_cDataBuffer));
}
/**
  * @brief	����Э��
  * @param	ch���ڽ��յ����ַ�
  * @retval	��
  */
void Recv_Data(uint8_t ch)
{
	if((ch == 'S') && (s_iLenFlag == 0))
	{
		s_iHeadFlag ++;
	}
	if((s_iHeadFlag == 2) && (s_ichlen == 2))
	{
		DataLen = ch;
		s_iLenFlag = 1;
		s_iHeadFlag = 0;
	}
	
	if((s_iLenFlag == 1) && (s_ichlen > 2))
	{
		g_cDataBuffer[i] = ch;
		//code
		if(DataContrast(g_cDataBuffer[1],s_cFuncCodeBuff,sizeof(s_cFuncCodeBuff)))
		{
			g_cFuncCode = g_cDataBuffer[1];		
			g_cVersion = g_cDataBuffer[0];
		}
		i ++;
		if(i >= (DataLen))
		{
			i = 0;
			DataLen = 0;
			g_nDataProComFlag = 1;
		}
		
	}
	s_ichlen ++;
	if((s_ichlen > (DataLen+2)) && (s_iLenFlag == 1))
	{
		s_iLenFlag = 0;
		s_ichlen = 0;
	}
}

/**
  * @brief	�Ա�pbuff���Ƿ���ch����
  * @param	ch��Ҫ�Աȵ��ַ�
			pbuff��Ҫ�Աȵ��ַ���
			nlen��pbuff����
  * @retval	true:pbuff�д�����ch��ͬ������
			false��������
  */
static bool DataContrast(unsigned char ch,unsigned char *pbuff,int nlen)
{
	int i;
	for(i = 0;i < nlen;i ++)
	{
		if(ch == pbuff[i])
		{
			return true;
		}
	}
	return false;
}

/**
  * @brief	У�������
  * @param	version �汾��
			func �������
			pbuff��Э����data��������
			nlen��pbuff�ĳ���
  * @retval	return:����õ���У��ֵ
  */
static uint8_t SumFSC(int version,int func,uint8_t *pbuff,int nlen)
{
	int i;
	uint8_t c_Temp = 0;
	for(i = 0;i < nlen;i ++)
	{
		c_Temp += pbuff[i];
	}
	return (c_Temp+version+func);
}

extern tm timer;
/**
  * @brief	��λ������ѡ��
  * @param	
  * @retval	return:��
  */
extern float pm2_5,CO2_ppm,Humidity,Temperature;
int g_iFlashCount = 0;
void Host_Code(void)
{
	st_iaq_core tvoc;
	int i;
	switch (g_cFuncCode)
	{
		case SET_DEVICE_MODEL:
		{
			Download_Host(g_cFuncCode+0x80,g_cVersion,g_cDev_Module,sizeof(g_cDev_Module));
			WirteFlashData(0,g_cDev_Module,sizeof(g_cDev_Module));
			InitRecvState();
			
			break;
		}
		case READ_DEVICE_MODEL:
		{
			g_iFlashCount = ReadFlashNBtye(0,g_cDev_Module_Flash,
									sizeof(g_cDev_Module_Flash));
			for(i = 0;i < g_iFlashCount;i ++)
			{
				g_cDev_Module_Flash[i] = g_cDev_Module_Flash[i*2];
			}
//			memset(g_cDev_Module_Flash,0,sizeof(g_cDev_Module_Flash));
			
			Updata_Host(g_cFuncCode+0x80,
						g_cVersion,
						0x0f,
						g_cDev_Module_Flash,
						sizeof(g_cDev_Module));			

			break;
		}
		case SET_DEVICE_ID:
		{
			Download_Host(g_cFuncCode+0x80,g_cVersion,g_cDev_ID,sizeof(g_cDev_ID));
			
			WirteFlashData(0x30,g_cDev_ID,sizeof(g_cDev_ID));
			
			InitRecvState();
			break;
		}
		case READ_DEVICE_ID:
		{
			g_iFlashCount = ReadFlashNBtye(0x30,g_cDev_ID_Flash,
									sizeof(g_cDev_ID_Flash));
			for(i = 0;i < g_iFlashCount;i ++)
			{
				g_cDev_ID_Flash[i] = g_cDev_ID_Flash[i*2];
			}
//			memset(g_cDev_ID_Flash,0,sizeof(g_cDev_ID_Flash));
			
			Updata_Host(g_cFuncCode+0x80,g_cVersion,0x1a,g_cDev_ID_Flash,sizeof(g_cDev_ID));	
			InitRecvState();
			break;
		}
		case SET_DEVICE_FUNC:
		{
			Download_Host(g_cFuncCode+0x80,g_cVersion,g_cDev_Func,sizeof(g_cDev_Func));
			InitRecvState();
			break;
		}
		case READ_DEVICE_FUNC:
		{
			Updata_Host(g_cFuncCode+0x80,g_cVersion,0x07,g_cDev_Func,sizeof(g_cDev_Func));
			InitRecvState();
			break;
		}
		case SET_DEVICE_TIME:
		{
			Download_Host(g_cFuncCode+0x80,g_cVersion,g_cDev_ArTi,sizeof(g_cDev_ArTi));
			Year = g_cDev_ArTi[1] << 8;
			Year += g_cDev_ArTi[2];
			
			rtc_set(Year,
			g_cDev_ArTi[3],//mon
			g_cDev_ArTi[4],//day
			g_cDev_ArTi[5],//hour
			g_cDev_ArTi[6],//min
			g_cDev_ArTi[7]);//sec
			
			InitRecvState();
			break;
		}
		case READ_DEVICE_TIME:
		{
			g_cDev_ArTi[1] = (uint8_t)(timer.w_year >> 8); 
			g_cDev_ArTi[2] = (uint8_t)timer.w_year;
			g_cDev_ArTi[3] = timer.w_month;
			g_cDev_ArTi[4] = timer.w_date;
			g_cDev_ArTi[5] = timer.hour;
			g_cDev_ArTi[6] = timer.min;
			g_cDev_ArTi[7] = timer.sec;
			Updata_Host(g_cFuncCode+0x80,
						g_cVersion,
						0x0d,
						g_cDev_ArTi,
						sizeof(g_cDev_ArTi));
			InitRecvState();
			break;
		}
		case READ_DEVICE_SENSE:
		{/*
			��Ҫ�Ѵ��������ݼӵ�g_cSenseBuff������
			0,1λ:Temp; 2,3λ:Humi; 4,5λ:PM2.5; 6,7λ:PM10
			8,9λ:CO2; 10,11λ:HCHO; 12,13λ:VOC;
			pm2_5,CO2_ppm,Humidity,Temperature;
		*/
			/*δ����*/
			g_cSenseBuff[0] = (uint8_t)((Temperature*10) / 256);
			g_cSenseBuff[1] = (uint8_t)(Temperature*10);
			g_cSenseBuff[2] = (uint8_t)((Humidity*10) / 256);
			g_cSenseBuff[3] = (uint8_t)(Humidity*10);
			g_cSenseBuff[4] = (uint8_t)(PM_value.PM2_5 / 256);
			g_cSenseBuff[5] = (uint8_t)PM_value.PM2_5;
			g_cSenseBuff[6] = (uint8_t)((PM_value.PM10) / 256);
			g_cSenseBuff[7] = (uint8_t)PM_value.PM10;
			/*�Ѳ���*/
//			IAQ_CORE_Read(&tvoc);
			g_cSenseBuff[8] = (uint8_t)(CO2_ppm / 256);
			g_cSenseBuff[9] = (uint8_t)CO2_ppm;
//			g_cSenseBuff[10] = (uint8_t)(HCHO_Process() >> 8);
//			g_cSenseBuff[11] = (uint8_t)HCHO_Process();
//			g_cSenseBuff[12] = (uint8_t)(tvoc.tvoc >> 8);
//			g_cSenseBuff[13] = (uint8_t)tvoc.tvoc;
			Updata_Host(g_cFuncCode+0x80,
						g_cVersion,
						0x13,
						g_cSenseBuff,
						sizeof(g_cSenseBuff));
			InitRecvState();
			break;
		}
		case SET_DEVICE_CARR_ADDR:
		{
			Download_Host(g_cFuncCode+0x80,g_cVersion,
						  g_cDev_Carr_Addr,sizeof(g_cDev_Carr_Addr));
			WriteToSD_address(g_cDev_Carr_Addr,16);
			InitRecvState();
			break;
		}
		case READ_DEVICE_CARR_ADDR:
		{
			ReadFromSD_address(g_cDev_Carr_Addr,16);
			Updata_Host(g_cFuncCode+0x80,
						g_cVersion,
						0x11,
						g_cDev_Carr_Addr,
						sizeof(g_cDev_Carr_Addr));
			InitRecvState();
			break;
		}
		case SET_DEVICE_485_ADDR:
		{
			Download_Host(g_cFuncCode+0x80,g_cVersion,
						  g_cDev_485_Addr,sizeof(g_cDev_485_Addr));
			InitRecvState();
			break;
		}
		case READ_DEVICE_485_ADDR:
		{
			Updata_Host(g_cFuncCode+0x80,
						g_cVersion,
						0x07,
						g_cDev_485_Addr,
						sizeof(g_cDev_485_Addr));
			InitRecvState();
			break;
		}
		case SET_DEVICE_485_BAUDRATE:
		{
			Download_Host(g_cFuncCode+0x80,g_cVersion,
						  g_cDev_485_Rate,sizeof(g_cDev_485_Rate));
			InitRecvState();
			break;
		}
		case READ_DEVICE_485_RAUNRATE:
		{
			Updata_Host(g_cFuncCode+0x80,
						g_cVersion,
						0x09,
						g_cDev_485_Rate,
						sizeof(g_cDev_485_Rate));
			InitRecvState();
			break;
		}
	}
}
/**
  * @brief	����λ����������
  * @param	version �汾��
			func �������
			pDev_buff������λ�����յ�����������
			nlen��Dev_buff�ĳ���
  * @retval	return:��
  */
static void Download_Host(uint8_t func,
						  uint8_t version,
						  uint8_t *pDev_buff,
						  int len)
{
	int i;
	int count = 0;
	uint8_t OKBuff[2] = {0x4f,0x4b};
//	memcpy(pDev_buff,pDev_buff+2,len);
	for(i = 0;i < len;i ++)
	{
		pDev_buff[i] = g_cDataBuffer[i+2];
	}
	for(i = 0;i < len;i ++)
	{
		if(pDev_buff[i] == g_cDataBuffer[i+2])
			count ++;
	}
	g_cResponse[4] = func;
	g_cResponse[7] = SumFSC(version,func,OKBuff,2);
	
	if(count == len)
	{
		for(i = 0;i < 10;i ++)
		{
			USART_SendData(DEF_HOST_USART,g_cResponse[i]);
			delay_ms(10);
		}
		count = 0;
	}
}
/**
  * @brief	�����ݸ���λ��
  * @param	version �汾��
			func �������
			cmd_len:version + func + data
			pDev_buff������λ�����յ�����������
			data_len��������data�ĳ���
  * @retval	return:��
  */
static void Updata_Host(uint8_t func,
						uint8_t version,
						uint8_t cmd_len,
						uint8_t *pDev_buff,
						int data_len)
{
	int i;
	USART_SendData(DEF_HOST_USART,0x53);
	delay_ms(10);
	USART_SendData(DEF_HOST_USART,0x53);
	delay_ms(10);
	USART_SendData(DEF_HOST_USART,cmd_len);
	delay_ms(10);
	USART_SendData(DEF_HOST_USART,version);
	delay_ms(10);
	USART_SendData(DEF_HOST_USART,func);
	delay_ms(10);
	for(i = 0;i < data_len;i ++)
	{
		USART_SendData(DEF_HOST_USART,pDev_buff[i]);
		delay_ms(10);
	}
	USART_SendData(DEF_HOST_USART,SumFSC(version,func,pDev_buff,data_len));
	delay_ms(10);
	USART_SendData(DEF_HOST_USART,0x45);
	delay_ms(10);
	USART_SendData(DEF_HOST_USART,0x44);
	delay_ms(10);
}

void WirteFlashData(uint32_t WriteAddress,uint8_t data[],int num)
{
	int i = 0;;
	uint16_t temp = 0;
	FLASH_UnlockBank1(); //����flash
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
	
	FLASHStatus = 1;//���״̬��־λ
	FLASHStatus = FLASH_ErasePage(STARTADDR);//������ҳ
	if(FLASHStatus == FLASH_COMPLETE)
	{
		FLASHStatus = 1;
		for(i = 0;i < num;i ++)
		{
			temp = (uint16_t)data[i];
			FLASHStatus = FLASH_ProgramHalfWord(STARTADDR+WriteAddress+i*2, temp);
		}
		
	}
	FLASHStatus = 1;
	FLASH_LockBank1();
}

int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum)
{
    int DataNum = 0;
    
    ReadAddress = (uint32_t)STARTADDR + ReadAddress; 
    while(DataNum < ReadNum)   
    {        
        *(ReadBuf + DataNum) = *(__IO uint8_t*) ReadAddress++; 		
        DataNum++;     
    }
    return DataNum;    
}

