#include "stm32f10x.h"
#include "gui.h"
#include "calendar.h"
#include "RTC.h"
#include "mstimer.h"
#include "Tiky_LCD.h" 
#include "display.h"
#include "SHT2x.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "Protol_3762.h"
#include "interface_disp.h"
//#include "sdio_sdcard.h"
#include "sd_spi.h"
#include "turn_ASCII.h"

#include "HostProcess.h"

//extern const GUI_BITMAP bmlv;
//extern const GUI_BITMAP bmcheng;
//extern const GUI_BITMAP bmhuang;
//extern const GUI_BITMAP bmB7565;


extern const GUI_BITMAP bmnum_bg_135_48;
extern const GUI_BITMAP bmnum_bg_135_60;    

extern void show_main_interface(void);




//��ʾ����̶�����Ĳ���
void display_contant(void)
{
    Lcd_ColorBox(390,76,12,9,WHITE);            
    Lcd_ColorBox(390,218,12,9,WHITE);      
    Lcd_ColorBox(390,360,12,9,WHITE);            //��ʾ����С�ĳ�����
    GUI_SetFont(&GUI_Font32);
    GUI_DispStringAt("T", 390, 105);            //��ʾT
    GUI_DispStringAt("RH", 390, 247);           //��ʾRH
    GUI_DispStringAt("CO2", 390, 389);          //��ʾCO2

    GUI_SetFont(&GUI_Font8x8);
    GUI_DispStringAt("0",759, 109);
    GUI_SetFont(&GUI_Font32);
    GUI_DispStringAt("C", 767, 105);            //��ʾ�¶ȵ�λ��
    GUI_DispStringAt("%", 767, 247);            //��ʾʪ�ȵ�λ%																																																
    GUI_DispStringAt("PPM", 735, 389);          //��ʾCO2��λPPM

    GUI_DrawHLine(164,390,795);
    GUI_DrawHLine(306,390,795);
}
//���ڴ洢��Ļ������ʾPM10��ֵ�������ж���Ļ�Ƿ���Ҫˢ��
static uint16_t s_iPlaying_PM10 = 0;

void display_num(void)
{
	
	extern float pm2_5,CO2_ppm,Humidity,Temperature;

	uint16_t co2_num_posx       = 1200/2, co2_num_posy = 360/2;          //       1356 1450    
	uint16_t pm2_5_num_posx     = 1200/2, pm2_5_num_posy = 680/2;
	static  float tttttt_sum    = 1001;

	// ��ʾPM10
	/*
		��������ֵС��35������Ļ������ʾ��ֵ����35�����0ʱ��
		��Ϊ��������ֵ��Χ�����˱仯����Ļˢ��
		s_iPlaying_PM10�ж��Ƿ����0������Ϊ��ʼֵΪ0
	*/
	if((PM_value.PM10 <= 35) && ((s_iPlaying_PM10 > 35) || (s_iPlaying_PM10 == 0)))
	{
		int x0 = 306/2; 
		int y0 = 560/2; 

		GUI_SetColor( GUI_DARKCYAN ); 
		GUI_DrawArc( x0,y0,54, 54,0, 360 );   //306-197 = 109
		/* ��ʾ��ɫ */
		GUI_SetColor( GUI_GREEN ); 
		GUI_DrawArc( x0,y0,54, 54,-30, 90 );   //306-197 = 109
		s_iPlaying_PM10 = PM_value.PM10;
	}
	else if((PM_value.PM10 <= 75) && (PM_value.PM10 > 35) 
		&& ((s_iPlaying_PM10 <= 35) || (s_iPlaying_PM10 > 75)))
	{
		int x0 = 306/2; 
		int y0 = 560/2; 

		GUI_SetColor( GUI_DARKCYAN ); 
		GUI_DrawArc( x0,y0,54, 54,0, 360 );   //306-197 = 109
		/* ��ʾ��ɫ */
		GUI_SetColor( GUI_YELLOW ); 
		GUI_DrawArc( x0,y0,54, 54,-150, 90 );   //306-197 = 109
		s_iPlaying_PM10 = PM_value.PM10;
	}
	else if((PM_value.PM10 > 75) && (s_iPlaying_PM10 <= 75))
	{
		int x0 = 306/2; 
		int y0 = 560/2; 

		/* ��ʾ��ɫ */
		GUI_SetColor( GUI_RED ); 
		GUI_DrawArc( x0,y0,54, 54,0, 360 );   //306-197 = 109
		s_iPlaying_PM10 = PM_value.PM10;
	}

	GUI_SetTextAlign(GUI_TA_LEFT);

	GUI_SetBkColor(0x1A0D07); 
	//��ʾ�¶�
	GUI_SetFont(&GUI_Font72);
	//        GUI_DrawBitmap(&bmnum_bg_135_60,   650/2, 375/2); 

	GUI_GotoX(660/2);           //X����  660  814  900   
	GUI_GotoY(370/2);           //Y����
	GUI_SetColor( GUI_WHITE ); 
	GUI_SetTextMode(LCD_DRAWMODE_NORMAL); 
	GUI_DispFloatMin(Temperature,1);  //��ʾС����Temperature
	GUI_DispCEOL_T();     //����

	//��ʾʪ��
	//        GUI_DrawBitmap(&bmnum_bg_135_60,   650/2, 700/2); 

	GUI_GotoX(660/2);           //X����
	GUI_GotoY(690/2);           //Y����
	GUI_SetTextAlign(GUI_TA_LEFT);
	GUI_SetColor( GUI_WHITE ); 
	GUI_SetTextMode(LCD_DRAWMODE_NORMAL); 
	GUI_DispFloatMin(Humidity,1);  //��ʾС����
	GUI_DispCEOL_RH();     //����
	
	//��ʾCO2
	GUI_GotoX(co2_num_posx);           //X����
	GUI_GotoY(co2_num_posy);           //Y����

	GUI_SetTextAlign(GUI_TA_LEFT);

	if(CO2_ppm<10)
	{
//      GUI_DrawBitmap(&bmnum_bg_135_60,   co2_num_posx, co2_num_posy); 
		GUI_DispDecSpace(CO2_ppm,1);//��ʾ1λ������
	}
	else if(CO2_ppm<100)
	{
//      GUI_DrawBitmap(&bmnum_bg_135_60,   co2_num_posx, co2_num_posy); 
		GUI_DispDecSpace(CO2_ppm,2);//��ʾ1λ������
	}
	else if(CO2_ppm<1000)
	{
//		GUI_DrawBitmap(&bmnum_bg_135_60,   co2_num_posx, co2_num_posy); 
		GUI_DispDecSpace(CO2_ppm,3);//��ʾ1λ������    
	}
	else
	{
//      GUI_DrawBitmap(&bmnum_bg_135_60,   co2_num_posx, co2_num_posy); 
		GUI_DispDecSpace(CO2_ppm,4);//��ʾ1λ������
	}
	GUI_DispCEOL_CO2();

	//��ʾPM2.5

	GUI_GotoX(pm2_5_num_posx);           //X����
	GUI_GotoY(pm2_5_num_posy);           //Y����

	GUI_SetTextAlign(GUI_TA_LEFT);

	if(PM_value.PM2_5<10)
	{
		//            GUI_DrawBitmap(&bmnum_bg_135_60,   pm2_5_num_posx, pm2_5_num_posy); 
		GUI_DispDecSpace(PM_value.PM2_5, 1);//��ʾ1λ������
	}
	else if(PM_value.PM2_5<100)
	{
		//            GUI_DrawBitmap(&bmnum_bg_135_60,   pm2_5_num_posx, pm2_5_num_posy); 
		GUI_DispDecSpace(PM_value.PM2_5, 2);//��ʾ1λ������
	}
	else if(PM_value.PM2_5<1000)
	{
		//            GUI_DrawBitmap(&bmnum_bg_135_60,   pm2_5_num_posx, pm2_5_num_posy); 
		GUI_DispDecSpace(PM_value.PM2_5, 3);//��ʾ1λ������    
	}
	else
	{
		//            GUI_DrawBitmap(&bmnum_bg_135_60,   pm2_5_num_posx, pm2_5_num_posy); 
		GUI_DispDecSpace(PM_value.PM2_5, 4);//��ʾ1λ������
	}

	GUI_DispCEOL_PM2_5();

	//��ʾʱ��
	GUI_SetBkColor(GUI_BLACK); 
	GUI_SetTextMode(LCD_DRAWMODE_NORMAL);
	Time_Show(752/2,75);                   
	//    GUI_DispCEOL();	
	

}

void first_disp(void)
{
	  //��ʾʱ��
	Time_Show(146,340);                    
	GUI_DispCEOL();	
	//	Time_Show(146,400);                    
	//GUI_DispCEOL();	
		
	Lcd_ColorBox(390,76,12,9,WHITE);            
	Lcd_ColorBox(390,218,12,9,WHITE);      
	Lcd_ColorBox(390,360,12,9,WHITE);            //��ʾ����С�ĳ�����
	GUI_SetFont(&GUI_Font32);
	GUI_DispStringAt("T", 390, 105);            //��ʾT
	GUI_DispStringAt("RH", 390, 247);           //��ʾRH
	GUI_DispStringAt("CO2", 390, 389);          //��ʾCO2

	GUI_SetFont(&GUI_Font8x8);
	GUI_DispStringAt("0",759, 109);
	GUI_SetFont(&GUI_Font32);
	GUI_DispStringAt("C", 767, 105);            //��ʾ�¶ȵ�λ��
	GUI_DispStringAt("%", 767, 247);            //��ʾʪ�ȵ�λ%																																																
	GUI_DispStringAt("PPM", 735, 389);          //��ʾCO2��λPPM

	GUI_DrawHLine(164,390,795);
	GUI_DrawHLine(306,390,795);


	GUI_SetFont(&GUI_Font32B_ASCII);
	GUI_DispStringAt("Loading...",585, 81);
	GUI_DispStringAt("Loading...",585, 223);
	GUI_DispStringAt("Loading...",585, 365);
	GUI_DispStringAt("Loading...",150, 180);     //������ʾLoading

}	
extern char read_dat[44];
void collect(void)
{
    Read_CO2();
    Read_PM();
    Read_T();
    Read_H();
    environment_list ();
}
int b=0;
int t;
static int step_t;
extern int times_flag,second,times_flag1;
void inall(void)
{
	if(b==0)
	{
//      first_disp();                               //ǰ20s�ȴ���ʼ����ʾ������
		collect();                                  //�ɼ�����
		Send_date();                               //��֡
		switch(step_t)
		{
			case 0:
				MSTimerSet(t, 10000);
				GPIO_SetBits(GPIOF, GPIO_Pin_15);	 	 //	BL = 1 
				step_t++;
			break;
			case 1:
				if(MSTimerCheck(t))                 //10s��
				{
					step_t++;							 
				}
				else                               //10sû��
				{
					step_t=1;
				} 
			break;
			case 2:
				MSTimerSet(t, 10000);
				step_t++;
			break;
			case 3:
				if(MSTimerCheck(t))                 //10s��
				{
					b=1;  
				}
				else                               //10sû��
				{
					step_t=3;
				} 
			break;

			default:
			break;
		}
	}
 	
		
	if(b == 1)
	{

		collect();                                //�ɼ�����
		show_Menu();                              //������ʾ����
		disp();                                                                        //���԰�������
		turn_ascii();                             //��������ת��

		if(timer.sec==second-1)
		{
			times_flag1=1;
		}
		b=1;
	}
}


