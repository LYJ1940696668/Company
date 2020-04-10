#include "stm32f10x.h"
#include "gui.h"
#include "calendar.h"
#include "RTC.h"
#include "mstimer.h"
#include "Tiky_LCD.h" 
#include "display.h"
#include "SHT2x.h"
#include "usart1.h"
#include "usart3.h"
#include "Protol_3762.h"
#include "interface_disp.h"
#include "led.h"
#include "key.h"
#include "turn_ASCII.h"
//#include "sdio_sdcard.h"
#include "sd_spi.h"



static	unsigned char Key = 0;


/* KEY 1*/
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//消抖
  if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)==0)
	{
	  Key = 'A'; 
	}
  EXTI_ClearITPendingBit(EXTI_Line0);  //清除LINE0上的中断标志位 
}

/* KEY 2 & KEY 4*/
void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);//消抖
    
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==0)
    {
        Key = 'B'; 
    }
    else if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8)==0)
    {
        Key = 'D'; 
	}
    
    
    /*key 3*/
    if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7)==0)
    {
        Key = 'C'; 
    }
    
    EXTI_ClearITPendingBit(EXTI_Line8);  //清除LINE8上的中断标志位 
    EXTI_ClearITPendingBit(EXTI_Line7);  //清除LINE4上的中断标志位 
}
/* KEY 3*/
//void EXTI4_IRQHandler(void)
//{
//    delay_ms(10);//消抖
//    if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4)==0)
//    {
//        Key = 'C'; 
//    }
//    EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位 
//}

//void EXTI3_IRQHandler(void)
//{
//	delay_ms(10);//消抖
//  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)==0)
//	{
//	  Key='D'; 
//	}
//  EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位 
//}

/* KEY 5*/
void EXTI15_10_IRQHandler(void)
{
    delay_ms(10);//消抖
    if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_15)==0)
    {
        Key = 'E'; 
    }
    EXTI_ClearITPendingBit(EXTI_Line15);  //清除LINE15上的中断标志位 
}
char mMenu = 0;
char sub_mMenu[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char number[10]={0,1,2,3,4,5,6,7,8,9};
char pass[16],pass1[16],pass2[16],pass3[16],date1[8];
int pas=0,dateCount=0; 

void password_number(void)
{
    GUI_DispStringAt("0",220,300);
    GUI_DispStringAt("1",252,300);
    GUI_DispStringAt("2",284,300);
    GUI_DispStringAt("3",316,300);
    GUI_DispStringAt("4",348,300);
    GUI_DispStringAt("5",380,300);
    GUI_DispStringAt("6",412,300);
    GUI_DispStringAt("7",444,300);
    GUI_DispStringAt("8",476,300);
    GUI_DispStringAt("9",508,300);
    GUI_DispStringAt("x",540,297);            //显示密码数字

    GUI_DrawHLine(298,218+pas*32,246+pas*32);
    GUI_DrawHLine(350,218+pas*32,246+pas*32);
    GUI_DrawVLine(218+pas*32,298,350);
    GUI_DrawVLine(246+pas*32,298,350);        //数字外框
}

void address_number(void)
{
    GUI_DispStringAt("0",220,300);
    GUI_DispStringAt("1",252,300);
    GUI_DispStringAt("2",284,300);
    GUI_DispStringAt("3",316,300);
    GUI_DispStringAt("4",348,300);
    GUI_DispStringAt("5",380,300);
    GUI_DispStringAt("6",412,300);
    GUI_DispStringAt("7",444,300);
    GUI_DispStringAt("8",476,300);
    GUI_DispStringAt("9",508,300);
    GUI_DispStringAt("x",540,297);            //显示密码数字

    GUI_DrawHLine(298,218+pas*32,246+pas*32);
    GUI_DrawHLine(350,218+pas*32,246+pas*32);
    GUI_DrawVLine(218+pas*32,298,350);
    GUI_DrawVLine(246+pas*32,298,350);        //数字外框


     GUI_DrawHLine(200,220,550);
     GUI_DrawHLine(255,220,550);
     GUI_DrawVLine(220,200,255);
     GUI_DrawVLine(550,200,255); 
}

void alter_password(void)
{
    GUI_DispStringAt("The new password:", 221, 100);     //输入新密码
    GUI_DispStringAt("Confirm password:", 221, 200);     //确认新密码
    GUI_SetColor(GUI_BLUE);                 //设置前景色为红色
    GUI_DrawHLine(98,220,700);
    GUI_DrawHLine(155,220,700);
    GUI_DrawVLine(220,98,155);
    GUI_DrawVLine(700,98,155);              //输入密码框
    GUI_SetColor(GUI_WHITE);                 //设置前景色为白色
    GUI_DrawHLine(198,220,700);
    GUI_DrawHLine(255,220,700);
    GUI_DrawVLine(220,198,255);
    GUI_DrawVLine(700,198,255);              //确认密码框	

}
extern uint8_t HJ_addr[6];
extern float pm2_5,CO2_ppm,Humidity,Temperature;
extern tm timer;
int times_flag=0,second,times_flag1=0;
void show_Menu(void)
{
    GUI_SetFont(&GUI_Font48ASCII);                           ///////////////////////////////////先屏蔽掉，等调试按键需要加上
    switch(mMenu)
	{
		case 0:                                                   ////////////////////////////////////当输入错误时，需要定时（error_flag=3）
//		display_contant();//显示不变的内容
		display_num();    //显示改变的内容
		break;

		case 1:

		switch (sub_mMenu[mMenu])
		{
			case 0:
//          	if((times_flag==0)|((times_flag==1)&&(second==second-1)))
				GUI_DispStringAt("Please enter the password:", 220, 100);           //登录密码界面
				password_number();                           //输入数字
				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //输入密码框	
			break;

			case 1:
				GUI_DispStringAt("Please enter the password:", 220, 100);           //输入第一个数字后界面
				GUI_DispStringAt("*", 240, 205);
				password_number();                           //输入数字

				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //输入密码框
			break;

			case 2:
				GUI_DispStringAt("Please enter the password:", 220, 100);          //输入第二个数字后界面
				GUI_DispStringAt("* *", 240, 205);
				password_number();                           //输入数字

				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //输入密码框
			break;

			case 3:
				GUI_DispStringAt("Please enter the password:", 220, 100);          //输入第三个数字后界面
				GUI_DispStringAt("* * *", 240, 205);
				password_number();                           //输入数字

				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //输入密码框
			break;

			case 4: 
				GUI_DispStringAt("Please enter the password:", 220, 100);      //输入第四个数字后界面
				GUI_DispStringAt("* * * *", 240, 205);
				password_number();                           //输入数字

				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //输入密码框
			break;

			case 5:
				GUI_DispStringAt("The password is error", 230, 150);            //密码错误界面
				GUI_DispStringAt("Please enter again!", 230, 250);
//			    GUI_DispStringAt("Press the identify key to return!", 285, 300);
			break;

			case 6:
				GUI_DispStringAt("Input password too much times,", 220, 150);            //密码输入次数过多
				GUI_DispStringAt("please try again later!", 220, 250);
				times_flag=1;
				if(timer.sec>0)
				{
					second=timer.sec;
				}
				else
				{
					second=59;
				}

			break;
			default:
			break;
		}

		break;

		case 2:
			GUI_DispStringAt("1.Set the machine", 225, 100);             //调试界面
			GUI_DispStringAt("2.Query the data", 225, 200);
			GUI_DispStringAt("3.Back to login", 225, 300);
			GUI_DrawHLine(98+100*pas,223,580);                                        //选择外框
			GUI_DrawHLine(150+100*pas,223,580);
			GUI_DrawVLine(223,98+100*pas,150+100*pas);
			GUI_DrawVLine(580,98+100*pas,150+100*pas);
		break;		

		case 3:

			GUI_DispStringAt("1.Set the machine password", 210, 100);             //本机设置界面
			GUI_DispStringAt("2.Set the machine adress", 210, 200);
			GUI_DispStringAt("3.Back to the debug directory", 210, 300);
			GUI_DrawHLine(98+100*pas,208,760);                                        //选择外框
			GUI_DrawHLine(150+100*pas,208,760);
			GUI_DrawVLine(208,98+100*pas,150+100*pas);
			GUI_DrawVLine(760,98+100*pas,150+100*pas);
		break;

		case 4:
		switch(sub_mMenu[mMenu])
		{
			case 0:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				address_number();
			break;

			case 1:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);
				address_number();	
			break;
			case 2:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				address_number();	
			break;
			case 3:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);
				address_number();		
			break;
			case 4:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				address_number();	
			break;
			case 5:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);
				address_number();	
			break;
			case 6:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				address_number();	
			break;
			case 7:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);
				address_number();	
			break;
			case 8:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				address_number();	
			break;
			case 9:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				GUI_DispDecAt(pass1[8], 427, 202,1);
				address_number();			
			break;
			case 10:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				GUI_DispDecAt(pass1[8], 427, 202,1);GUI_DispDecAt(pass1[9], 451, 202,1);
				address_number();	
			break;
			case 11:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				GUI_DispDecAt(pass1[8], 427, 202,1);GUI_DispDecAt(pass1[9], 451, 202,1);
				GUI_DispDecAt(pass1[10], 475, 202,1);
				address_number();		
			break;
			case 12:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //输入地址界面
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				GUI_DispDecAt(pass1[8], 427, 202,1);GUI_DispDecAt(pass1[9], 451, 202,1);
				GUI_DispDecAt(pass1[10], 475, 202,1); GUI_DispDecAt(pass1[11], 499, 202,1);
				address_number();	
			break;

			case 13:
				GUI_DispStringAt("Set the address successfully", 225, 210);           //地址设置成功界面
			///////////////////////////////////////////////////////////更改地址

		break;
		default:
		break;
		}
		break;

		case 5:
		switch( sub_mMenu[mMenu])
		{
			case 0:	
				alter_password();				
				password_number();
			break;

			case 1:
				GUI_DispStringAt("*", 570, 105);  				
				alter_password();
				password_number();
			break;
			case 2:
				GUI_DispStringAt("* *", 570, 105);  				
				alter_password();
				password_number();
			break;
			case 3:
				GUI_DispStringAt("* * *", 570, 105);  			
				alter_password();
				password_number();
			break;
			case 4:
				GUI_DispStringAt("* * * *", 570, 105);  					
				alter_password();
				password_number();				
			break;
			case 5:
				GUI_DispStringAt("* * * *", 570, 105);  
				GUI_DispStringAt("*", 570, 205);
				alter_password();
				password_number();
			break;
			case 6:
				GUI_DispStringAt("* * * *", 570, 105);  
				GUI_DispStringAt("* *", 570, 205);
				alter_password();
				password_number(); 
			break;
			case 7:
				GUI_DispStringAt("* * * *", 570, 105);  
				GUI_DispStringAt("* * *", 570, 205);  
				alter_password();
				password_number();
			break;
			case 8:
				GUI_DispStringAt("* * * *", 570, 105);  
				GUI_DispStringAt("* * * *", 570, 205);
				alter_password();
				password_number();
			break;
			case 9:
				GUI_DispStringAt("Set the password successfully!", 210, 210);           //密码设置成功界面
			break;
			case 10:
				GUI_DispStringAt("The password does not match!", 210, 210);           //密码不匹配界面
			break;
			default:
			break;
		}
		break;
		case 6:
			GUI_DispStringAt("1.Query the current data", 210, 100);             //数据查询界面
			GUI_DispStringAt("2.Query the historical data", 210, 200);
			GUI_DispStringAt("3.Back to the debug directory", 210, 300);
			GUI_DrawHLine(98+100*pas,208,760);                                        //选择外框
			GUI_DrawHLine(150+100*pas,208,760);
			GUI_DrawVLine(208,98+100*pas,150+100*pas);
			GUI_DrawVLine(760,98+100*pas,150+100*pas);
		break;

		case 7:
			GUI_DispStringAt("The adress:",210,60);
			GUI_DispStringAt("PM2.5:",210,140);
			GUI_DispStringAt("CO2:",210,220);
			GUI_DispStringAt("H:",210,300);
			GUI_DispStringAt("T:",210,380);
			GUI_DispHexAt(HJ_addr[0],430,60,2);GUI_DispHexAt(HJ_addr[1],478,60,2);
			GUI_DispHexAt(HJ_addr[2],526,60,2);GUI_DispHexAt(HJ_addr[3],574,60,2);
			GUI_DispHexAt(HJ_addr[4],622,60,2);GUI_DispHexAt(HJ_addr[5],670,60,2);///////显示地址

			GUI_SetColor(GUI_BLUE);//设置前景色为白色
			GUI_DrawHLine(431,769,849);
			GUI_DrawVLine(769,431,479);
			GUI_DrawVLine(849,431,479);              //退出外框		
			GUI_DispStringAt("EXIT", 770, 432);      //退出

			GUI_SetColor(GUI_WHITE);//设置前景色为白色
			GUI_GotoX(430);          //X坐标
			GUI_GotoY(140);          //Y坐标
			if(0<pm2_5<10)         GUI_DispDecSpace(pm2_5,1);
			else if(pm2_5<100)     GUI_DispDecSpace(pm2_5,2);
			else                   GUI_DispDecSpace(pm2_5,3);               ////////显示Pm2.5
			GUI_GotoX(430);          //X坐标
			GUI_GotoY(220);          //Y坐标
			if(0<CO2_ppm<10)       GUI_DispDecSpace(CO2_ppm,1);
			else if(CO2_ppm<100)   GUI_DispDecSpace(CO2_ppm,2);
			else if(CO2_ppm<1000)  GUI_DispDecSpace(CO2_ppm,3);
			else                   GUI_DispDecSpace(CO2_ppm,4);            ////////显示CO2

			GUI_GotoX(430);          //X坐标
			GUI_GotoY(300);          //Y坐标
			if(Temperature==0)                       //如果0℃
			GUI_DispFloatMin(Temperature,0);         //显示整数0；
			else    GUI_DispFloatMin(Temperature,1); //显示带小数的数值；       //////////显示温度

			GUI_GotoX(430);          //X坐标
			GUI_GotoY(380);          //Y坐标
			GUI_DispFloatMin(Humidity,1);//显示带小数的数值；					           ///////显示湿度
		break;

		case 8:
			switch( sub_mMenu[mMenu])
			{
				case 0:
					GUI_DispStringAt("DATE:", 220, 110);           //输入日期界面
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);						
					address_number();
				break;

				case 1:
					GUI_DispStringAt("DATE:", 220, 110);           //输入日期界面
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[0], 230, 205,1);               //显示日期
					address_number();
				break;

				case 2:
					GUI_DispStringAt("DATE:", 220, 110);           //输入日期界面
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[1], 254, 205,1);               //显示日期
					address_number();
				break;

				case 3:
					GUI_DispStringAt("DATE:", 220, 110);           //输入日期界面
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[2], 278, 205,1);          //显示日期
					address_number();
				break;

				case 4:
					GUI_DispStringAt("DATE:", 220, 110);           //输入日期界面
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[3], 302, 205,1);               //显示日期
					address_number();
				break;

				case 5:
					GUI_DispStringAt("DATE:", 220, 110);           //输入日期界面
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[4], 342, 205,1);               //显示日期
					address_number();
				break;

				case 6:
					GUI_DispStringAt("DATE:", 220, 110);           //输入日期界面
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[5], 366, 205,1);               //显示日期
					address_number();
				break;

				case 7:
					GUI_DispStringAt("DATE:", 220, 110);           //输入日期界面
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[6], 406, 205,1);               //显示日期
					address_number();
				break;

				case 8:
					GUI_DispStringAt("DATE:", 220, 110);           //输入日期界面
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[7], 430, 205,1);               //显示日期
					address_number();
				break;

				case 9:
					read_data();
				break;
				default:
				break;
			}
		break;
		default:
		break;
	}

}

int error_flag=0,address_flag=0;
int MenuCount = 0,MenuCount1 = 0,MenuCount2 = 0,MenuCount3 = 0;                  /////////////////////////////////////////////////先屏蔽掉，先不用，等调试按键需要加上
	
void turn_hexadecimal(void)                                  //将输入的数字组成16进制数
{
	int i=0;

	for(i=0;i<11;i=i+2)
	{
		pass3[i]=pass1[i]<<4;
		pass3[i+1]=pass3[i]+pass1[i+1];
		HJ_addr[i-i/2]=pass3[i+1];
	}
}
int f,k,g,times;
extern char read_dat[44];

u8 key_value_t = 0;
void disp(void)
{
	unsigned int i = 0;                                     /////////////////////////////////////////////////先屏蔽掉，先不用，等调试按键需要加上
    key_value_t = Key;
    
	if((mMenu == 0) && (Key != 0))    //主界面且有按键按下    
	{
		switch (Key)
		{
			case 'A':                     //按下键1，进入登录密码界面
			if((times_flag==0)|((times_flag==1)&&(times_flag1==1)))
			{
				mMenu=1;
				sub_mMenu[mMenu]=0;
				Key=0;
				GUI_SetColor(GUI_WHITE);        //设置前景色为白色
				GUI_SetBkColor(GUI_beijing);    //设置背景色为黑色
				GUI_Clear();                    //清屏
				times_flag=0;
				times_flag1=0;
			}
			break;                                               /////////////////////////////////////////////////先屏蔽掉，先不用，等调试按键需要加上

			case 'C':                     //Key3按下
				mMenu=0;
				GUI_Clear();//清屏
				GPIO_ResetBits(GPIOF, GPIO_Pin_15);	 	 //	BL = 0

				Key=0;
			break;

			case 'E':                                        //Key5按下
				GPIO_SetBits(GPIOF, GPIO_Pin_15);	 	 //	BL = 1
				mMenu=0;
				Key=0;
			break;
			default:                                    //其他键按下，无效
			break;
		}
	}
	                                                                          //先屏蔽掉,等调试按键需要加上
	else if((mMenu == 1)&& (Key != 0))              //密码登录界面
	{
		switch (Key)
		{
			case 'B':                     //左移
				pas--;              
				if(pas <0) pas = 10;
				Key=0;
				GUI_DispCEOL_leftline() ;
			break;

			case 'D':                     //右移
				pas++;
				if(pas > 10) pas = 0;
				Key=0;
				GUI_DispCEOL_rightline() ;
			break;

			case 'E':                     //返回主界面
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount=0;
				pass[15]=0;
				GUI_DispCEOL_enter();     //清屏

				show_main_interface();
			break;

			case'C':
				if(sub_mMenu[mMenu]==0&&pas==10)          //登录密码界面选择del
				{
					mMenu=1;

//					GUI_DispCEOL_confirm_num();             //清除“删除”
//					pas=0;
					Key=0;
					break;
				}

				if((sub_mMenu[mMenu]>0)&&(pas==10))       //输入密码界面选择del
				{
					sub_mMenu[mMenu]=sub_mMenu[mMenu]-1;
					pass[MenuCount-1]=0;
					MenuCount --;
					GUI_DispCEOL_asterisk();                //清除密码
//					GUI_DispCEOL_confirm_num();             //清除“删除”
//				    pas=0;
					Key=0;

					break;
				}

				pass[MenuCount] = pas;
				if(MenuCount > 3)           //四个数字全部输入完毕
				{  
					if(i<4)
					{
						for(i=0;i<4;i++)
						{
							ReadFromSD_password(read_dat,16);
							if((pass[i]+0x30) != read_dat[i]) 
								break;
						}
						if(i<4)
						{
							sub_mMenu[mMenu] =5;	//密码错误界面，自动刷新为0，输入密码界面
							Key=0;
							pas=0;
							read_dat[0]=0;
							GUI_DispCEOL_enter();
							MenuCount = 0;
							error_flag++;
							if(error_flag==3)       //如果输入三次错误密码，返回需再次进入
							{
								sub_mMenu[mMenu] =6;
								Key=0;
								pas=0;
								GUI_DispCEOL_enter();
								MenuCount = 0;
								error_flag=0;
							}
							return;
						}
					}

					mMenu = 2;	//密码正确，进入调试目录界面
					GUI_DispCEOL_enter();     	////////////////清屏
					Key=0;
					MenuCount = 0;
					pas=0;
					read_dat[0]=0;
					break;


			}
			if(sub_mMenu[mMenu]==5)                  //如果出现密码错误界面，按下确认键后，返回到密码输入界面
			{
				sub_mMenu[mMenu]=0;
				Key=0;
				MenuCount = 0;
				GUI_DispCEOL_enter();

				break;
			}
			if(sub_mMenu[mMenu]==6)                  //如果出现输入密码错误次数过多界面，按下确认键后，返回到主界面
			{
				mMenu=0;
				sub_mMenu[mMenu]=0;
				Key=0;
				MenuCount = 0;
				GUI_DispCEOL_enter();

				break;

			}		 

			else
			{
				sub_mMenu[mMenu] = MenuCount+1;           //显示输入密码后的一个界面
//				GUI_DispCEOL_confirm_num() ;                     //清第三行数字框
//				pas=0; 
				Key=0;					 
			}
			MenuCount++;
			break;
			default:
			break; 
		}
	}


	else if((mMenu == 2) && (Key != 0))    //调试目录界面且有按键按下 
	{
		switch(Key)
		{
			case 'B':                     //上移
				pas--;              
				if(pas <0) pas = 2;
				Key=0;
				GUI_DispCEOL_up_debug() ;    //
			break;

			case 'D':                     //下移
				pas++;
				if(pas >2) pas = 0;
				Key=0;
				GUI_DispCEOL_down_debug() ;
			break;

			case 'E':                     //返回主界面
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_DispCEOL_enter();     //清屏

				show_main_interface();
			break;

			case 'C':
				if(pas==0)                 //进入本机设置界面
				{
					mMenu = 3;
					sub_mMenu[mMenu]=0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //清屏
				}
				else if(pas==1)           //查询数据界面
				{
					mMenu = 6;
					sub_mMenu[mMenu]=0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //清屏

				}
				else                       //返回上一级
				{ 
					mMenu = 1;
					sub_mMenu[mMenu]=0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //清屏
				}
			break;
			default:
			break; 
		}
	}			
	else if((mMenu == 3) && (Key != 0))    //本机设置界面且有按键按下 		
	{
		switch(Key)
		{
			case 'B':                     //上移
				pas--;              
				if(pas <0) 
					pas = 2;
				Key=0;
				GUI_DispCEOL_up_machine() ;
			break;

			case 'D':                     //下移
				pas++;
				if(pas >2) 
					pas = 0;
				Key=0;
				GUI_DispCEOL_down_machine() ;
			break;

			case 'E':                     //返回主界面
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_DispCEOL_enter();     //清屏

				show_main_interface();
			break;

			case 'C':
				if(pas==2)                  //选中3，返回调试目录界面
				{
					mMenu = 2;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //清屏
					break;
				}

				if(pas==1)                  //选中2，进入地址设置界面
				{
					mMenu = 4;
					sub_mMenu[mMenu] = 0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //清屏
					break;
				}

				if(pas==0)                 //选中1，密码更改设置界面
				{
					mMenu = 5;
					sub_mMenu[mMenu] = 0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //清屏
				}
			break;
			default:
			break;
		}
	}
			
	else if((mMenu == 4) && (Key != 0))    //地址设置界面且有按键按下 		
	{
		switch (Key)
		{
			case 'A':                       //返回本机设置界面
				mMenu = 3;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount1=0;
				pass1[15]=0;
				GUI_DispCEOL_enter();         //清屏
			break;

			case 'B':                        //左移
				pas--;              
				if(pas <0) pas = 10;
				Key=0;
				GUI_DispCEOL_leftline() ;
			break;

			case 'D':                        //右移
				pas++;
				if(pas > 10) pas = 0;
				Key=0;
				GUI_DispCEOL_rightline() ;
			break;

			case 'E':                     //返回主界面
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount1=0;
				pass1[15]=0;
				GUI_DispCEOL_enter();     //清屏

				show_main_interface();
			break;

			case 'C':
				if(sub_mMenu[mMenu]==0&&pas==10)      //更改地址界面选择del
				{
					mMenu=4;
//					pas=0;
//					GUI_DispCEOL_password();            //二、三行
					Key=0;
					break;
				}

				if((sub_mMenu[mMenu]>0)&&(pas==10))       //输入地址界面选择del
				{
					sub_mMenu[mMenu]=sub_mMenu[mMenu]-1;
					pass1[MenuCount1-1]=0;
					MenuCount1 --;
					GUI_DispCEOL_adress();                //清除地址
//					GUI_DispCEOL_confirm_num();             //清除“删除”
//					pas=0;
					Key=0;

					break;
				}

			pass1[MenuCount1] = pas;               //更改地址数据储存pass1

			if(address_flag==1|address_flag==3|address_flag==5|address_flag==7|address_flag==9|address_flag==11)//当输入偶数个数字
			{
				turn_hexadecimal();                                                                                 //组合成16进制数
			}


			address_flag++;
			if(address_flag>11)
			{
				address_flag=0;                      //标志位清0
			}
			if(sub_mMenu[mMenu] ==13)               //设置成功后按确认键，返回到上一级
			{
				mMenu=3;
				MenuCount1 = 0;
				pas=0; 
				Key=0;
				GUI_DispCEOL_enter() ; 

				break;
			} 
			if(MenuCount1 > 11)           //四个数字全部输入完毕
			{ 
				sub_mMenu[mMenu] =13;       //设置成功
				pas=0; 
				Key=0;
				GUI_DispCEOL_enter() ; 
				read_dat[0]=pass1[0]+0x30;
				read_dat[1]=pass1[1]+0x30;
				read_dat[2]=pass1[2]+0x30;
				read_dat[3]=pass1[3]+0x30;
				read_dat[4]=pass1[4]+0x30;
				read_dat[5]=pass1[5]+0x30;
				read_dat[6]=pass1[6]+0x30;
				read_dat[7]=pass1[7]+0x30;
				read_dat[8]=pass1[8]+0x30;
				read_dat[9]=pass1[9]+0x30;
				read_dat[10]=pass1[10]+0x30;
				read_dat[11]=pass1[11]+0x30;
				WriteToSD_address(read_dat,16);     //新密码写入sd卡
				read_dat[0]=0;
				break;
			}


			else
			{
				sub_mMenu[mMenu] = MenuCount1+1;           //显示输入数字后的一个界面
//		        GUI_DispCEOL_confirm_num() ;                     //清第三行数字框
//				pas=0; 
				Key=0;
//				break;
			}
			MenuCount1++;
			break;
			default:
			break;
		}		
	}			
			
	else if((mMenu == 5) && (Key != 0))    //密码设置界面且有按键按下 		
	{
		switch (Key)
		{
			case 'A':                      //返回本机设置界面
				mMenu = 3;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount2=0;
				pass2[15]=0;
				GUI_DispCEOL_enter();     //清屏
			break;

			case 'B':                     //上移
				pas--;              
				if(pas <0) 
					pas = 10;
				Key=0;
				GUI_DispCEOL_leftline() ;
			break;

			case 'D':                     //右移
				pas++;
				if(pas > 10)
					pas = 0;
				Key=0;
				GUI_DispCEOL_rightline() ;
			break;

			case 'E':                     //返回主界面
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount2=0;
				MenuCount3 = 0;
				pass2[15]=0;
				GUI_DispCEOL_enter();     //清屏

				show_main_interface();
			break;

			case 'C':			
				if(sub_mMenu[mMenu]==0&&pas==10)      //更改密码界面选择del
				{
					mMenu=5;
//					pas=0;
//					GUI_DispCEOL_password();            //二、三行
					Key=0;
					break;
				}

				if((sub_mMenu[mMenu]>0)&&(pas==10))       //输入密码界面选择del
				{
				sub_mMenu[mMenu]=sub_mMenu[mMenu]-1;
				pass2[MenuCount2-1]=0;
				MenuCount2 --;
				//					pas=0;
				Key=0;
				GUI_DispCEOL_password();            //二、三行
				break;
				}

				if(sub_mMenu[mMenu]==9)      //密码更改成功返回本机设置界面
				{
				mMenu=3;
				Key=0;
				MenuCount2 = 0;
				MenuCount3 = 0;
				pas=0;
				GUI_DispCEOL_enter();
				break;
				}
				if(sub_mMenu[mMenu]==10)   //密码更改失败返回重新更改密码界面
				{
				mMenu=5;
				sub_mMenu[mMenu]=0;
				pas=0;
				Key=0;
				MenuCount2 = 0;
				MenuCount3 = 0;
				GUI_DispCEOL_enter();
				break;
				}
				pass2[MenuCount2] = pas;             //更改密码数据储存pass2

				if(MenuCount2 > 7)           //确认密码四个数字全部输入完毕
				{  
					if(i<4)
					{
						for(i=0;i<4;i++)
						{
							if(pass2[i] != pass2[i+4]) 
								break; //密码确认是否正确
						}
						if(i<4)
						{
							sub_mMenu[mMenu] =10;	//密码第二次输入错误，自动刷新为0，输入密码界面
							Key=0;
							pas=0;
							GUI_DispCEOL_enter();
							return;
						}
					}

					sub_mMenu[mMenu]= 9;	       //密码更改成功，
					GUI_DispCEOL_enter();     	 //清屏
					Key=0;
					pas=0;
					read_dat[0]=pass2[0]+0x30;
					read_dat[1]=pass2[1]+0x30;
					read_dat[2]=pass2[2]+0x30;
					read_dat[3]=pass2[3]+0x30;
					WriteToSD_password(read_dat,16);     //新密码写入sd卡
					read_dat[0]=0;
					break;
				}
				else
				{
					sub_mMenu[mMenu] = MenuCount2+1;           //显示输入数字后的一个界面
//		   		    GUI_DispCEOL_confirm_num() ;                     //清第三行数字框
//					pas=0; 
					Key=0;
				}
				MenuCount2++;
			break;
			default:
			break;	 
		}
	}		 
	
	else if((mMenu == 6) && (Key != 0))    //数据查询界面且有按键按下	
	{
		switch(Key)
		{
			case 'B':                     //上移
				pas--;              
				if(pas <0) 
					pas = 2;
				Key=0;
				GUI_DispCEOL_up_machine() ;
			break;

			case 'D':                     //下移
				pas++;
				if(pas >2) 
					pas = 0;
				Key=0;
				GUI_DispCEOL_down_machine() ;
			break;

			case 'E':                     //返回主界面
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_DispCEOL_enter();     //清屏

				show_main_interface();
			break;

			case 'C':
				if(pas==2)                 //选中3，返回调试目录界面
				{
					mMenu = 2;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //清屏
					break;
				}

				if(pas==1)                 //选中2，查询历史数据界面
				{
					mMenu =8;
					sub_mMenu[mMenu] = 0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //清屏
					break;
				}

				if(pas==0)                  //选中1，查询当前数据界面
				{
					mMenu =7;
					sub_mMenu[mMenu] = 0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //清屏
				}
			break;
			default:
			break;
		}

	}
	else if((mMenu == 7) && (Key != 0))    //当前数据界面且有按键按下	
	{
		switch(Key)
		{
			case 'E':                     //返回主界面
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_Clear();//清屏

				show_main_interface();
			break;

			case 'C':
				mMenu = 6;                //返回查询数据界面
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_Clear();//清屏
			break;
			default:
			break;
		}
	}	
			
	else if((mMenu == 8) && (Key != 0))    //历史数据界面且有按键按下
	{
		switch(Key)
		{
			case 'A':                      //返回数据查询界面
				mMenu = 6;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				date1[7]=0;
				dateCount=0;
				GUI_Clear();//清屏
			break;

			case 'B':                     //左移
				pas--;              
				if(pas <0) 
					pas = 10;
				Key=0;
				GUI_DispCEOL_leftline() ;
			break;

			case 'D':                     //右移
				pas++;
				if(pas > 10) 
					pas = 0;
				Key=0;
				GUI_DispCEOL_rightline() ;
			break;

			case 'E':                     //返回主界面
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				dateCount=0;
				date1[7]=0;
				GUI_Clear();//清屏

				show_main_interface();
			break;

			case 'C':
				if(sub_mMenu[mMenu]==0&&pas==10)      //输入日期选择del
				{
					mMenu=8;
//					pas=0;
//					GUI_DispCEOL_password();            //二、三行
					Key=0;
					break;
				}

				if((sub_mMenu[mMenu]>0)&&(pas==10))       //输入地址界面选择del
				{
					sub_mMenu[mMenu]=sub_mMenu[mMenu]-1;
					date1[dateCount-1]=0;                   //清除写入的数字
					dateCount--;                            //数字个数-1
					GUI_DispCEOL_date();                    //清除日期数字
//					GUI_DispCEOL_confirm_num();             //清除“删除”
//					pas=0;
					Key=0;
					break;
				}

				if(sub_mMenu[mMenu] ==9)
				{
					sub_mMenu[mMenu] =0;
					Key=0;
					pas=0;
					dateCount=0;
					date1[7]=0;
					GUI_Clear();//清屏
					break;
				}

				if(dateCount > 7)                         //日期输入完毕
				{  
					sub_mMenu[mMenu] =9;                    //历史数据显示界面
					f=date1[4]*10+date1[5];                 //月
					k=date1[6]*10+date1[7];                 //日
					g=date1[0]*1000+date1[1]*100+date1[2]*10+date1[3];
					pas=0; 
					Key=0;
					GUI_DispCEOL_enter() ; 
					break;
				}


				date1[dateCount]=pas;
				sub_mMenu[mMenu] = dateCount+1;                   //显示输入数字后的一个界面
//				GUI_DispCEOL_confirm_num() ;                      //清第三行数字框
				Key=0;
//				pas=0;
				dateCount++;
			break;
			default:
			break;
		}
	}			
}	
			
