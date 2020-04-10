/*******************************************************************************
* File Name          : main.c
* Author             : Wuhan R&D Center, Embest
* Date First Issued  : 08/08/2008
* Description        : Main program body
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "Tiky_LCD.h"
//#include "lcm_api.h"
#include "calendar.h"
#include "gui.h"
//#include "ADC.H"
#include "RTC.h"
#include "HostProcess.h"
#include "SHT2x.h"

/************ 用于定义ITM Viewer相关的ITM激励寄存器端口 ************************/
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

/*用于定义是否使用ITM Viewer*/
//#define DBG_ITM   

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define RTCClockSource_LSI   /* Use the internal 32 KHz oscillator as RTC clock source */
#define RTCClockSource_LSE   /* Use the external 32.768 KHz oscillator as RTC clock source */
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
vu32 TimeDisplay = 0;
//static ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/

//void RTC_Configuration(void);
//void RTC_NVIC_Configuration(void);
//u32 Time_Regulate(void);
//void Time_Adjust(void);
//void Time_Show(u16 x,u16 y);
//void Time_Display(u32 TimeVar,u16 x,u16 y);


/* Private functions ---------------------------------------------------------*/ 
/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_main(void)
{

  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */
   
    /* RTC Configuration */
    RTC_Configuration();
 
    /* Adjust time by values entred by the user on the hyperterminal */
    Time_Adjust();

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);    
  }
  else
  {//========================================================
		//启用PWR和BKP的时钟（from APB1）
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
//    //后备域解锁
//    PWR_BackupAccessCmd(ENABLE);
//		
//		#ifdef RTCClockSource_LSI
//  /* Enable LSI */ 
//  RCC_LSICmd(ENABLE);
//  /* Wait till LSI is ready */
//  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
//  {
//  }

//  /* Select LSI as RTC Clock Source */
//  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
//	#endif
//		RCC_RTCCLKCmd(ENABLE);
	//==========================================================
    /* Check if the Power On Reset flag is set */
    if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      //printf("\r\n\n Power On Reset occurred....");
    }
    /* Check if the Pin Reset flag is set */
    else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      //printf("\r\n\n External Reset occurred....");
    }

    //printf("\r\n No need to configure RTC....");    
    /* Wait for RTC registers synchronization */
//    RTC_WaitForSynchro();

//     /* Enable the RTC Second */  
//     RTC_ITConfig(RTC_IT_SEC, ENABLE);
//     /* Wait until last write operation on RTC registers has finished */
//     RTC_WaitForLastTask();
//  }

	/* Clear reset flags */
	RCC_ClearFlag();
	/*允许RTC报警中断*/
	RTC_WaitForSynchro();
	RTC_ITConfig(RTC_IT_ALR, ENABLE); 

	/*等待最后一条写指令完成*/
	RTC_WaitForLastTask();
}
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_NVIC_Configuration(void)
{
//   NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the RTC Interrupt */
//   NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
//   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//   NVIC_Init(&NVIC_InitStructure);
}



/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
	
// 	RCC_RTCCLKCmd(ENABLE);


  /* Reset Backup Domain */
  BKP_DeInit();

#ifdef RTCClockSource_LSI
  /* Enable LSI */ 
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select LSI as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);  
#elif defined	RTCClockSource_LSE  
  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  
#endif


  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);


#ifdef RTCClockOutput_Enable  
  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                               functionality must be disabled */
                               
  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCCalibrationClockOutputCmd(ENABLE);
#endif 

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  
  /* Enable the RTC Second */  
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  
  /* Set RTC prescaler: set RTC period to 1sec */
#ifdef RTCClockSource_LSI
  RTC_SetPrescaler(41222); /* RTC period = RTCCLK/RTC_PR = (32.000 KHz)/(31999+1) */
#elif defined	RTCClockSource_LSE
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
#endif
  
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	
}

/*******************************************************************************
* Function Name  : Time_Regulate
* Description    : Returns the time entered by user, using Hyperterminal.
* Input          : None
* Output         : None
* Return         : Current time RTC counter value
*******************************************************************************/
u32 Time_Regulate(void)
{
  u32 Tmp_HH = 0xFF, Tmp_MM = 0xFF, Tmp_SS = 0xFF;
/*
  printf("\r\n==============Time Settings=====================================");
  printf("\r\n  Please Set Hours");
  
  while(Tmp_HH == 0xFF)
  {
    Tmp_HH = USART_Scanf(23);
  }
  printf(":  %d", Tmp_HH); 
  printf("\r\n  Please Set Minutes");
  while(Tmp_MM == 0xFF)
  {
    Tmp_MM = USART_Scanf(59);
  }
  printf(":  %d", Tmp_MM); 
  printf("\r\n  Please Set Seconds");
  while(Tmp_SS == 0xFF)
  {
    Tmp_SS = USART_Scanf(59);
  }
  printf(":  %d", Tmp_SS); 
*/
  //rtc_set(2010,11,29,Tmp_HH,Tmp_MM,Tmp_SS);
  rtc_set(2017,12,6,21,56,0);
  /* Return the value to store in RTC counter register */
  return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}

/*******************************************************************************
* Function Name  : Time_Adjust
* Description    : Adjusts time.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Time_Adjust(void)
{
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask(); 
	/* Change the current time */
	Time_Regulate();
	//RTC_SetCounter(Time_Regulate());

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();   
}

/*******************************************************************************
* Function Name  : Time_Display
* Description    : Displays the current time.
* Input          : - TimeVar: RTC counter value.
* Output         : None
* Return         : None
*******************************************************************************/
extern 	unsigned int ADCConvertedValue;

extern const GUI_BITMAP bmnum_yue_ri;
extern const GUI_BITMAP bmnum_xq_ri;
extern const GUI_BITMAP bmnum_nian;

static uint8_t s_cPlaying_Time[8];//用于存储屏幕正在显示时间
static uint16_t s_iPlaying_year;
static uint8_t s_ciPlaying_mon;
static uint8_t s_ciPlaying_day;
extern float  temperature;
void Time_Get(void)
{
	rtc_get();
	//记录开机时间用于屏幕刷新
	s_iPlaying_year = 1995;
	s_ciPlaying_mon = 0x01;
	s_ciPlaying_day = 0x01;
	
	//记录开机时间，用于温度矫正
	g_cBoot_StrapHour = timer.hour;
	g_cBoot_StrapMin = timer.min;

	SHT21_Work_T();
//	if(temperature_flag == 1)
	{
		g_fBoot_StrapTem = temperature;  //返回温度
	}
}

void Time_Display(u32 TimeVar,u16 x,u16 y)
{
//  double Value;
    
    int hz_pos_y = 7, hz_pos_x = 630/2;
    int hz_xq_pos_x = hz_pos_x + 64 * 4;
    
	rtc_get();
	
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_WHITE);
	
//	GUI_SetFont(&GUI_Font20);
//  GUI_DispDecAt( timer.w_year ,x ,y+60,4);
//  GUI_DispCharAt('.',x+32,y+60);
//  GUI_DispDecAt( timer.w_month, x+40,y+60,2);
//  GUI_DispCharAt('.',x+56,y+60);
//  GUI_DispDecAt( timer.w_date , x+64,y+60,2);

    GUI_SetFont(&GUI_Font32);
    GUI_SetColor( GUI_WHITE ); 
    /*
		时间更改，与屏幕正在显示的时间s_cPlaying_Time[]不同
		对应位置屏幕刷新，更改时间
	  */
	if(s_iPlaying_year != timer.w_year)
	{
		GUI_DrawBitmap(&bmnum_nian,   hz_pos_x-2, hz_pos_y - 1); 
		GUI_DispDecAt(timer.w_year,   hz_pos_x, hz_pos_y, 4);
		s_iPlaying_year = timer.w_year;
	}
		
	if((s_ciPlaying_mon != timer.w_month))
	{
		GUI_DrawBitmap(&bmnum_yue_ri, hz_pos_x + 64 + 32 - 1, hz_pos_y - 1); 
		GUI_DispDecAt(timer.w_month,     hz_pos_x + 64 + 32, hz_pos_y, 2);
		s_ciPlaying_mon = timer.w_month;
	}
     
	if((s_ciPlaying_day != timer.w_date))
	{
		GUI_DrawBitmap(&bmnum_yue_ri, hz_pos_x + 64 + 32 + 32 + 32 - 1, hz_pos_y - 1); 
		GUI_DispDecAt(timer.w_date,     hz_pos_x + 64 + 32 + 32 + 32, hz_pos_y, 2);
		s_ciPlaying_day = timer.w_date;
	}
    
    
    
    GUI_SetFont(&GUI_Font44);
    GUI_DispDecAt( timer.hour, x,y,2);
    GUI_DispCharAt(':',x+44,y);
    GUI_DispDecAt( timer.min , x+55,y,2);
    
    
//  GUI_DispCharAt(':',x+99,y);
//  GUI_DispDecAt( timer.sec , x+110,y,2);

//  Value=ADC_Filter();
////   temp=(1.42 - Value*3.3/4096)*1000/4.35 + 40;
////   Value=(1.4 - Value*3.3/4096)*1000/4.3 + 40;
//  GUI_GotoXY(x+10*8,y+16);
//  GUI_DispFloat( Value ,4);	 //ADCConvertedValue	GUI_DispDecAt   GUI_DispFloat  GUI_DispDecAt
  //GUI_DispDecAt( ADCConvertedValue ,x+10*8,y+16,4);
}

/*******************************************************************************
* Function Name  : Time_Show
* Description    : Shows the current time (HH:MM:SS) on the Hyperterminal.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Time_Show(u16 x,u16 y)
{
	Time_Display(RTC_GetCounter(),x,y);
	TimeDisplay = 0;
}


/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
