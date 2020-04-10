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
	delay_ms(10);//����
  if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)==0)
	{
	  Key = 'A'; 
	}
  EXTI_ClearITPendingBit(EXTI_Line0);  //���LINE0�ϵ��жϱ�־λ 
}

/* KEY 2 & KEY 4*/
void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);//����
    
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
    
    EXTI_ClearITPendingBit(EXTI_Line8);  //���LINE8�ϵ��жϱ�־λ 
    EXTI_ClearITPendingBit(EXTI_Line7);  //���LINE4�ϵ��жϱ�־λ 
}
/* KEY 3*/
//void EXTI4_IRQHandler(void)
//{
//    delay_ms(10);//����
//    if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4)==0)
//    {
//        Key = 'C'; 
//    }
//    EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ 
//}

//void EXTI3_IRQHandler(void)
//{
//	delay_ms(10);//����
//  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)==0)
//	{
//	  Key='D'; 
//	}
//  EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ 
//}

/* KEY 5*/
void EXTI15_10_IRQHandler(void)
{
    delay_ms(10);//����
    if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_15)==0)
    {
        Key = 'E'; 
    }
    EXTI_ClearITPendingBit(EXTI_Line15);  //���LINE15�ϵ��жϱ�־λ 
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
    GUI_DispStringAt("x",540,297);            //��ʾ��������

    GUI_DrawHLine(298,218+pas*32,246+pas*32);
    GUI_DrawHLine(350,218+pas*32,246+pas*32);
    GUI_DrawVLine(218+pas*32,298,350);
    GUI_DrawVLine(246+pas*32,298,350);        //�������
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
    GUI_DispStringAt("x",540,297);            //��ʾ��������

    GUI_DrawHLine(298,218+pas*32,246+pas*32);
    GUI_DrawHLine(350,218+pas*32,246+pas*32);
    GUI_DrawVLine(218+pas*32,298,350);
    GUI_DrawVLine(246+pas*32,298,350);        //�������


     GUI_DrawHLine(200,220,550);
     GUI_DrawHLine(255,220,550);
     GUI_DrawVLine(220,200,255);
     GUI_DrawVLine(550,200,255); 
}

void alter_password(void)
{
    GUI_DispStringAt("The new password:", 221, 100);     //����������
    GUI_DispStringAt("Confirm password:", 221, 200);     //ȷ��������
    GUI_SetColor(GUI_BLUE);                 //����ǰ��ɫΪ��ɫ
    GUI_DrawHLine(98,220,700);
    GUI_DrawHLine(155,220,700);
    GUI_DrawVLine(220,98,155);
    GUI_DrawVLine(700,98,155);              //���������
    GUI_SetColor(GUI_WHITE);                 //����ǰ��ɫΪ��ɫ
    GUI_DrawHLine(198,220,700);
    GUI_DrawHLine(255,220,700);
    GUI_DrawVLine(220,198,255);
    GUI_DrawVLine(700,198,255);              //ȷ�������	

}
extern uint8_t HJ_addr[6];
extern float pm2_5,CO2_ppm,Humidity,Temperature;
extern tm timer;
int times_flag=0,second,times_flag1=0;
void show_Menu(void)
{
    GUI_SetFont(&GUI_Font48ASCII);                           ///////////////////////////////////�����ε����ȵ��԰�����Ҫ����
    switch(mMenu)
	{
		case 0:                                                   ////////////////////////////////////���������ʱ����Ҫ��ʱ��error_flag=3��
//		display_contant();//��ʾ���������
		display_num();    //��ʾ�ı������
		break;

		case 1:

		switch (sub_mMenu[mMenu])
		{
			case 0:
//          	if((times_flag==0)|((times_flag==1)&&(second==second-1)))
				GUI_DispStringAt("Please enter the password:", 220, 100);           //��¼�������
				password_number();                           //��������
				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //���������	
			break;

			case 1:
				GUI_DispStringAt("Please enter the password:", 220, 100);           //�����һ�����ֺ����
				GUI_DispStringAt("*", 240, 205);
				password_number();                           //��������

				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //���������
			break;

			case 2:
				GUI_DispStringAt("Please enter the password:", 220, 100);          //����ڶ������ֺ����
				GUI_DispStringAt("* *", 240, 205);
				password_number();                           //��������

				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //���������
			break;

			case 3:
				GUI_DispStringAt("Please enter the password:", 220, 100);          //������������ֺ����
				GUI_DispStringAt("* * *", 240, 205);
				password_number();                           //��������

				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //���������
			break;

			case 4: 
				GUI_DispStringAt("Please enter the password:", 220, 100);      //������ĸ����ֺ����
				GUI_DispStringAt("* * * *", 240, 205);
				password_number();                           //��������

				GUI_DrawHLine(200,220,500);
				GUI_DrawHLine(255,220,500);
				GUI_DrawVLine(220,200,255);
				GUI_DrawVLine(500,200,255);              //���������
			break;

			case 5:
				GUI_DispStringAt("The password is error", 230, 150);            //����������
				GUI_DispStringAt("Please enter again!", 230, 250);
//			    GUI_DispStringAt("Press the identify key to return!", 285, 300);
			break;

			case 6:
				GUI_DispStringAt("Input password too much times,", 220, 150);            //���������������
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
			GUI_DispStringAt("1.Set the machine", 225, 100);             //���Խ���
			GUI_DispStringAt("2.Query the data", 225, 200);
			GUI_DispStringAt("3.Back to login", 225, 300);
			GUI_DrawHLine(98+100*pas,223,580);                                        //ѡ�����
			GUI_DrawHLine(150+100*pas,223,580);
			GUI_DrawVLine(223,98+100*pas,150+100*pas);
			GUI_DrawVLine(580,98+100*pas,150+100*pas);
		break;		

		case 3:

			GUI_DispStringAt("1.Set the machine password", 210, 100);             //�������ý���
			GUI_DispStringAt("2.Set the machine adress", 210, 200);
			GUI_DispStringAt("3.Back to the debug directory", 210, 300);
			GUI_DrawHLine(98+100*pas,208,760);                                        //ѡ�����
			GUI_DrawHLine(150+100*pas,208,760);
			GUI_DrawVLine(208,98+100*pas,150+100*pas);
			GUI_DrawVLine(760,98+100*pas,150+100*pas);
		break;

		case 4:
		switch(sub_mMenu[mMenu])
		{
			case 0:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				address_number();
			break;

			case 1:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);
				address_number();	
			break;
			case 2:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				address_number();	
			break;
			case 3:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);
				address_number();		
			break;
			case 4:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				address_number();	
			break;
			case 5:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);
				address_number();	
			break;
			case 6:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				address_number();	
			break;
			case 7:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);
				address_number();	
			break;
			case 8:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				address_number();	
			break;
			case 9:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				GUI_DispDecAt(pass1[8], 427, 202,1);
				address_number();			
			break;
			case 10:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				GUI_DispDecAt(pass1[8], 427, 202,1);GUI_DispDecAt(pass1[9], 451, 202,1);
				address_number();	
			break;
			case 11:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				GUI_DispDecAt(pass1[8], 427, 202,1);GUI_DispDecAt(pass1[9], 451, 202,1);
				GUI_DispDecAt(pass1[10], 475, 202,1);
				address_number();		
			break;
			case 12:
				GUI_DispStringAt("Please enter the address:", 225, 100);           //�����ַ����
				GUI_DispDecAt(pass1[0], 235, 202,1);GUI_DispDecAt(pass1[1], 259, 202,1);
				GUI_DispDecAt(pass1[2], 283, 202,1);GUI_DispDecAt(pass1[3], 307, 202,1);
				GUI_DispDecAt(pass1[4], 331, 202,1);GUI_DispDecAt(pass1[5], 355, 202,1);
				GUI_DispDecAt(pass1[6], 379, 202,1);GUI_DispDecAt(pass1[7], 403, 202,1);
				GUI_DispDecAt(pass1[8], 427, 202,1);GUI_DispDecAt(pass1[9], 451, 202,1);
				GUI_DispDecAt(pass1[10], 475, 202,1); GUI_DispDecAt(pass1[11], 499, 202,1);
				address_number();	
			break;

			case 13:
				GUI_DispStringAt("Set the address successfully", 225, 210);           //��ַ���óɹ�����
			///////////////////////////////////////////////////////////���ĵ�ַ

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
				GUI_DispStringAt("Set the password successfully!", 210, 210);           //�������óɹ�����
			break;
			case 10:
				GUI_DispStringAt("The password does not match!", 210, 210);           //���벻ƥ�����
			break;
			default:
			break;
		}
		break;
		case 6:
			GUI_DispStringAt("1.Query the current data", 210, 100);             //���ݲ�ѯ����
			GUI_DispStringAt("2.Query the historical data", 210, 200);
			GUI_DispStringAt("3.Back to the debug directory", 210, 300);
			GUI_DrawHLine(98+100*pas,208,760);                                        //ѡ�����
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
			GUI_DispHexAt(HJ_addr[4],622,60,2);GUI_DispHexAt(HJ_addr[5],670,60,2);///////��ʾ��ַ

			GUI_SetColor(GUI_BLUE);//����ǰ��ɫΪ��ɫ
			GUI_DrawHLine(431,769,849);
			GUI_DrawVLine(769,431,479);
			GUI_DrawVLine(849,431,479);              //�˳����		
			GUI_DispStringAt("EXIT", 770, 432);      //�˳�

			GUI_SetColor(GUI_WHITE);//����ǰ��ɫΪ��ɫ
			GUI_GotoX(430);          //X����
			GUI_GotoY(140);          //Y����
			if(0<pm2_5<10)         GUI_DispDecSpace(pm2_5,1);
			else if(pm2_5<100)     GUI_DispDecSpace(pm2_5,2);
			else                   GUI_DispDecSpace(pm2_5,3);               ////////��ʾPm2.5
			GUI_GotoX(430);          //X����
			GUI_GotoY(220);          //Y����
			if(0<CO2_ppm<10)       GUI_DispDecSpace(CO2_ppm,1);
			else if(CO2_ppm<100)   GUI_DispDecSpace(CO2_ppm,2);
			else if(CO2_ppm<1000)  GUI_DispDecSpace(CO2_ppm,3);
			else                   GUI_DispDecSpace(CO2_ppm,4);            ////////��ʾCO2

			GUI_GotoX(430);          //X����
			GUI_GotoY(300);          //Y����
			if(Temperature==0)                       //���0��
			GUI_DispFloatMin(Temperature,0);         //��ʾ����0��
			else    GUI_DispFloatMin(Temperature,1); //��ʾ��С������ֵ��       //////////��ʾ�¶�

			GUI_GotoX(430);          //X����
			GUI_GotoY(380);          //Y����
			GUI_DispFloatMin(Humidity,1);//��ʾ��С������ֵ��					           ///////��ʾʪ��
		break;

		case 8:
			switch( sub_mMenu[mMenu])
			{
				case 0:
					GUI_DispStringAt("DATE:", 220, 110);           //�������ڽ���
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);						
					address_number();
				break;

				case 1:
					GUI_DispStringAt("DATE:", 220, 110);           //�������ڽ���
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[0], 230, 205,1);               //��ʾ����
					address_number();
				break;

				case 2:
					GUI_DispStringAt("DATE:", 220, 110);           //�������ڽ���
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[1], 254, 205,1);               //��ʾ����
					address_number();
				break;

				case 3:
					GUI_DispStringAt("DATE:", 220, 110);           //�������ڽ���
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[2], 278, 205,1);          //��ʾ����
					address_number();
				break;

				case 4:
					GUI_DispStringAt("DATE:", 220, 110);           //�������ڽ���
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[3], 302, 205,1);               //��ʾ����
					address_number();
				break;

				case 5:
					GUI_DispStringAt("DATE:", 220, 110);           //�������ڽ���
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[4], 342, 205,1);               //��ʾ����
					address_number();
				break;

				case 6:
					GUI_DispStringAt("DATE:", 220, 110);           //�������ڽ���
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[5], 366, 205,1);               //��ʾ����
					address_number();
				break;

				case 7:
					GUI_DispStringAt("DATE:", 220, 110);           //�������ڽ���
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[6], 406, 205,1);               //��ʾ����
					address_number();
				break;

				case 8:
					GUI_DispStringAt("DATE:", 220, 110);           //�������ڽ���
					GUI_DispStringAt("-", 326, 205);
					GUI_DispStringAt("-", 390, 205);
					GUI_DispHexAt(date1[7], 430, 205,1);               //��ʾ����
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
int MenuCount = 0,MenuCount1 = 0,MenuCount2 = 0,MenuCount3 = 0;                  /////////////////////////////////////////////////�����ε����Ȳ��ã��ȵ��԰�����Ҫ����
	
void turn_hexadecimal(void)                                  //��������������16������
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
	unsigned int i = 0;                                     /////////////////////////////////////////////////�����ε����Ȳ��ã��ȵ��԰�����Ҫ����
    key_value_t = Key;
    
	if((mMenu == 0) && (Key != 0))    //���������а�������    
	{
		switch (Key)
		{
			case 'A':                     //���¼�1�������¼�������
			if((times_flag==0)|((times_flag==1)&&(times_flag1==1)))
			{
				mMenu=1;
				sub_mMenu[mMenu]=0;
				Key=0;
				GUI_SetColor(GUI_WHITE);        //����ǰ��ɫΪ��ɫ
				GUI_SetBkColor(GUI_beijing);    //���ñ���ɫΪ��ɫ
				GUI_Clear();                    //����
				times_flag=0;
				times_flag1=0;
			}
			break;                                               /////////////////////////////////////////////////�����ε����Ȳ��ã��ȵ��԰�����Ҫ����

			case 'C':                     //Key3����
				mMenu=0;
				GUI_Clear();//����
				GPIO_ResetBits(GPIOF, GPIO_Pin_15);	 	 //	BL = 0

				Key=0;
			break;

			case 'E':                                        //Key5����
				GPIO_SetBits(GPIOF, GPIO_Pin_15);	 	 //	BL = 1
				mMenu=0;
				Key=0;
			break;
			default:                                    //���������£���Ч
			break;
		}
	}
	                                                                          //�����ε�,�ȵ��԰�����Ҫ����
	else if((mMenu == 1)&& (Key != 0))              //�����¼����
	{
		switch (Key)
		{
			case 'B':                     //����
				pas--;              
				if(pas <0) pas = 10;
				Key=0;
				GUI_DispCEOL_leftline() ;
			break;

			case 'D':                     //����
				pas++;
				if(pas > 10) pas = 0;
				Key=0;
				GUI_DispCEOL_rightline() ;
			break;

			case 'E':                     //����������
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount=0;
				pass[15]=0;
				GUI_DispCEOL_enter();     //����

				show_main_interface();
			break;

			case'C':
				if(sub_mMenu[mMenu]==0&&pas==10)          //��¼�������ѡ��del
				{
					mMenu=1;

//					GUI_DispCEOL_confirm_num();             //�����ɾ����
//					pas=0;
					Key=0;
					break;
				}

				if((sub_mMenu[mMenu]>0)&&(pas==10))       //�����������ѡ��del
				{
					sub_mMenu[mMenu]=sub_mMenu[mMenu]-1;
					pass[MenuCount-1]=0;
					MenuCount --;
					GUI_DispCEOL_asterisk();                //�������
//					GUI_DispCEOL_confirm_num();             //�����ɾ����
//				    pas=0;
					Key=0;

					break;
				}

				pass[MenuCount] = pas;
				if(MenuCount > 3)           //�ĸ�����ȫ���������
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
							sub_mMenu[mMenu] =5;	//���������棬�Զ�ˢ��Ϊ0�������������
							Key=0;
							pas=0;
							read_dat[0]=0;
							GUI_DispCEOL_enter();
							MenuCount = 0;
							error_flag++;
							if(error_flag==3)       //����������δ������룬�������ٴν���
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

					mMenu = 2;	//������ȷ���������Ŀ¼����
					GUI_DispCEOL_enter();     	////////////////����
					Key=0;
					MenuCount = 0;
					pas=0;
					read_dat[0]=0;
					break;


			}
			if(sub_mMenu[mMenu]==5)                  //����������������棬����ȷ�ϼ��󣬷��ص������������
			{
				sub_mMenu[mMenu]=0;
				Key=0;
				MenuCount = 0;
				GUI_DispCEOL_enter();

				break;
			}
			if(sub_mMenu[mMenu]==6)                  //���������������������������棬����ȷ�ϼ��󣬷��ص�������
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
				sub_mMenu[mMenu] = MenuCount+1;           //��ʾ����������һ������
//				GUI_DispCEOL_confirm_num() ;                     //����������ֿ�
//				pas=0; 
				Key=0;					 
			}
			MenuCount++;
			break;
			default:
			break; 
		}
	}


	else if((mMenu == 2) && (Key != 0))    //����Ŀ¼�������а������� 
	{
		switch(Key)
		{
			case 'B':                     //����
				pas--;              
				if(pas <0) pas = 2;
				Key=0;
				GUI_DispCEOL_up_debug() ;    //
			break;

			case 'D':                     //����
				pas++;
				if(pas >2) pas = 0;
				Key=0;
				GUI_DispCEOL_down_debug() ;
			break;

			case 'E':                     //����������
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_DispCEOL_enter();     //����

				show_main_interface();
			break;

			case 'C':
				if(pas==0)                 //���뱾�����ý���
				{
					mMenu = 3;
					sub_mMenu[mMenu]=0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //����
				}
				else if(pas==1)           //��ѯ���ݽ���
				{
					mMenu = 6;
					sub_mMenu[mMenu]=0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //����

				}
				else                       //������һ��
				{ 
					mMenu = 1;
					sub_mMenu[mMenu]=0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //����
				}
			break;
			default:
			break; 
		}
	}			
	else if((mMenu == 3) && (Key != 0))    //�������ý������а������� 		
	{
		switch(Key)
		{
			case 'B':                     //����
				pas--;              
				if(pas <0) 
					pas = 2;
				Key=0;
				GUI_DispCEOL_up_machine() ;
			break;

			case 'D':                     //����
				pas++;
				if(pas >2) 
					pas = 0;
				Key=0;
				GUI_DispCEOL_down_machine() ;
			break;

			case 'E':                     //����������
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_DispCEOL_enter();     //����

				show_main_interface();
			break;

			case 'C':
				if(pas==2)                  //ѡ��3�����ص���Ŀ¼����
				{
					mMenu = 2;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //����
					break;
				}

				if(pas==1)                  //ѡ��2�������ַ���ý���
				{
					mMenu = 4;
					sub_mMenu[mMenu] = 0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //����
					break;
				}

				if(pas==0)                 //ѡ��1������������ý���
				{
					mMenu = 5;
					sub_mMenu[mMenu] = 0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //����
				}
			break;
			default:
			break;
		}
	}
			
	else if((mMenu == 4) && (Key != 0))    //��ַ���ý������а������� 		
	{
		switch (Key)
		{
			case 'A':                       //���ر������ý���
				mMenu = 3;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount1=0;
				pass1[15]=0;
				GUI_DispCEOL_enter();         //����
			break;

			case 'B':                        //����
				pas--;              
				if(pas <0) pas = 10;
				Key=0;
				GUI_DispCEOL_leftline() ;
			break;

			case 'D':                        //����
				pas++;
				if(pas > 10) pas = 0;
				Key=0;
				GUI_DispCEOL_rightline() ;
			break;

			case 'E':                     //����������
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount1=0;
				pass1[15]=0;
				GUI_DispCEOL_enter();     //����

				show_main_interface();
			break;

			case 'C':
				if(sub_mMenu[mMenu]==0&&pas==10)      //���ĵ�ַ����ѡ��del
				{
					mMenu=4;
//					pas=0;
//					GUI_DispCEOL_password();            //��������
					Key=0;
					break;
				}

				if((sub_mMenu[mMenu]>0)&&(pas==10))       //�����ַ����ѡ��del
				{
					sub_mMenu[mMenu]=sub_mMenu[mMenu]-1;
					pass1[MenuCount1-1]=0;
					MenuCount1 --;
					GUI_DispCEOL_adress();                //�����ַ
//					GUI_DispCEOL_confirm_num();             //�����ɾ����
//					pas=0;
					Key=0;

					break;
				}

			pass1[MenuCount1] = pas;               //���ĵ�ַ���ݴ���pass1

			if(address_flag==1|address_flag==3|address_flag==5|address_flag==7|address_flag==9|address_flag==11)//������ż��������
			{
				turn_hexadecimal();                                                                                 //��ϳ�16������
			}


			address_flag++;
			if(address_flag>11)
			{
				address_flag=0;                      //��־λ��0
			}
			if(sub_mMenu[mMenu] ==13)               //���óɹ���ȷ�ϼ������ص���һ��
			{
				mMenu=3;
				MenuCount1 = 0;
				pas=0; 
				Key=0;
				GUI_DispCEOL_enter() ; 

				break;
			} 
			if(MenuCount1 > 11)           //�ĸ�����ȫ���������
			{ 
				sub_mMenu[mMenu] =13;       //���óɹ�
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
				WriteToSD_address(read_dat,16);     //������д��sd��
				read_dat[0]=0;
				break;
			}


			else
			{
				sub_mMenu[mMenu] = MenuCount1+1;           //��ʾ�������ֺ��һ������
//		        GUI_DispCEOL_confirm_num() ;                     //����������ֿ�
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
			
	else if((mMenu == 5) && (Key != 0))    //�������ý������а������� 		
	{
		switch (Key)
		{
			case 'A':                      //���ر������ý���
				mMenu = 3;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount2=0;
				pass2[15]=0;
				GUI_DispCEOL_enter();     //����
			break;

			case 'B':                     //����
				pas--;              
				if(pas <0) 
					pas = 10;
				Key=0;
				GUI_DispCEOL_leftline() ;
			break;

			case 'D':                     //����
				pas++;
				if(pas > 10)
					pas = 0;
				Key=0;
				GUI_DispCEOL_rightline() ;
			break;

			case 'E':                     //����������
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				MenuCount2=0;
				MenuCount3 = 0;
				pass2[15]=0;
				GUI_DispCEOL_enter();     //����

				show_main_interface();
			break;

			case 'C':			
				if(sub_mMenu[mMenu]==0&&pas==10)      //�����������ѡ��del
				{
					mMenu=5;
//					pas=0;
//					GUI_DispCEOL_password();            //��������
					Key=0;
					break;
				}

				if((sub_mMenu[mMenu]>0)&&(pas==10))       //�����������ѡ��del
				{
				sub_mMenu[mMenu]=sub_mMenu[mMenu]-1;
				pass2[MenuCount2-1]=0;
				MenuCount2 --;
				//					pas=0;
				Key=0;
				GUI_DispCEOL_password();            //��������
				break;
				}

				if(sub_mMenu[mMenu]==9)      //������ĳɹ����ر������ý���
				{
				mMenu=3;
				Key=0;
				MenuCount2 = 0;
				MenuCount3 = 0;
				pas=0;
				GUI_DispCEOL_enter();
				break;
				}
				if(sub_mMenu[mMenu]==10)   //�������ʧ�ܷ������¸����������
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
				pass2[MenuCount2] = pas;             //�����������ݴ���pass2

				if(MenuCount2 > 7)           //ȷ�������ĸ�����ȫ���������
				{  
					if(i<4)
					{
						for(i=0;i<4;i++)
						{
							if(pass2[i] != pass2[i+4]) 
								break; //����ȷ���Ƿ���ȷ
						}
						if(i<4)
						{
							sub_mMenu[mMenu] =10;	//����ڶ�����������Զ�ˢ��Ϊ0�������������
							Key=0;
							pas=0;
							GUI_DispCEOL_enter();
							return;
						}
					}

					sub_mMenu[mMenu]= 9;	       //������ĳɹ���
					GUI_DispCEOL_enter();     	 //����
					Key=0;
					pas=0;
					read_dat[0]=pass2[0]+0x30;
					read_dat[1]=pass2[1]+0x30;
					read_dat[2]=pass2[2]+0x30;
					read_dat[3]=pass2[3]+0x30;
					WriteToSD_password(read_dat,16);     //������д��sd��
					read_dat[0]=0;
					break;
				}
				else
				{
					sub_mMenu[mMenu] = MenuCount2+1;           //��ʾ�������ֺ��һ������
//		   		    GUI_DispCEOL_confirm_num() ;                     //����������ֿ�
//					pas=0; 
					Key=0;
				}
				MenuCount2++;
			break;
			default:
			break;	 
		}
	}		 
	
	else if((mMenu == 6) && (Key != 0))    //���ݲ�ѯ�������а�������	
	{
		switch(Key)
		{
			case 'B':                     //����
				pas--;              
				if(pas <0) 
					pas = 2;
				Key=0;
				GUI_DispCEOL_up_machine() ;
			break;

			case 'D':                     //����
				pas++;
				if(pas >2) 
					pas = 0;
				Key=0;
				GUI_DispCEOL_down_machine() ;
			break;

			case 'E':                     //����������
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_DispCEOL_enter();     //����

				show_main_interface();
			break;

			case 'C':
				if(pas==2)                 //ѡ��3�����ص���Ŀ¼����
				{
					mMenu = 2;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //����
					break;
				}

				if(pas==1)                 //ѡ��2����ѯ��ʷ���ݽ���
				{
					mMenu =8;
					sub_mMenu[mMenu] = 0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //����
					break;
				}

				if(pas==0)                  //ѡ��1����ѯ��ǰ���ݽ���
				{
					mMenu =7;
					sub_mMenu[mMenu] = 0;
					pas=0;
					Key=0;
					GUI_DispCEOL_enter();     //����
				}
			break;
			default:
			break;
		}

	}
	else if((mMenu == 7) && (Key != 0))    //��ǰ���ݽ������а�������	
	{
		switch(Key)
		{
			case 'E':                     //����������
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_Clear();//����

				show_main_interface();
			break;

			case 'C':
				mMenu = 6;                //���ز�ѯ���ݽ���
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				GUI_Clear();//����
			break;
			default:
			break;
		}
	}	
			
	else if((mMenu == 8) && (Key != 0))    //��ʷ���ݽ������а�������
	{
		switch(Key)
		{
			case 'A':                      //�������ݲ�ѯ����
				mMenu = 6;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				date1[7]=0;
				dateCount=0;
				GUI_Clear();//����
			break;

			case 'B':                     //����
				pas--;              
				if(pas <0) 
					pas = 10;
				Key=0;
				GUI_DispCEOL_leftline() ;
			break;

			case 'D':                     //����
				pas++;
				if(pas > 10) 
					pas = 0;
				Key=0;
				GUI_DispCEOL_rightline() ;
			break;

			case 'E':                     //����������
				mMenu = 0;
				sub_mMenu[mMenu] = 0;
				pas=0;
				Key=0;
				dateCount=0;
				date1[7]=0;
				GUI_Clear();//����

				show_main_interface();
			break;

			case 'C':
				if(sub_mMenu[mMenu]==0&&pas==10)      //��������ѡ��del
				{
					mMenu=8;
//					pas=0;
//					GUI_DispCEOL_password();            //��������
					Key=0;
					break;
				}

				if((sub_mMenu[mMenu]>0)&&(pas==10))       //�����ַ����ѡ��del
				{
					sub_mMenu[mMenu]=sub_mMenu[mMenu]-1;
					date1[dateCount-1]=0;                   //���д�������
					dateCount--;                            //���ָ���-1
					GUI_DispCEOL_date();                    //�����������
//					GUI_DispCEOL_confirm_num();             //�����ɾ����
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
					GUI_Clear();//����
					break;
				}

				if(dateCount > 7)                         //�����������
				{  
					sub_mMenu[mMenu] =9;                    //��ʷ������ʾ����
					f=date1[4]*10+date1[5];                 //��
					k=date1[6]*10+date1[7];                 //��
					g=date1[0]*1000+date1[1]*100+date1[2]*10+date1[3];
					pas=0; 
					Key=0;
					GUI_DispCEOL_enter() ; 
					break;
				}


				date1[dateCount]=pas;
				sub_mMenu[mMenu] = dateCount+1;                   //��ʾ�������ֺ��һ������
//				GUI_DispCEOL_confirm_num() ;                      //����������ֿ�
				Key=0;
//				pas=0;
				dateCount++;
			break;
			default:
			break;
		}
	}			
}	
			
