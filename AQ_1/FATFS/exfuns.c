#include "string.h"
#include "exfuns.h"

//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌÐòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßÐí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEKÕ½½¢STM32¿ª·¢°åV3
//FATFS À©Õ¹´úÂë	   
//ÕýµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2015/1/20
//°æ±¾£ºV1.0
//°æÈ¨ËùÓÐ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖÝÊÐÐÇÒíµç×Ó¿Æ¼¼ÓÐÏÞ¹«Ë¾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define FILE_MAX_TYPE_NUM		6	//×î¶àFILE_MAX_TYPE_NUM¸ö´óÀà
#define FILE_MAX_SUBT_NUM		13	//×î¶àFILE_MAX_SUBT_NUM¸öÐ¡Àà
#define SRAMIN	 0		//ÄÚ²¿ÄÚ´æ³Ø
#define SRAMEX   1		//Íâ²¿ÄÚ´æ³


 //ÎÄ¼þÀàÐÍÁÐ±í
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BINÎÄ¼þ
{"LRC"},			//LRCÎÄ¼þ
{"NES"},			//NESÎÄ¼þ
{"TXT","C","H"},	//ÎÄ±¾ÎÄ¼þ
{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","AAC","WMA","WAV","MID","FLAC"},//ÒôÀÖÎÄ¼þ
{"BMP","JPG","JPEG","GIF"},//Í¼Æ¬ÎÄ¼þ 
};
///////////////////////////////¹«¹²ÎÄ¼þÇø,Ê¹ÓÃmallocµÄÊ±ºò////////////////////////////////////////////
FATFS *fs[_VOLUMES];//Âß¼­´ÅÅÌ¹¤×÷Çø.	 
FIL *file;	  		//ÎÄ¼þ1
FIL *ftemp;	  		//ÎÄ¼þ2.
UINT br,bw;			//¶ÁÐ´±äÁ¿
FILINFO fileinfo;	//ÎÄ¼þÐÅÏ¢
DIR dir;  			//Ä¿Â¼

u8 *fatbuf;			//SD¿¨Êý¾Ý»º´æÇø
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
	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //ÉêÇëÓÐÒ»¸öÊ§°Ü,¼´Ê§°Ü.
	else return 1;	
}

//½«Ð¡Ð´×ÖÄ¸×ªÎª´óÐ´×ÖÄ¸,Èç¹ûÊÇÊý×Ö,Ôò±£³Ö²»±ä.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//Êý×Ö,±£³Ö²»±ä.
	if(c>='a')return c-0x20;//±äÎª´óÐ´.
	else return c;//´óÐ´,±£³Ö²»±ä
}	      
//±¨¸æÎÄ¼þµÄÀàÐÍ
//fname:ÎÄ¼þÃû
//·µ»ØÖµ:0XFF,±íÊ¾ÎÞ·¨Ê¶±ðµÄÎÄ¼þÀàÐÍ±àºÅ.
//		 ÆäËû,¸ßËÄÎ»±íÊ¾ËùÊô´óÀà,µÍËÄÎ»±íÊ¾ËùÊôÐ¡Àà.
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
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//È«²¿±äÎª´óÐ´ 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//´óÀà¶Ô±È
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//×ÓÀà¶Ô±È
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//´Ë×éÒÑ¾­Ã»ÓÐ¿É¶Ô±ÈµÄ³ÉÔ±ÁË.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//ÕÒµ½ÁË
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//Ã»ÕÒµ½		 			   
}	 

















//ÄÚ´æ³Ø(32×Ö½Ú¶ÔÆë)
__align(32) u8 mem1base[MEM1_MAX_SIZE];													//ÄÚ²¿SRAMÄÚ´æ³Ø
__align(32) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));					//Íâ²¿SRAMÄÚ´æ³Ø
//ÄÚ´æ¹ÜÀí±í
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];													//ÄÚ²¿SRAMÄÚ´æ³ØMAP
u16 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+MEM2_MAX_SIZE)));	//Íâ²¿SRAMÄÚ´æ³ØMAP
//ÄÚ´æ¹ÜÀí²ÎÊý	   
const u32 memtblsize[SRAMBANK]={MEM1_ALLOC_TABLE_SIZE,MEM2_ALLOC_TABLE_SIZE};			//ÄÚ´æ±í´óÐ¡
const u32 memblksize[SRAMBANK]={MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE};						//ÄÚ´æ·Ö¿é´óÐ¡
const u32 memsize[SRAMBANK]={MEM1_MAX_SIZE,MEM2_MAX_SIZE};								//ÄÚ´æ×Ü´óÐ¡


//ÄÚ´æ¹ÜÀí¿ØÖÆÆ÷
struct _m_mallco_dev mallco_dev=
{
	my_mem_init,				//ÄÚ´æ³õÊ¼»¯
	my_mem_perused,				//ÄÚ´æÊ¹ÓÃÂÊ
	mem1base,mem2base,			//ÄÚ´æ³Ø
	mem1mapbase,mem2mapbase,	//ÄÚ´æ¹ÜÀí×´Ì¬±í
	0,0,  		 				//ÄÚ´æ¹ÜÀíÎ´¾ÍÐ÷
};

//¸´ÖÆÄÚ´æ
//*des:Ä¿µÄµØÖ·
//*src:Ô´µØÖ·
//n:ÐèÒª¸´ÖÆµÄÄÚ´æ³¤¶È(×Ö½ÚÎªµ¥Î»)
void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  
//ÉèÖÃÄÚ´æ
//*s:ÄÚ´æÊ×µØÖ·
//c :ÒªÉèÖÃµÄÖµ
//count:ÐèÒªÉèÖÃµÄÄÚ´æ´óÐ¡(×Ö½ÚÎªµ¥Î»)
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	   
//ÄÚ´æ¹ÜÀí³õÊ¼»¯  
//memx:ËùÊôÄÚ´æ¿é
void my_mem_init(u8 memx)  
{  
    mymemset(mallco_dev.memmap[memx], 0,memtblsize[memx]*2);//ÄÚ´æ×´Ì¬±íÊý¾ÝÇåÁã  
	mymemset(mallco_dev.membase[memx], 0,memsize[memx]);	//ÄÚ´æ³ØËùÓÐÊý¾ÝÇåÁã  
	mallco_dev.memrdy[memx]=1;								//ÄÚ´æ¹ÜÀí³õÊ¼»¯OK  
}  
//»ñÈ¡ÄÚ´æÊ¹ÓÃÂÊ
//memx:ËùÊôÄÚ´æ¿é
//·µ»ØÖµ:Ê¹ÓÃÂÊ(0~100)
u8 my_mem_perused(u8 memx)  
{  
    u32 used=0;  
    u32 i;  
    for(i=0;i<memtblsize[memx];i++)  
    {  
        if(mallco_dev.memmap[memx][i])used++; 
    } 
    return (used*100)/(memtblsize[memx]);  
}  
//ÄÚ´æ·ÖÅä(ÄÚ²¿µ÷ÓÃ)
//memx:ËùÊôÄÚ´æ¿é
//size:Òª·ÖÅäµÄÄÚ´æ´óÐ¡(×Ö½Ú)
//·µ»ØÖµ:0XFFFFFFFF,´ú±í´íÎó;ÆäËû,ÄÚ´æÆ«ÒÆµØÖ· 
u32 my_mem_malloc(u8 memx,u32 size)  
{  
    signed long offset=0;  
    u32 nmemb;	//ÐèÒªµÄÄÚ´æ¿éÊý  
	u32 cmemb=0;//Á¬Ðø¿ÕÄÚ´æ¿éÊý
    u32 i;  
    if(!mallco_dev.memrdy[memx])mallco_dev.init(memx);//Î´³õÊ¼»¯,ÏÈÖ´ÐÐ³õÊ¼»¯ 
    if(size==0)return 0XFFFFFFFF;//²»ÐèÒª·ÖÅä
    nmemb=size/memblksize[memx];  	//»ñÈ¡ÐèÒª·ÖÅäµÄÁ¬ÐøÄÚ´æ¿éÊý
    if(size%memblksize[memx])nmemb++;  
    for(offset=memtblsize[memx]-1;offset>=0;offset--)//ËÑË÷Õû¸öÄÚ´æ¿ØÖÆÇø  
    {     
		if(!mallco_dev.memmap[memx][offset])cmemb++;//Á¬Ðø¿ÕÄÚ´æ¿éÊýÔö¼Ó
		else cmemb=0;								//Á¬ÐøÄÚ´æ¿éÇåÁã
		if(cmemb==nmemb)							//ÕÒµ½ÁËÁ¬Ðønmemb¸ö¿ÕÄÚ´æ¿é
		{
            for(i=0;i<nmemb;i++)  					//±ê×¢ÄÚ´æ¿é·Ç¿Õ 
            {  
                mallco_dev.memmap[memx][offset+i]=nmemb;  
            }  
            return (offset*memblksize[memx]);//·µ»ØÆ«ÒÆµØÖ·  
		}
    }  
    return 0XFFFFFFFF;//Î´ÕÒµ½·ûºÏ·ÖÅäÌõ¼þµÄÄÚ´æ¿é  
}  
//ÊÍ·ÅÄÚ´æ(ÄÚ²¿µ÷ÓÃ) 
//memx:ËùÊôÄÚ´æ¿é
//offset:ÄÚ´æµØÖ·Æ«ÒÆ
//·µ»ØÖµ:0,ÊÍ·Å³É¹¦;1,ÊÍ·ÅÊ§°Ü;  
u8 my_mem_free(u8 memx,u32 offset)  
{  
    int i;  
    if(!mallco_dev.memrdy[memx])//Î´³õÊ¼»¯,ÏÈÖ´ÐÐ³õÊ¼»¯
	{
		mallco_dev.init(memx);    
        return 1;//Î´³õÊ¼»¯  
    }  
    if(offset<memsize[memx])//Æ«ÒÆÔÚÄÚ´æ³ØÄÚ. 
    {  
        int index=offset/memblksize[memx];			//Æ«ÒÆËùÔÚÄÚ´æ¿éºÅÂë  
        int nmemb=mallco_dev.memmap[memx][index];	//ÄÚ´æ¿éÊýÁ¿
        for(i=0;i<nmemb;i++)  						//ÄÚ´æ¿éÇåÁã
        {  
            mallco_dev.memmap[memx][index+i]=0;  
        }  
        return 0;  
    }else return 2;//Æ«ÒÆ³¬ÇøÁË.  
}  
//ÊÍ·ÅÄÚ´æ(Íâ²¿µ÷ÓÃ) 
//memx:ËùÊôÄÚ´æ¿é
//ptr:ÄÚ´æÊ×µØÖ· 
void myfree(u8 memx,void *ptr)  
{  
	u32 offset;   
	if(ptr==NULL)return;//µØÖ·Îª0.  
 	offset=(u32)ptr-(u32)mallco_dev.membase[memx];     
    my_mem_free(memx,offset);	//ÊÍ·ÅÄÚ´æ      
}  
//·ÖÅäÄÚ´æ(Íâ²¿µ÷ÓÃ)
//memx:ËùÊôÄÚ´æ¿é
//size:ÄÚ´æ´óÐ¡(×Ö½Ú)
//·µ»ØÖµ:·ÖÅäµ½µÄÄÚ´æÊ×µØÖ·.
void *mymalloc(u8 memx,u32 size)  
{  
    u32 offset;   
	offset=my_mem_malloc(memx,size);  	   	 	   
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((u32)mallco_dev.membase[memx]+offset);  
}  
//ÖØÐÂ·ÖÅäÄÚ´æ(Íâ²¿µ÷ÓÃ)
//memx:ËùÊôÄÚ´æ¿é
//*ptr:¾ÉÄÚ´æÊ×µØÖ·
//size:Òª·ÖÅäµÄÄÚ´æ´óÐ¡(×Ö½Ú)
//·µ»ØÖµ:ÐÂ·ÖÅäµ½µÄÄÚ´æÊ×µØÖ·.
void *myrealloc(u8 memx,void *ptr,u32 size)  
{  
    u32 offset;    
    offset=my_mem_malloc(memx,size);   	
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  									   
	    mymemcpy((void*)((u32)mallco_dev.membase[memx]+offset),ptr,size);	//¿½±´¾ÉÄÚ´æÄÚÈÝµ½ÐÂÄÚ´æ   
        myfree(memx,ptr);  											  		//ÊÍ·Å¾ÉÄÚ´æ
        return (void*)((u32)mallco_dev.membase[memx]+offset);  				//·µ»ØÐÂÄÚ´æÊ×µØÖ·
    }  
}










