#include "sd_spi.h" 
#include "ff.h"			/* Declarations of FatFs API */
#include "RTC.h"
#include "spi.h"
#include "comm.h"
#include "calendar.h"

u8 SD_Type = 0;
u32 SD_Sectornum = 0;

/**
  * @brief		Configure the IO of SPI
  * @param		None
  * @retval		None
  */
void SD_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );//PORTB时钟使能 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // PB12 推挽 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);
  
//    SD_CS = 1;				//SPI FLASH不选中
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
	SPI2_Init();		   	//初始化SPI

	SPI2_SetSpeed(SPI_BaudRatePrescaler_256);
}

/**
  * @brief		Configure the IO of SPI
  * @param		None
  * @retval		0 success, other false
  */
u8 SD_WaitReady(void)
{
	u32 t=0;
	do
	{
		if(SPI2_ReadWriteByte(0xff)==0XFF)
			return 0;//OK
		t++;		  	
	}while(t++<0XFFF);//等待
	return 1;
}

/**
  * @brief		Disable the CS of SPI
  * @param		None
  * @retval		0 success, other unsuccess
  */
void SD_CS_Disable(void)
{
//    SD_CS = 1;
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
	SPI2_ReadWriteByte(0xFF);//提供8个时钟
}

/**
  * @brief		Enable the CS of SPI
  * @param		None
  * @retval		0 success, other unsuccess
  */
u8 SD_CS_Enable(void)
{
//  SD_CS = 0;
  GPIO_ResetBits(GPIOB, GPIO_Pin_12);
  if(SD_WaitReady()==0) 
  {
      return 0; 
  }
  SD_CS_Disable();
//  printf("enable false\n");
  return 1;
}

/**
  * @brief		等待SD卡返回指定支字符
  * @param		Response   指定的字符
  * @retval	  0 返回成功  1返回错误
  */
u8 Get_Response(u8 Response)
{
  u16 count = 0xffff;
  while((SPI2_ReadWriteByte(0xff) != Response) && count) 
	  count--;
  if(count==0) 
	  return 1;
  else return 0;
}

/**
  * @brief		向SD卡发送一个命令
  * @param		cmd   命令
  * @param   	arg   命令参数
  * @param    crc   crc校检值      
  * @param   
  * @retval	  SD卡返回的响应
  */
u8 SD_SendCmd(u8 cmd,u32 arg,u8 crc)
{
	u8 r1,time = 0;

	SD_CS_Disable();
	if(SD_CS_Enable()) 
		return 0xFF;

	//send the command,arguments and CRC
	SPI2_ReadWriteByte((cmd & 0x3f) | 0x40);
	SPI2_ReadWriteByte(arg >> 24);
	SPI2_ReadWriteByte(arg >> 16);
	SPI2_ReadWriteByte(arg >> 8);
	SPI2_ReadWriteByte(arg);
	SPI2_ReadWriteByte(crc);
  
	if(cmd == CMD12) 
	{
		SPI2_ReadWriteByte(0xFF);//Skip a stuff byte when stop reading
		printf("cmd = CMD12\n");
	}
    time = 0xf0;
    do
    {
        r1 = SPI2_ReadWriteByte(0xFF);
    } while(r1 == 0xFF && time--);  /*while(r1&0X80)*/ 
 
//  SD_CS_Disable();
  
	return r1;
}

/**
  * @brief		获取SD卡的CID或CSD
  * @param		cid_csd ：CSD or CID     
  * @retval	  buffer  ：SD卡返回的信息
  */
u8 SD_GetCIDCSD(u8 cid_csd,u8 * buffer)
{
	u8 r1;
	u16 i,time = 0;
 
	//set CS low
	SD_CS_Enable();
 
	//send CMD10 for CID read or CMD9 for CSD
	do{
		if(cid_csd == CID)
			r1 = SD_SendCmd(CMD10,0,0xFF);
		else if(cid_csd == CSD)
			r1 = SD_SendCmd(CMD9,0,0xFF);
		time ++;
		//if time out,set CS high and return r1
		if(time > 254)
		{
			//set CS high and send 8 clocks
			SD_CS_Disable();
			return r1;
		}
	}while(r1 != 0x00);
	time = 0;
 
	//continually read till get 0xfe
	do{
		r1 = SPI2_ReadWriteByte(0xFF);
		time ++;
		//if time out,set CS high and return r1
		if(time > 30000)
		{
			//set CS high and send 8 clocks
			SD_CS_Disable();
			return r1;
		}
	}while(r1 != 0xfe);
 
	//read 512 Bits of data
	for(i = 0;i < 16;i ++)
	{
		*buffer ++ = SPI2_ReadWriteByte(0xFF);
	}
 
	//read two bits of CRC
	SPI2_ReadWriteByte(0xFF);
	SPI2_ReadWriteByte(0xFF);
 
	//set CS high and send 8 clocks
	SD_CS_Disable();
 
	return 0;
}

/**
  * @brief		reset SD card
  * @param		None
  * @retval	  0 success,other unsuccess
  */
u8 SD_Reset(void)
{
	u8 i,r1,time = 0;
 
//	SD_CS_Disable();
	for(i = 0;i < 16;i ++)
	{
		SPI2_ReadWriteByte(0xFF);
	}
    
	SD_CS_Enable();
    
    time = 20;
	do
    {
		r1 = SD_SendCmd(CMD0,0,0x95);//进入idle状态
        
	}while((r1 != 0x01) && time--);
 
//	SD_CS_Disable();
	return r1;
}

/**
  * @brief		初始化SD卡
  * @param		None
  * @retval	  SD卡返回的响应
  */
u8 SD_Init(void)
{
    u8 r1 = 0;
    u16 time = 0;
    u8 buf[4];  
    u16 i;
  
    r1 = SD_Reset();

    SD_CS_Enable();
    
    SD_Type = 0;
    
    if(r1 == 0x01)
    {
        if(SD_SendCmd(CMD8,0x1AA,0x87) == 1)  //SD V2.0
        {
            for(i=0;i<4;i++)
                buf[i] = SPI2_ReadWriteByte(0XFF);	//Get trailing return value of R7 res
            
            if(buf[2]==0X01 && buf[3]==0XAA)//卡是否支持2.7~3.6V
            {
                time = 0xfffe;
                do
                {
                    SD_SendCmd(CMD55,0,0X01);	
                    r1=SD_SendCmd(CMD41,0x40000000,0X01);//??CMD41
                }while(r1 && time--);

                if(time && SD_SendCmd(CMD58,0,0X01)==0)//鉴别2.0
                {
                    for(i=0;i<4;i++)
                    {
                        buf[i]=SPI2_ReadWriteByte(0XFF);//OCR
                    }
                    if(buf[0]&0x40)
                        SD_Type=SD_TYPE_V2HC;    //2.0高容量 SDHC 扇区寻址
                    else
                        SD_Type=SD_TYPE_V2;  //2.0标准  字节寻址
                }
            }
        }
    else  //SD V1.x or MMC V3
    {
        SD_SendCmd(CMD55,0,0X01);	
        r1=SD_SendCmd(CMD41,0x40000000,0X01);//CMD41

        if(r1 <= 1)
        {
            SD_Type=SD_TYPE_V1;  //1.0标准卡

            time=0XFFFE;
            do 
            {
                SD_SendCmd(CMD55,0,0X01);	
                r1=SD_SendCmd(CMD41,0,0X01);
            }while(r1 && time--);
        }
        else
        {
            SD_Type=SD_TYPE_MMC;//MMC V3
            time=0XFFFE;
            do 
            {
                r1=SD_SendCmd(CMD1,0,0X01);
            }while(r1 && time--);  
        }
        if(time == 0 || SD_SendCmd(CMD16,512,0x01) != 0) SD_Type = SD_TYPE_ERR;
    }
  }
  //set CS high and send 8 clocks
	SD_CS_Disable();
  
    SD_Sectornum = SD_GetSectorCount();
  
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);
 
    if(SD_Type) 
        return 0;
    else if(r1) 
        return r1;
  
    return 0xaa;
}
 
u8 SD_RecvDate(u8 * buffer,u16 len)
{
	if(Get_Response(0xfe)) return 1;
	while(len--)
	{
		*buffer = SPI2_ReadWriteByte(0xFF);
		buffer++;
	}
	//read two bits of CRC
	SPI2_ReadWriteByte(0xFF);
	SPI2_ReadWriteByte(0xFF);
	return 0;
}


u8 SD_Sendblock(u8 * buffer,u16 arg)
{
	u16 t;
	if(SD_WaitReady()) return 1;
	SPI2_ReadWriteByte(arg);
	if(arg != 0xfd)
	{
		for(t = 0;t < 512;t++)
		{
			SPI2_ReadWriteByte(buffer[t]);
		}  
		//write 2 bytes of CRC
		SPI2_ReadWriteByte(0xFF);
		SPI2_ReadWriteByte(0xFF);

		t = SPI2_ReadWriteByte(0xFF);
		if((t & 0x1f)!= 0x05) return 2;
	}
	return 0;
}


/**
  * @brief		read data from sector
  * @param		addr    :扇区号
  * @param		buffer  :数据数组
  * @param		cnt     :读取的数据字节数
  * @retval	  0 success,other unsuccess
  */
u8 SDReadSector(u32 addr,u8 * buffer,u8 cnt)
{
	u8 r1;

	if(SD_Type != SD_TYPE_V2HC)
	{
		addr *= 512; //SDHC sector----SDSC  byte
	}
	SD_CS_Enable();

	if(cnt ==1)
	{
		//send CMD17 for single block read
		r1 = SD_SendCmd(CMD17,addr,0x01);
		if(r1 != 0x00)
		{
			SD_CS_Disable();
			return r1;
		}
		r1 = SD_RecvDate(buffer,512);
	}
	else
	{
		r1 = SD_SendCmd(CMD18,addr,0x01);
		if(r1 != 0x00)
		{
			SD_CS_Disable();
			return r1;
		}
		do
		{
			r1 = SD_RecvDate(buffer,512);
			buffer += 512;

		}while(--cnt && r1 == 0);
		SD_SendCmd(CMD12,0,0xFF);
	}

	//set CS high and send 8 clocks
	SD_CS_Disable();
	return r1;
}

/**
  * @brief		Write data to sector
  * @param		addr    :扇区号
  * @param		buffer  :数据数组
  * @param		cnt     :读取的数据字节数
  * @retval	  0 success,other unsuccess
  */
u8 SDWriteSector(u32 addr,u8 * buffer,u8 cnt)
{
	u16 i = 0;
	u8 r1;
	if(SD_Type != SD_TYPE_V2HC)
	{
		addr *= 512;
	}
	SD_CS_Enable();

	if(cnt == 1)
	{
		r1 = SD_SendCmd(CMD24,addr,0x01);
		if(r1==0)
		{
			for(i = 0;i < 5;i ++)
			{
				SPI2_ReadWriteByte(0xFF);
			}
			SD_Sendblock(buffer,0xfe);
		}
	}
	else 
	{
		if(SD_Type != SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0x01);
			SD_SendCmd(CMD23,cnt,0x01);
		}
		r1 = SD_SendCmd(CMD25,addr,0x01);
		if(r1 == 0)
		{
			do
			{
				r1 = SD_Sendblock(buffer,0xfc);
				buffer+=512;
			}while(--cnt && r1 ==0);
			r1 = SD_Sendblock(0,0xfd);
		}
	}
	SD_WaitReady();
	//set CS high and send 8 clocks
	SD_CS_Disable();

	return r1;
}


/**
  * @brief		Get the Sector num of SD
  * @param		None
  * @retval	  the Sector num of SD
  */
u32 SD_GetSectorCount(void)
{
	u8 csd[16];
	u32 count=0;
	u8 n;
	u16 csize;
	if(SD_GetCIDCSD(CSD,csd)!=0) return 0;
	if((csd[0] & 0xc0) == 0x40)
	{
		csize = csd[9] + ((u16)csd[8]<<8) + ((uint32_t)(csd[7] & 0x3f) << 16) + 1;
	}
	else
	{
		n = (csd[5] & 0x0F) + ((csd[10] & 0x80) >> 7) + ((csd[9] & 0x03) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 0x03) << 10) + 1;
		count = (uint32_t)csize << (n - 10);
		csize= count/512;
	}
	return csize;
}



extern tm timer;
int8_t WriteToSD_Env(const void *buffer, UINT cnt)            //单独轮抄环境表
{
	FRESULT     res;
	FIL         parafile;     //文件对象
    FILINFO     file_info;
	DIR dp;
	UINT br;     //已存储字节数变量的指针
	int32_t num; //转换后的电表表号

	char folder[50] = {0};     //文件夹路径名称
	char file[50] = {0};        //文件夹路径名称
	uint32_t offset = 0;        //偏移地址

    int8_t Rxbuf[20];
    
	rtc_get();
	
	sprintf(folder,"0:\\%0.4d-%0.2d",timer.w_year,timer.w_month);  //获取文件夹路径
    printf("folder %s\n", folder);
    
    res = f_mkdir((const char*)folder) ;
    
	res = f_opendir(&dp,(const char*)folder);
    
    if( res == 0)
	{
        printf("open dir success\n");	
	}
    else
    {
        printf("open dir faild\n");
        return 0;
    }
    
    
    
	if((res==FR_OK) || (res==FR_EXIST)) 
	{
		sprintf(file,"0:\\%0.4d-%0.2d\\%0.2d-%0.2d.txt",timer.w_year,timer.w_month,timer.w_month,timer.w_date);     //输入文件路径
//			sprintf(file,"0:\\%0.4d-%0.2d\\%0.2d-%0.2d.txt",timer.w_year,timer.w_month,timer.hour,timer.min);     //输入文件路径
        /* 获取文件信息 */
        f_stat(file, &file_info);
        printf("file name : %s\n", file);
        
		res = f_open(&parafile, file, FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_WRITE);     //此处注意文件打开模式
		if(res) 
        {
            printf("open file faild\n");
            return res;
        }
        else
            printf("open file success\n");
        
        res = f_lseek(&parafile, file_info.fsize);       //偏移出文件空间    移动到文件末尾处
//        res = f_lseek(&parafile, parafile.fsize);       //偏移出文件空间    移动到文件末尾处
		if ( f_write (&parafile, buffer, 47, &br) == 0)  ////形参的数组为写入的内容指针
		{
                printf("write file success\n");		
		}

        else
        {
            printf("write file failed\n");
            return 0;
        }
        
		f_close(&parafile);
	}
	
}
extern int f,k,times;
int8_t ReadFromSD_Env(char *buffer1, UINT cnt)            //读SD卡数据
{
	FRESULT res;
	FIL parafile;     //文件对象
	DIR dp;
	UINT br;     //已存储字节数变量的指针
	int32_t num; //转换后的电表表号
  
	
	char folder[50] = {0};     //文件夹路径名称
	char file[50] = {0};     //文件夹路径名称
	uint32_t offset = 0;     //偏移地址

	int8_t Rxbuf[20];

//	sprintf(folder,"0:\\%0.4d-%0.2d",timer.w_year,timer.w_month);  //获取文件夹路径
	f_mkdir((const char*)folder);
	res=f_opendir(&dp,(const char*)folder);
	if((res==FR_OK) || (res==FR_EXIST)) 
	{
//		sprintf(file,"0:\\%0.4d-%0.2d\\%0.2d-%0.2d.txt",timer.w_year,timer.w_month,f,k);     //输入文件路径
		res = f_open(&parafile, file, FA_READ|FA_OPEN_ALWAYS);     //此处注意文件打开模式
		if(res) return res;
        f_lseek(&parafile,44*times);                 //偏移出足够的空间    	
		f_read (&parafile, buffer1, 44, &br);  ////形参的数组为写入的内容指针		
		f_close(&parafile);
	}
}




int8_t WriteToSD_password(const void *buffer2, UINT cnt)            //写SD卡密码数据
{
	FRESULT res;
	FIL parafile;     //文件对象
	DIR dp;
	UINT br;     //已存储字节数变量的指针
	int32_t num; //转换后的电表表号

	char folder[50] = {0};     //文件夹路径名称
	char file[50] = {0};     //文件夹路径名称
	uint32_t offset = 0;     //偏移地址

//	rtc_get();


	f_mkdir((const char*)folder);                  //建立文件根目录

	res=f_opendir(&dp,(const char*)folder);         //打开文件根目录

	if((res==FR_OK) || (res==FR_EXIST)) 
	{
//		sprintf(file,"0:\\%0.4d-%0.2d\\%0.2d-%0.2d.txt",2017,2,2,10);     //输入文件路径
		res = f_open(&parafile, "password.txt", FA_CREATE_ALWAYS|FA_OPEN_ALWAYS|FA_WRITE);     //此处注意文件打开模式
		if(res) return res;
//	  res = f_lseek(&parafile, parafile.fsize);       //偏移出文件空间
		f_write (&parafile, buffer2, 4, &br);  ////形参的数组为写入的内容指针

		f_close(&parafile);
        
        printf("write password success\n");
	}
    else
    {
        printf("no this file name");
        res = f_open(&parafile, "password.txt", FA_CREATE_ALWAYS|FA_OPEN_ALWAYS|FA_WRITE);     //此处注意文件打开模式
		if(res) return res;
//	  res = f_lseek(&parafile, parafile.fsize);       //偏移出文件空间
		f_write (&parafile, buffer2, 4, &br);  ////形参的数组为写入的内容指针

		f_close(&parafile);
    }
}


int8_t ReadFromSD_password(char *buffer3, UINT cnt)            //读SD卡密码数据
{
	FRESULT res;
	FIL parafile;     //文件对象
	DIR dp;
	UINT br;     //已存储字节数变量的指针
//	int32_t num; //转换后的电表表号
  
	
	char folder[50] = {0};     //文件夹路径名称
	char file[50] = {0};     //文件夹路径名称
	uint32_t offset = 0;     //偏移地址

//  int8_t Rxbuf[20];

	f_mkdir((const char*)folder);                    //建立文件根目录

	res=f_opendir(&dp,(const char*)folder);          //打开文件根目录

	if((res==FR_OK) || (res==FR_EXIST)) 
	{
		res = f_open(&parafile, "password.txt",FA_READ|FA_OPEN_ALWAYS);      //此处注意文件打开模式
		if(res) return res;
    res = f_lseek(&parafile,0);       //偏移出文件空间
		f_read (&parafile, buffer3, 4, &br);  ////形参的数组为写入的内容指针

		f_close(&parafile);
	}

}




int8_t WriteToSD_address(const void *buffer4, UINT cnt)            //写SD卡地址数据
{
	FRESULT res;
	FIL parafile;     //文件对象
	DIR dp;
	UINT br;     //已存储字节数变量的指针
	int32_t num; //转换后的电表表号

	char folder[50] = {0};     //文件夹路径名称
	char file[50] = {0};     //文件夹路径名称
	uint32_t offset = 0;     //偏移地址

//	rtc_get();


	f_mkdir((const char*)folder);                        //建立文件根目录

	res=f_opendir(&dp,(const char*)folder);              //打开文件根目录

	if((res==FR_OK) || (res==FR_EXIST)) 
	{
//		sprintf(file,"0:\\%0.4d-%0.2d\\%0.2d-%0.2d.txt",2017,2,2,10);     //输入文件路径
		res = f_open(&parafile, "address.txt", FA_OPEN_ALWAYS|FA_WRITE);     //此处注意文件打开模式
		if(res) return res;
//	  res = f_lseek(&parafile, parafile.fsize);       //偏移出文件空间
		f_write (&parafile, buffer4, 12, &br);  ////形参的数组为写入的内容指针

		f_close(&parafile);
        
        printf("write address success\n");
	}
    else
        printf("no this file name");
	 
	
}



int8_t ReadFromSD_address(char *buffer5, UINT cnt)            //读SD卡地址数据
{
	FRESULT res;
	FIL parafile;     //文件对象
	DIR dp;
	UINT br;     //已存储字节数变量的指针
//	int32_t num; //转换后的电表表号
  
	
	char folder[50] = {0};     //文件夹路径名称
	char file[50] = {0};     //文件夹路径名称
	uint32_t offset = 0;     //偏移地址

//  int8_t Rxbuf[20];

	f_mkdir((const char*)folder);                           //建立文件根目录

	res=f_opendir(&dp,(const char*)folder);                  //打开文件根目录

	if((res==FR_OK) || (res==FR_EXIST)) 
	{
		res = f_open(&parafile, "address.txt",FA_READ|FA_OPEN_ALWAYS);      //此处注意文件打开模式
		if(res) return res;
		res = f_lseek(&parafile,0);       //偏移出文件空间
		f_read (&parafile, buffer5, 12, &br);  ////形参的数组为写入的内容指针

		f_close(&parafile);
	}
}