#ifndef __SD_SPI_H
#define __SD_SPI_H			

#include "stm32f10x.h"
#include "ff.h"			/* Declarations of FatFs API */

// SD类型定义
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	

// SD指令表
#define CMD0    0	  /* GO_IDLE_STATE */
#define CMD1    1	  /* SEND_OP_COND */
#define CMD8    8 	/* SEND_IF_COND */
#define CMD9	9	  /* SEND_CSD */
#define CMD10 	10	/* SEND_CID */
#define CMD12   12	/* STOP_TRANSMISSION */
#define CMD13	13	/* SEND_STATUS */
#define CMD16   16  /* 设置SectorSize 返回0x00*/
#define CMD17 	17	/* READ_SINGLE_BLOCK */
#define CMD18   18	/* READ_MULTIPLE_BLOCK */
#define CMD23   23  /*擦除N个block*/
#define CMD24   24	/* WRITE_BLOCK */
#define CMD25 	25	/* WRITE_MULTIPLE_BLOCK */
#define CMD41 	41	/* SEND_OP_COND (ACMD) */
#define CMD55 	55	/* APP_CMD */
#define CMD58   58  /*读取OCR寄存器*/

#define  SD_CS  PBout(12)

#define CSD		9
#define CID		10

extern u8 SD_Type;
extern u32 SD_Sectornum;

void SD_SPI_Init(void);
void SD_CS_Disable(void);
u8 SD_CS_Enable(void);
u8 Get_Response(u8 Response);

u8  SD_SendCmd(u8 cmd,u32 arg,u8 crc);
u8  SD_GetCIDCSD(u8 cid_csd,u8 * buffer);
u8  SD_Reset(void);
u8  SD_Init(void);
u8 SD_RecvDate(u8 * buffer,u16 len);

u8  SDReadSector(u32 addr,u8 * buffer,u8 cnt);
//u8  SDReadMultiSector(u32 addr,u8 sector_num,u8 * buffer);
u8  SDWriteSector(u32 addr,u8 * buffer,u8 cnt);
//u8  SDWriteMultiSector(u32 addr,u8 sector_num,u8 * buffer);
u32 SD_GetSectorCount(void);


int8_t WriteToSD_Env(const void *buffer, UINT cnt);
int8_t ReadFromSD_Env(char *buffer1, UINT cnt);
int8_t WriteToSD_password(const void *buffer, UINT cnt);
int8_t ReadFromSD_password(char *buffer3, UINT cnt) ;
int8_t WriteToSD_address(const void *buffer4, UINT cnt) ;
int8_t ReadFromSD_address(char *buffer5, UINT cnt);

#endif


