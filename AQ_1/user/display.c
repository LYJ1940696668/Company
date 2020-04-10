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




//显示界面固定不变的部分
void display_contant(void)
{
    Lcd_ColorBox(390,76,12,9,WHITE);            
    Lcd_ColorBox(390,218,12,9,WHITE);      
    Lcd_ColorBox(390,360,12,9,WHITE);            //显示三个小的长方形
    GUI_SetFont(&GUI_Font32);
    GUI_DispStringAt("T", 390, 105);            //显示T
    GUI_DispStringAt("RH", 390, 247);           //显示RH
    GUI_DispStringAt("CO2", 390, 389);          //显示CO2

    GUI_SetFont(&GUI_Font8x8);
    GUI_DispStringAt("0",759, 109);
    GUI_SetFont(&GUI_Font32);
    GUI_DispStringAt("C", 767, 105);            //显示温度单位℃
    GUI_DispStringAt("%", 767, 247);            //显示湿度单位%																																																
    GUI_DispStringAt("PPM", 735, 389);          //显示CO2单位PPM

    GUI_DrawHLine(164,390,795);
    GUI_DrawHLine(306,390,795);
}
//用于存储屏幕正在显示PM10的值，用于判断屏幕是否需要刷新
static uint16_t s_iPlaying_PM10 = 0;

void display_num(void)
{
	
	extern float pm2_5,CO2_ppm,Humidity,Temperature;

	uint16_t co2_num_posx       = 1200/2, co2_num_posy = 360/2;          //       1356 1450    
	uint16_t pm2_5_num_posx     = 1200/2, pm2_5_num_posy = 680/2;
	static  float tttttt_sum    = 1001;

	// 显示PM10
	/*
		传感器的值小于35，且屏幕正在显示的值大于35或等于0时，
		认为传感器数值范围发生了变化，屏幕刷新
		s_iPlaying_PM10判断是否等于0，是因为初始值为0
	*/
	if((PM_value.PM10 <= 35) && ((s_iPlaying_PM10 > 35) || (s_iPlaying_PM10 == 0)))
	{
		int x0 = 306/2; 
		int y0 = 560/2; 

		GUI_SetColor( GUI_DARKCYAN ); 
		GUI_DrawArc( x0,y0,54, 54,0, 360 );   //306-197 = 109
		/* 显示绿色 */
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
		/* 显示黄色 */
		GUI_SetColor( GUI_YELLOW ); 
		GUI_DrawArc( x0,y0,54, 54,-150, 90 );   //306-197 = 109
		s_iPlaying_PM10 = PM_value.PM10;
	}
	else if((PM_value.PM10 > 75) && (s_iPlaying_PM10 <= 75))
	{
		int x0 = 306/2; 
		int y0 = 560/2; 

		/* 显示红色 */
		GUI_SetColor( GUI_RED ); 
		GUI_DrawArc( x0,y0,54, 54,0, 360 );   //306-197 = 109
		s_iPlaying_PM10 = PM_value.PM10;
	}

	GUI_SetTextAlign(GUI_TA_LEFT);

	GUI_SetBkColor(0x1A0D07); 
	//显示温度
	GUI_SetFont(&GUI_Font72);
	//        GUI_DrawBitmap(&bmnum_bg_135_60,   650/2, 375/2); 

	GUI_GotoX(660/2);           //X坐标  660  814  900   
	GUI_GotoY(370/2);           //Y坐标
	GUI_SetColor( GUI_WHITE ); 
	GUI_SetTextMode(LCD_DRAWMODE_NORMAL); 
	GUI_DispFloatMin(Temperature,1);  //显示小数；Temperature
	GUI_DispCEOL_T();     //清屏

	//显示湿度
	//        GUI_DrawBitmap(&bmnum_bg_135_60,   650/2, 700/2); 

	GUI_GotoX(660/2);           //X坐标
	GUI_GotoY(690/2);           //Y坐标
	GUI_SetTextAlign(GUI_TA_LEFT);
	GUI_SetColor( GUI_WHITE ); 
	GUI_SetTextMode(LCD_DRAWMODE_NORMAL); 
	GUI_DispFloatMin(Humidity,1);  //显示小数；
	GUI_DispCEOL_RH();     //清屏
	
	//显示CO2
	GUI_GotoX(co2_num_posx);           //X坐标
	GUI_GotoY(co2_num_posy);           //Y坐标

	GUI_SetTextAlign(GUI_TA_LEFT);

	if(CO2_ppm<10)
	{
//      GUI_DrawBitmap(&bmnum_bg_135_60,   co2_num_posx, co2_num_posy); 
		GUI_DispDecSpace(CO2_ppm,1);//显示1位整数；
	}
	else if(CO2_ppm<100)
	{
//      GUI_DrawBitmap(&bmnum_bg_135_60,   co2_num_posx, co2_num_posy); 
		GUI_DispDecSpace(CO2_ppm,2);//显示1位整数；
	}
	else if(CO2_ppm<1000)
	{
//		GUI_DrawBitmap(&bmnum_bg_135_60,   co2_num_posx, co2_num_posy); 
		GUI_DispDecSpace(CO2_ppm,3);//显示1位整数；    
	}
	else
	{
//      GUI_DrawBitmap(&bmnum_bg_135_60,   co2_num_posx, co2_num_posy); 
		GUI_DispDecSpace(CO2_ppm,4);//显示1位整数；
	}
	GUI_DispCEOL_CO2();

	//显示PM2.5

	GUI_GotoX(pm2_5_num_posx);           //X坐标
	GUI_GotoY(pm2_5_num_posy);           //Y坐标

	GUI_SetTextAlign(GUI_TA_LEFT);

	if(PM_value.PM2_5<10)
	{
		//            GUI_DrawBitmap(&bmnum_bg_135_60,   pm2_5_num_posx, pm2_5_num_posy); 
		GUI_DispDecSpace(PM_value.PM2_5, 1);//显示1位整数；
	}
	else if(PM_value.PM2_5<100)
	{
		//            GUI_DrawBitmap(&bmnum_bg_135_60,   pm2_5_num_posx, pm2_5_num_posy); 
		GUI_DispDecSpace(PM_value.PM2_5, 2);//显示1位整数；
	}
	else if(PM_value.PM2_5<1000)
	{
		//            GUI_DrawBitmap(&bmnum_bg_135_60,   pm2_5_num_posx, pm2_5_num_posy); 
		GUI_DispDecSpace(PM_value.PM2_5, 3);//显示1位整数；    
	}
	else
	{
		//            GUI_DrawBitmap(&bmnum_bg_135_60,   pm2_5_num_posx, pm2_5_num_posy); 
		GUI_DispDecSpace(PM_value.PM2_5, 4);//显示1位整数；
	}

	GUI_DispCEOL_PM2_5();

	//显示时间
	GUI_SetBkColor(GUI_BLACK); 
	GUI_SetTextMode(LCD_DRAWMODE_NORMAL);
	Time_Show(752/2,75);                   
	//    GUI_DispCEOL();	
	

}

void first_disp(void)
{
	  //显示时间
	Time_Show(146,340);                    
	GUI_DispCEOL();	
	//	Time_Show(146,400);                    
	//GUI_DispCEOL();	
		
	Lcd_ColorBox(390,76,12,9,WHITE);            
	Lcd_ColorBox(390,218,12,9,WHITE);      
	Lcd_ColorBox(390,360,12,9,WHITE);            //显示三个小的长方形
	GUI_SetFont(&GUI_Font32);
	GUI_DispStringAt("T", 390, 105);            //显示T
	GUI_DispStringAt("RH", 390, 247);           //显示RH
	GUI_DispStringAt("CO2", 390, 389);          //显示CO2

	GUI_SetFont(&GUI_Font8x8);
	GUI_DispStringAt("0",759, 109);
	GUI_SetFont(&GUI_Font32);
	GUI_DispStringAt("C", 767, 105);            //显示温度单位℃
	GUI_DispStringAt("%", 767, 247);            //显示湿度单位%																																																
	GUI_DispStringAt("PPM", 735, 389);          //显示CO2单位PPM

	GUI_DrawHLine(164,390,795);
	GUI_DrawHLine(306,390,795);


	GUI_SetFont(&GUI_Font32B_ASCII);
	GUI_DispStringAt("Loading...",585, 81);
	GUI_DispStringAt("Loading...",585, 223);
	GUI_DispStringAt("Loading...",585, 365);
	GUI_DispStringAt("Loading...",150, 180);     //开机显示Loading

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
//      first_disp();                               //前20s等待初始化显示的内容
		collect();                                  //采集数据
		Send_date();                               //组帧
		switch(step_t)
		{
			case 0:
				MSTimerSet(t, 10000);
				GPIO_SetBits(GPIOF, GPIO_Pin_15);	 	 //	BL = 1 
				step_t++;
			break;
			case 1:
				if(MSTimerCheck(t))                 //10s到
				{
					step_t++;							 
				}
				else                               //10s没到
				{
					step_t=1;
				} 
			break;
			case 2:
				MSTimerSet(t, 10000);
				step_t++;
			break;
			case 3:
				if(MSTimerCheck(t))                 //10s到
				{
					b=1;  
				}
				else                               //10s没到
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

		collect();                                //采集数据
		show_Menu();                              //调试显示程序
		disp();                                                                        //调试案件程序
		turn_ascii();                             //数据码制转换

		if(timer.sec==second-1)
		{
			times_flag1=1;
		}
		b=1;
	}
}


