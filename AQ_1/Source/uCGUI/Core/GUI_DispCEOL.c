/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_DispBin.c
Purpose     : Routines to display values as binary
----------------------------------------------------------------------
*/

#include "GUI_Protected.h"
#include "GUIDebug.h"
#include "string.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DispCEOL
*/
void GUI_DispCEOL(void) {
  int y = GUI_Context.DispPosY - GUI_GetYAdjust();
  GUI_ClearRect(GUI_Context.DispPosX, y, GUI_Context.DispPosX + 80,             /* Max pos x */
                y +80+ GUI_Context.pAFont->YDist - 1);
}



void GUI_DispCEOL_SS_T(void) 
{
	int y = 388/2;
	GUI_ClearRect(718/2, y, 900/2 + 72,             /* Max pos x */
				  y +72);
}

void GUI_DispCEOL_T(void) 
{
	int y = 185;
	GUI_ClearRect(GUI_Context.DispPosX, y, 905/2,             /* Max pos x */
                  y +72);
}

void GUI_DispCEOL_RH(void) 
{
  int y = 345;
  GUI_ClearRect(GUI_Context.DispPosX, y, 905/2,             /* Max pos x */
                y +72);
}
void GUI_DispCEOL_CO2(void) {
  int y = 185;
  GUI_ClearRect(GUI_Context.DispPosX, y, 1470/2,             /* Max pos x */
                y +72);
}

void GUI_DispCEOL_PM2_5(void) {
  int y = 345;
  GUI_ClearRect(GUI_Context.DispPosX, y, 1470/2,             /* Max pos x */
                y +72);
}





void GUI_DispCEOL_password(void) {
	extern char  mMenu ,sub_mMenu[];
  int x=570, y = 100;
	if(sub_mMenu[mMenu]<=3)
GUI_ClearRect(x+sub_mMenu[mMenu]*24, y,610+sub_mMenu[mMenu]*24 ,154);
	else GUI_ClearRect(x+(sub_mMenu[mMenu]-4)*24, 201,610+(sub_mMenu[mMenu]-4)*24 ,254);
}
//
void GUI_DispCEOL_leftline(void)                 //数字左移时清屏
{
	extern int pas;
  int x=218, y = 298;
	if(pas==10)
	{
    GUI_ClearRect(x, y, 247,   299);          //横线
		GUI_ClearRect(x, 350, 247, 351);          //横线
		GUI_ClearRect(x, y, 219,   350);          //竖线
		GUI_ClearRect(246, y, 247,   350);          //竖线
	}
	else
	{
	  GUI_ClearRect(x+(pas+1)*32, y, 247+(pas+1)*32,   299); 
		GUI_ClearRect(x+(pas+1)*32, 350, 247+(pas+1)*32,   351); 
		GUI_ClearRect(x+(pas+1)*32, y, x+(pas+1)*32+1,   350); 
		GUI_ClearRect(246+(pas+1)*32, y, 246+(pas+1)*32+1,   350); 
	}
}

void GUI_DispCEOL_rightline(void)                   //数字右移时清屏
{
	extern int pas;
  int x=218, y = 298;
	if(pas==0)
	{
  GUI_ClearRect(x+10*32, y, 247+10*32,   299);          /* Max pos x */
		GUI_ClearRect(x+10*32, 350, 247+10*32,   351);
		GUI_ClearRect(x+10*32, y, x+10*32+1,   350);
		GUI_ClearRect(246+10*32, y, 247+10*32+1,   350);
	}
	else
	{
	  GUI_ClearRect(x+(pas-1)*32, y, 247+(pas-1)*32,   299); 
		GUI_ClearRect(x+(pas-1)*32, 350, 247+(pas-1)*32,   351);
		GUI_ClearRect(x+(pas-1)*32, y, x+(pas-1)*32+1,   350);
		GUI_ClearRect(246+(pas-1)*32, y, 247+(pas-1)*32,   350);
	}
}
void GUI_DispCEOL_confirm_num(void)                    //确认数字时清屏
{
  extern int pas;
  int x=224, y = 298;
  GUI_ClearRect(x+pas*32, y, 242+pas*32,   335);
}

void GUI_DispCEOL_asterisk(void) 
{
int x=240, y = 205;
GUI_ClearRect(x, y,490 ,254);
}

void GUI_DispCEOL_date(void) 
{
	extern char  mMenu ,sub_mMenu[];
int x=230, y = 205;
	if(sub_mMenu[mMenu]<=3)
GUI_ClearRect(x+sub_mMenu[mMenu]*24, y,254+sub_mMenu[mMenu]*24 ,254);
	else if(sub_mMenu[mMenu]<=5)
GUI_ClearRect(342+(sub_mMenu[mMenu]-4)*24, y,366+(sub_mMenu[mMenu]-4)*24 ,254);		
	else
GUI_ClearRect(406+(sub_mMenu[mMenu]-6)*24, y,430+(sub_mMenu[mMenu]-6)*24 ,254);		
}

void GUI_DispCEOL_adress(void) 
{
	extern char  mMenu ,sub_mMenu[];
int x=235, y = 202;
//	if(sub_mMenu[mMenu]<=1)
GUI_ClearRect(x+sub_mMenu[mMenu]*24, y,259+sub_mMenu[mMenu]*24 ,250);
//	else if(sub_mMenu[mMenu]<=3)
//GUI_ClearRect(243+sub_mMenu[mMenu]*16, y,259+sub_mMenu[mMenu]*16 ,250);
//	else if(sub_mMenu[mMenu]<=5)
//GUI_ClearRect(251+sub_mMenu[mMenu]*16, y,267+sub_mMenu[mMenu]*16 ,250);
//	else if(sub_mMenu[mMenu]<=7)
//GUI_ClearRect(259+sub_mMenu[mMenu]*16, y,275+sub_mMenu[mMenu]*16 ,250);
//		else if(sub_mMenu[mMenu]<=9)
//GUI_ClearRect(267+sub_mMenu[mMenu]*16, y,283+sub_mMenu[mMenu]*16 ,250);
//	else 
//GUI_ClearRect(275+sub_mMenu[mMenu]*16, y,291+sub_mMenu[mMenu]*16 ,250);
}
void GUI_DispCEOL_enter(void) {
  int x=208, y =98;
  GUI_ClearRect(x, y, 795,             /* Max pos x */
                351);
}

void GUI_DispCEOL_up_debug(void)
{
	extern int pas;
  int x=223, y =98;
	if(pas==0)
	{

	 GUI_ClearRect(x, 198, 580, 198);
	 GUI_ClearRect(x, 250, 580, 250);
	 GUI_ClearRect(x, 198, x, 250);
	 GUI_ClearRect(580, 198, 580, 250);
	}
	else if(pas==1)
	{
	 GUI_ClearRect(x, 298, 580, 298);
	 GUI_ClearRect(x, 350, 580, 350);
	 GUI_ClearRect(x, 298, x, 350);
	 GUI_ClearRect(580, 298, 580, 350);
	
	}	
  else
	{
	 GUI_ClearRect(x, y, 580, y);
	 GUI_ClearRect(x, 150, 580, 150);
	 GUI_ClearRect(x, y, x, 150);
	 GUI_ClearRect(580, y, 580, 150);

	}

}

void GUI_DispCEOL_down_debug(void)
{
	extern int pas;
  int x=223, y =98;
	if(pas==0)
	{

	 GUI_ClearRect(x, 298, 580, 298);
	 GUI_ClearRect(x, 350, 580, 350);
	 GUI_ClearRect(x, 298, x, 350);
	 GUI_ClearRect(580, 298, 580, 350);
	}
	else if(pas==1)
	{
	 GUI_ClearRect(x, y, 580, y);
	 GUI_ClearRect(x, 150, 580, 150);
	 GUI_ClearRect(x, y, x, 150);
	 GUI_ClearRect(580, y, 580, 150);
	
	}	
  else
	{
	 GUI_ClearRect(x, 198, 580, 198);
	 GUI_ClearRect(x, 250, 580, 250);
	 GUI_ClearRect(x, 198, x, 250);
	 GUI_ClearRect(580, 198, 580, 250);

	}

}

void GUI_DispCEOL_up_machine(void)
{
	extern int pas;
  int x=208, y =98;
	if(pas==0)
	{

	 GUI_ClearRect(x, 198, 760, 198);
	 GUI_ClearRect(x, 250, 760, 250);
	 GUI_ClearRect(x, 198, x, 250);
	 GUI_ClearRect(760, 198, 760, 250);
	}
	else if(pas==1)
	{
	 GUI_ClearRect(x, 298, 760, 298);
	 GUI_ClearRect(x, 350, 760, 350);
	 GUI_ClearRect(x, 298, x, 350);
	 GUI_ClearRect(760, 298, 760, 350);
	
	}	
  else
	{
	 GUI_ClearRect(x, y, 760, y);
	 GUI_ClearRect(x, 150, 760, 150);
	 GUI_ClearRect(x, y, x, 150);
	 GUI_ClearRect(760, y, 760, 150);

	}

}

void GUI_DispCEOL_down_machine(void)
{
	extern int pas;
  int x=208, y =98;
	if(pas==0)
	{

	 GUI_ClearRect(x, 298, 760, 298);
	 GUI_ClearRect(x, 350, 760, 350);
	 GUI_ClearRect(x, 298, x, 350);
	 GUI_ClearRect(760, 298, 760, 350);
	}
	else if(pas==1)
	{
	 GUI_ClearRect(x, y, 760, y);
	 GUI_ClearRect(x, 150, 760, 150);
	 GUI_ClearRect(x, y, x, 150);
	 GUI_ClearRect(760, y, 760, 150);
	
	}	
  else
	{
	 GUI_ClearRect(x, 198, 760, 198);
	 GUI_ClearRect(x, 250, 760, 250);
	 GUI_ClearRect(x, 198, x, 250);
	 GUI_ClearRect(760, 198, 760, 250);

	}

}
/*************************** End of file ****************************/

/*************************** End of file ****************************/
