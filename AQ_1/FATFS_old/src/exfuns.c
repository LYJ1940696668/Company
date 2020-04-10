#include "string.h"
#include "exfuns.h"
//#include "fattester.h"	
//#include "malloc.h"
//#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEKÕ½½¢STM32¿ª·¢°åV3
//FATFS À©Õ¹´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2015/1/20
//°æ±¾£ºV1.0
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define FILE_MAX_TYPE_NUM		6	//×î¶àFILE_MAX_TYPE_NUM¸ö´óÀà
#define FILE_MAX_SUBT_NUM		13	//×î¶àFILE_MAX_SUBT_NUM¸öĞ¡Àà
#define SRAMIN	 0		//ÄÚ²¿ÄÚ´æ³Ø
#define SRAMEX   1		//Íâ²¿ÄÚ´æ³


 //ÎÄ¼şÀàĞÍÁĞ±í
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BINÎÄ¼ş
{"LRC"},			//LRCÎÄ¼ş
{"NES"},			//NESÎÄ¼ş
{"TXT","C","H"},	//ÎÄ±¾ÎÄ¼ş
{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","AAC","WMA","WAV","MID","FLAC"},//ÒôÀÖÎÄ¼ş
{"BMP","JPG","JPEG","GIF"},//Í¼Æ¬ÎÄ¼ş 
};
///////////////////////////////¹«¹²ÎÄ¼şÇø,Ê¹ÓÃmallocµÄÊ±ºò////////////////////////////////////////////
FATFS *fs[_VOLUMES];//Âß¼­´ÅÅÌ¹¤×÷Çø.	 
FIL *file;	  		//ÎÄ¼ş1
FIL *ftemp;	  		//ÎÄ¼ş2.
UINT br,bw;			//¶ÁĞ´±äÁ¿
FILINFO fileinfo;	//ÎÄ¼şĞÅÏ¢
DIR dir;  			//Ä¿Â¼

u8 *fatbuf;			//SD¿¨Êı¾İ»º´æÇø
///////////////////////////////////////////////////////////////////////////////////////
//ÎªexfunsÉêÇëÄÚ´æ
//·µ»ØÖµ:0,³É¹¦
//1,Ê§°Ü
u8 exfuns_init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++)
	{
		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//Îª´ÅÅÌi¹¤×÷ÇøÉêÇëÄÚ´æ	
		if(!fs[i])break;
	}
	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//ÎªfileÉêÇëÄÚ´æ
	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//ÎªftempÉêÇëÄÚ´æ
	fatbuf=(u8*)mymalloc(SRAMIN,512);				//ÎªfatbufÉêÇëÄÚ´æ
	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //ÉêÇëÓĞÒ»¸öÊ§°Ü,¼´Ê§°Ü.
	else return 1;	
}

//½«Ğ¡Ğ´×ÖÄ¸×ªÎª´óĞ´×ÖÄ¸,Èç¹ûÊÇÊı×Ö,Ôò±£³Ö²»±ä.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//Êı×Ö,±£³Ö²»±ä.
	if(c>='a')return c-0x20;//±äÎª´óĞ´.
	else return c;//´óĞ´,±£³Ö²»±ä
}	      
//±¨¸æÎÄ¼şµÄÀàĞÍ
//fname:ÎÄ¼şÃû
//·µ»ØÖµ:0XFF,±íÊ¾ÎŞ·¨Ê¶±ğµÄÎÄ¼şÀàĞÍ±àºÅ.
//		 ÆäËû,¸ßËÄÎ»±íÊ¾ËùÊô´óÀà,µÍËÄÎ»±íÊ¾ËùÊôĞ¡Àà.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//ºó×ºÃû
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//Æ«ÒÆµ½ÁË×îºóÁË.
		fname++;
	}
	if(i==250)return 0XFF;//´íÎóµÄ×Ö·û´®.
 	for(i=0;i<5;i++)//µÃµ½ºó×ºÃû
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//È«²¿±äÎª´óĞ´ 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//´óÀà¶Ô±È
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//×ÓÀà¶Ô±È
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//´Ë×éÒÑ¾­Ã»ÓĞ¿É¶Ô±ÈµÄ³ÉÔ±ÁË.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//ÕÒµ½ÁË
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//Ã»ÕÒµ½		 			   
}	 

//µÃµ½´ÅÅÌÊ£ÓàÈİÁ¿
//drv:´ÅÅÌ±àºÅ("0:"/"1:")
//total:×ÜÈİÁ¿	 £¨µ¥Î»KB£©
//free:Ê£ÓàÈİÁ¿	 £¨µ¥Î»KB£©
//·µ»ØÖµ:0,Õı³£.ÆäËû,´íÎó´úÂë
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //µÃµ½´ÅÅÌĞÅÏ¢¼°¿ÕÏĞ´ØÊıÁ¿
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//µÃµ½×ÜÉÈÇøÊı
	    fre_sect=fre_clust*fs1->csize;			//µÃµ½¿ÕÏĞÉÈÇøÊı	   
#if _MAX_SS!=512				  				//ÉÈÇø´óĞ¡²»ÊÇ512×Ö½Ú,Ôò×ª»»Îª512×Ö½Ú
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//µ¥Î»ÎªKB
		*free=fre_sect>>1;	//µ¥Î»ÎªKB 
 	}
	return res;
}	











