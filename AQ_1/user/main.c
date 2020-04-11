/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "gui.h"
#include "Tiky_LCD.h" 	 
#include "calendar.h"
#include "RTC.h"
#include "mstimer.h"
#include "display.h"
#include "delay.h"
#include "SHT2x.h"
#include "usart1.h"
#include "usart3.h"
#include "usart2.h"
#include "date.h"    					//ʵʱʱ��ṹ��
#include "Protol_3762.h"   		        //�ز�ͨ�����ݴ���
#include "interface_disp.h"
#include "WDG.h"
#include "led.h"
#include "key.h" 
#include "turn_ASCII.h"

#include "hcho.h"
#include "comm.h"

#include "sd_spi.h"
#include "ff.h"
#include "exfuns.h"

#include "HostProcess.h"

/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

extern void RTC_main(void);

//extern const GUI_BITMAP bmlv;
//extern const GUI_BITMAP bmcheng;
//extern const GUI_BITMAP bmhuang;

//extern const GUI_BITMAP bmB7565;

extern const GUI_BITMAP bmB1_up_72;
extern const GUI_BITMAP bmPM25_logo;
extern const GUI_BITMAP bmbg_24;
extern const GUI_BITMAP bmbg_24_b;
extern const GUI_BITMAP bmBx_up_60_purl;
extern const GUI_BITMAP bmbottom_256_135_purl;




extern const GUI_BITMAP bmwendu_font;
extern const GUI_BITMAP bmug_m3;
extern const GUI_BITMAP bmco2_font;
extern const GUI_BITMAP bmshidu_font;
extern const GUI_BITMAP bmPM25_font;
extern const GUI_BITMAP bmPM10_font;
extern const GUI_BITMAP bmtemp_icon;
extern const GUI_BITMAP bmPM25_icon;
extern const GUI_BITMAP bmhumity_icon;
extern const GUI_BITMAP bmco2_icon;
extern const GUI_BITMAP bmPM10_circle;
extern const GUI_BITMAP bmB1_up_60_purl;

extern const GUI_BITMAP bmnian;
extern const GUI_BITMAP bmyue;
extern const GUI_BITMAP bmri;

extern const GUI_BITMAP bmnum_bg_135_48;
extern const GUI_BITMAP bmnum_bg_135_60;      

extern const GUI_BITMAP bmnum_yue_ri;
extern const GUI_BITMAP bmnum_xq_ri;
extern const GUI_BITMAP bmnum_nian;


#define     GUI_BG_TEXT_COLOR     0x1a0d07   



#define  SystemFrequency  72000000


int timeflag=0;
u8 sd_rev = 0;
u8 fan_en = 0;




void fs_test(void);
void show_main_interface(void);
extern void GUI_DispCEOL_SS_T(void) ;
uint8_t send_temp[31] = {0x49,0x41,0x51,0x2d,0x42,0x30,0x57,0x30,0x30,0x30,
						 0x49,0x41,0x51,0x2d,0x42,0x30,0x57,0x30,0x30,0x30,
						 0x53,0x53,0x32,0x30,0x32,0x30,0x30,0x30,0x34,0x31,
						 0x31};
uint8_t recv_temp[62];
uint8_t id_temp[21];
int num_temp;
int i_temp;
int main(void)
{	
    SystemInit();                              //��ʼ��STM32
    delay_init(72);                            //SysTick����ʱ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
    Lcd_Initialize();                          //LCD��ʼ��
    InitIwdg();                              //���Ź���ʼ��
    MSTimerInit();                             //��ʱ��ʼ��
    GUI_Init();    	                           //��ʼ��GUI
    RTC_main();                                //RTC��ʼ��
    hw_led_init();	                           //LED��ʼ��
    EXTIX_Init();                              //�ⲿ�жϳ�ʼ��
    CO2_Init();                                //CO2��ʼ��
    SHT21_Init();                              //��ʪ�ȳ�ʼ��
    PM_Init();                                 //PM2.5��ʼ��
    USART_Configuration();                     //�ز���ʼ��
    send_beidongcommand();
    
    USART_ClearFlag(USART1,USART_IT_RXNE);     //����PM2.5Ϊ�����ϱ�ģʽ
    GUI_SetColor(GUI_WHITE);                   //����ǰ��ɫΪ��ɫ
    GUI_SetBkColor(GUI_BLACK);               //���ñ���ɫΪ��ɫ

    GUI_Clear();                               //����
    delay_ms(1000);
    comm_init();

    
    SD_SPI_Init();          //SD��spi��ʼ��
    
    sd_rev = SD_Init();
    
    printf("ssd_rev = %d\n", sd_rev);
    
    if(sd_rev == 0)
    {
        printf("sd is checked i = %d\n", sd_rev);
        exfuns_init();
        if(f_mount(fs[0],"0:",1) != FR_OK) 					//����SD�� 
        {
            printf("sd is NOT mounted\n");
            while(1);
        }
        printf("sd is mounted\n");
    }
    else
        printf("sd is not checked i = %d\n", sd_rev);
    
    printf("first display \n");
    
    show_main_interface();
    Time_Get();
    /* ���ȿ��� */
    if(1)
        GPIO_SetBits(GPIOG,GPIO_Pin_13);
    else
        GPIO_ResetBits(GPIOG,GPIO_Pin_13);
    while(1)
    {
		if(g_nDataProComFlag == 1)
		{
			Host_Code();
			g_nDataProComFlag = 0;
		}
		if(timeflag)
		{
			rtc_set(2019,11,28,23,53,00);
			timeflag = 0;
		}
		

        inall(); 
        
        LED_Run();                                //ָʾ��
        
        IWDG_ReloadCounter();                     //���Ź�
    }
//    
//    fs_test();
    
    
    
    
	while(1)
	{
//        IWDG_ReloadCounter();                     //���Ź�
//        comm_send_buff("test buff\n");
//        delay_ms(1000);
        
		if(timeflag)
		{
			rtc_set(2019,11,28,23,53,00);
			timeflag = 0;
		}
        
        
        if(1)
            GPIO_SetBits(GPIOG,GPIO_Pin_13);
        else
            GPIO_ResetBits(GPIOG,GPIO_Pin_13);
 
        
    #if 1
		inall();                                  //�ܳ���
        
        
//        Read_T();

//        Read_CO2();
//        Read_PM();
//        display_contant();//��ʾ���������
//        display_num();    //��ʾ�ı������
        
        
		IWDG_ReloadCounter();                     //���Ź�
		
        
    #endif
        
//	GPIO_SetBits(GPIOF, GPIO_Pin_15);	 	 //	BL = 1
//  show_Menu();
//  disp();
  }
	
}



void fs_test(void)
{
    FATFS fs;           /* Filesystem object */
    FIL fil;            /* File object */
    UINT bw;            /* Bytes written */
    BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */

    UINT i;
    UINT* ii;

    int res;
	
    res = f_mount(&fs, "0:", 1);
    delay_ms(1000);
    res = f_open(&fil, "0:/20191001.txt", FA_CREATE_ALWAYS|FA_WRITE);

    res = f_write(&fil, "helle,world!--YXY2", 40, &bw);
    res = f_close(&fil);
    
    WriteToSD_password("zhjia", 16);
    WriteToSD_address("saishukeji",16);


    //  f_open(&fil, "0:/test1.txt", FA_READ); 
    //  res =  f_read (&fil,buff,4,ii);
    //  res = f_close(&fil);

    res = f_mount(0, "0:", 1);

}



//extern const GUI_BITMAP bmwendu_font;
//extern const GUI_BITMAP bmug_m3;
//extern const GUI_BITMAP bmtemp_icon;
//extern const GUI_BITMAP bmshidu_font;
//extern const GUI_BITMAP bmPM25_font;
//extern const GUI_BITMAP bmPM10_font;
//extern const GUI_BITMAP bmPM25_icon;
//extern const GUI_BITMAP bmhumity_icon;
//extern const GUI_BITMAP bmco2_icon;
//extern const GUI_BITMAP bmco2_font;
//extern const GUI_BITMAP bmPM10_circle;
//extern const GUI_BITMAP bmnian;
//extern const GUI_BITMAP bmyue;
//extern const GUI_BITMAP bmri;   bmnum_bg

//extern const GUI_BITMAP bmnum_yue_ri;
//extern const GUI_BITMAP bmnum_xq_ri;
//extern const GUI_BITMAP bmnum_nian;


void show_main_interface(void)
{
    int f_sum = 0;
    
    /* ���ñ�����ɫ */
    GUI_SetBkColor(GUI_BLACK); 
    GUI_Clear();                               //����
    
    /* ��ʾ�Ͽ� */
    GUI_DrawBitmap(&bmB1_up_60_purl,0,0);
        
    /* ���÷��� */
    GUI_DrawBitmap(&bmbottom_256_135_purl,280,140);    //����
    GUI_DrawBitmap(&bmbottom_256_135_purl,1110/2,140); //����
    
    GUI_DrawBitmap(&bmbottom_256_135_purl,280,588/2);  //����
    GUI_DrawBitmap(&bmbottom_256_135_purl,1110/2,588/2); //����
    
    GUI_SetTextMode(GUI_TM_TRANS);
    
    /*��λ��ɫ 0xfffc0e*/
    GUI_SetColor(0xfffc0e); 
    
    /* ͼ�� �� ��λ */
    /* �� */
    GUI_SetFont(&GUI_Font8x8);
    GUI_DispStringAt("o",987/2, 471/2);
    GUI_SetFont(&GUI_Font32);
    GUI_DispStringAt("C", 995/2, 240);          
    
    /* % */
    GUI_DispStringAt("%", 990/2, 390);            
    
    /* PPM */
    GUI_DispStringAt("PPM", 1481/2, 472/2);            
    
    /* ug/m3 */
    GUI_DrawBitmap(&bmug_m3, 1481/2, 784/2); 

    /* �¶� */
    GUI_DrawBitmap(&bmwendu_font, 612/2, 324/2); 
    /* CO2*/
    GUI_DrawBitmap(&bmco2_font, 1164/2, 324/2); 
    /* ʪ��*/
    GUI_DrawBitmap(&bmshidu_font, 612/2, 630/2); 
    /* PM2.5*/
    GUI_DrawBitmap(&bmPM25_font, 1164/2, 630/2); 
    
    /* ICON */
    GUI_DrawBitmap(&bmtemp_icon, 992/2, 328/2); 
    GUI_DrawBitmap(&bmhumity_icon, 964/2, 634/2); 
    GUI_DrawBitmap(&bmco2_icon, 1512/2, 322/2); 
    GUI_DrawBitmap(&bmPM25_icon, 1504/2, 630/2); 
    
    /* PM10 Բ�� */
    GUI_DrawBitmap(&bmPM10_circle, 88/2, 415/2); 
    
    /* �¶�  718   388*/
    GUI_SetFont(&GUI_Font72);
    GUI_DispStringAt("C", 995/2, 240);          
    
    /* ��Բ */
    { 
        int x0 = 306/2; 
        int y0 = 560/2; 
        int i; 

        GUI_SetPenSize( 7 );   //Բ����12����
        GUI_SetTextMode(GUI_TM_TRANS); 

        GUI_SetColor( GUI_DARKCYAN ); 
        GUI_DrawArc( x0,y0,54, 54,0, 360 );   //306-197 = 109
        
        GUI_SetTextMode(GUI_TM_TRANS); 
        /* ���� */ 
        GUI_SetFont(&GUI_FontHZ32x32);
        GUI_SetColor( GUI_WHITE ); 
        
        {
        int hz_pos_y = 7, hz_pos_x = 630/2;
        int hz_xq_pos_x = hz_pos_x + 64 * 4;
        
        GUI_DrawBitmap(&bmnum_xq_ri,   hz_xq_pos_x + 64 -2, hz_pos_y - 1);   
            
//        if(f_sum %7 == 0)
//            GUI_DispStringAt("������", hz_xq_pos_x, hz_pos_y);  
//        else if(f_sum %7 == 1)  
//            GUI_DispStringAt("����һ", hz_xq_pos_x, hz_pos_y); 
//        else if(f_sum %7 == 2)  
//            GUI_DispStringAt("���ڶ�", hz_xq_pos_x, hz_pos_y);   
//        else if(f_sum %7 == 3)  
//            GUI_DispStringAt("������", hz_xq_pos_x, hz_pos_y);  
//        else if(f_sum %7 == 4)  
//            GUI_DispStringAt("������", hz_xq_pos_x, hz_pos_y);   
//        else if(f_sum %7 == 5)  
//            GUI_DispStringAt("������", hz_xq_pos_x, hz_pos_y);   
//        else if(f_sum %7 == 6)  
//            GUI_DispStringAt("������", hz_xq_pos_x, hz_pos_y);  
        
        GUI_DispStringAt("��", hz_pos_x + 64, hz_pos_y); 
        GUI_DispStringAt("��", hz_pos_x + 64 + 32 + 32, hz_pos_y); 
        GUI_DispStringAt("��", hz_pos_x + 64 + 32 + 32 + 32 + 32, hz_pos_y); 
        
        /* ���� */
        GUI_SetFont(&GUI_Font32);
        GUI_SetColor( GUI_WHITE ); 
        
        GUI_DrawBitmap(&bmnum_nian,   hz_pos_x-2, hz_pos_y - 1); 
        GUI_DispDecAt(2020 + f_sum,   hz_pos_x, hz_pos_y, 4);
        
        GUI_DrawBitmap(&bmnum_yue_ri, hz_pos_x + 64 + 32 - 1, hz_pos_y - 1); 
        GUI_DispDecAt(04 + f_sum,     hz_pos_x + 64 + 32, hz_pos_y, 2);
        
        GUI_DrawBitmap(&bmnum_yue_ri, hz_pos_x + 64 + 32 + 32 + 32 - 1, hz_pos_y - 1); 
        GUI_DispDecAt(01 + f_sum,     hz_pos_x + 64 + 32 + 32 + 32, hz_pos_y, 2);
        
        }
        f_sum++;
        
    } 
}


/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
