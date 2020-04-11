#include "stm32f10x.h"
#include "turn_ASCII.h"
#include "usart2.h"
//#include "sdio_sdcard.h"
#include "calendar.h"
#include "gui.h"
#include "ff.h"

#include "sd_spi.h"

extern float pm2_5,CO2_ppm,Humidity,Temperature;
extern tm timer;
//uint8_t dat1[4],dat2[3],dat3[4],dat4[4],dat[44],dat5[4];
uint8_t dat1[4],dat2[3],dat3[4],dat4[4],dat[47],dat5[4];
char read_dat[44];
extern uint8_t send_buf[16];

void CO2_ppm_TO_ascii(void)                                    //CO2数据十进制转化ASCII码
{
	if(1000<=CO2_ppm&&CO2_ppm<10000)                            //1000-9999
	{
		dat1[0] = (int)CO2_ppm%10000/1000 + 0x30;
		dat1[1] = (int)CO2_ppm%1000/100 + 0x30;
		dat1[2] = (int)CO2_ppm%100/10 + 0x30;
		dat1[3] = (int)CO2_ppm%10 + 0x30;
	}
	if(100<=CO2_ppm&&CO2_ppm<1000)                                //100-999
	{
		dat1[0] = 0x20;
		dat1[1] = (int)CO2_ppm%1000/100 + 0x30;
		dat1[2] = (int)CO2_ppm%100/10 + 0x30;
		dat1[3] = (int)CO2_ppm%10 + 0x30;
	}
	if(10<=CO2_ppm&&CO2_ppm<100)                                 //10-99
	{
		dat1[0] = 0x20;
		dat1[1] = 0x20;
		dat1[2] = (int)CO2_ppm%100/10 + 0x30;
		dat1[3] = (int)CO2_ppm%10 + 0x30;
	}
	if(0<=CO2_ppm&&CO2_ppm<10)                                  //0-9
	{
		dat1[0] = 0x20;
		dat1[1] = 0x20;
		dat1[2] = 0x32;
		dat1[3] = (int)CO2_ppm%10 + 0x30;
	}

}





void pm2_5_TO_ascii(void)                                    //pm2.5数据十进制转化ASCII码
{
	
	if(100<=pm2_5&&pm2_5<1000)                                   //100-999
	{
		dat2[0] = (int)pm2_5%1000/100 + 0x30;
		dat2[1] = (int)pm2_5%100/10 + 0x30;
		dat2[2] = (int)pm2_5%10 + 0x30;
	}
	if(10<=pm2_5&&pm2_5<100)                                     //10-99
	{
		dat2[0] = 0x20;
		dat2[1] = (int)pm2_5%100/10 + 0x30;
		dat2[2] = (int)pm2_5%10 + 0x30;
	}
	if(0<=pm2_5&&pm2_5<10)                                       //0-9
	{
		dat2[0] = 0x20;
		dat2[1] = 0x20;
		dat2[2] = (int)pm2_5%10 + 0x30;
	}

}


void Temperature_TO_ascii(void)                             //温度数据十进制转化ASCII码
{

	if(10<=Temperature&&Temperature<100)                        //10.0-99.9
	{
		dat3[0] = (int)Temperature%100/10 + 0x30;
		dat3[1] = (int)Temperature%10 + 0x30;
		dat3[2] = 0x2e;
		dat3[3] = (int)(Temperature*10)%10+0x30;                        
	}
	if(1<=Temperature&&Temperature<10)                         //1.0-9.9
	{
		dat3[0] = 0x20;
		dat3[1] = (int)Temperature%10 + 0x30;
		dat3[2] = 0x2e;
		dat3[3] = (int)(Temperature*10)%10+0x30;
	}
	if(0<=Temperature&&Temperature<1)                          //0-0.9
	{
		dat3[0] = 0x20;
		dat3[1] = 0x30;
		dat3[2] = 0x2e;
		dat3[3] = (int)(Temperature*10)%10+0x30;
	}	

}

void Humidity_TO_ascii(void)                               //湿度数据十进制转化ASCII码
{
	if(10<=Humidity&&Humidity<100)                             //10.0-99.9
	{
		dat4[0] = (int)Humidity%100/10 + 0x30;
		dat4[1] = (int)Humidity%10 + 0x30;
		dat4[2] = 0x2e;
		dat4[3] = (int)(Humidity*10)%10+0x30;                        
	}
	if(1<=Humidity&&Humidity<10)                               //1.0-9.9
	{
		dat4[0] = 0x20;
		dat4[1] = (int)Humidity%10 + 0x30;
		dat4[2] = 0x2e;
		dat4[3] = (int)(Humidity*10)%10+0x30;
	}
	if(0<=Humidity&&Humidity<1)                                 //0-0.9
	{
		dat4[0] = 0x20;
		dat4[1] = 0x30;
		dat4[2] = 0x2e;
		dat4[3] = (int)(Humidity*10)%10+0x30;
	}	
}
int time_flag=0;
int onlyone = 1;
void turn_ascii(void)
{
	Send_date();
	rtc_get();
	if(onlyone == 1)
	{
		dat[0]=(timer.hour / 10) + 0x30;
		dat[1]=(timer.hour % 10) + 0x30;	
		dat[2]=0x3A;                //冒号
		dat[3]=(timer.min / 10) + 0x30;
		dat[4]=(timer.min % 10) + 0x30;	              //0分
		dat[5]=0x3A;                //冒号
		dat[6]=(timer.sec / 10) + 0x30;
		dat[7]=(timer.sec % 10) + 0x30;	
		
		dat[8]=0x20;
		dat[9]=0x20;                   //空格

		dat[10]=0x43;
		dat[11]=0x4f;
		dat[12]=0x32;                //CO2
		dat[13]=0x3A;               //冒号
		dat[14]='T';
		dat[15]='E';
		dat[16]='S';
		dat[17]='T';            //CO2数值
		dat[18]=0x20;
		dat[19]=0x20;                   //空格

		dat[20]=0x50;
		dat[21]=0x4d;
		dat[22]=0x32;
		dat[23]=0x2e;
		dat[24]=0x35;               //pm2.5
		dat[25]=0x3A;               //冒号
		dat[26]='T';
		dat[27]='T';
		dat[28]='T';            //pm数值
		dat[29]=0x20;
		dat[30]=0x20;                   //空格

		dat[31]=0x54;               //T
		dat[32]=0x3a;               //冒号
		dat[33]='T';
		dat[34]='E';
		dat[35]='S';
		dat[36]='T';            //温度数值
		dat[37]=0x20;
		dat[38]=0x20;                   //空格

		dat[39]=0x48;               //H
		dat[40]=0x3a;               //冒号
		dat[41]='T';
		dat[42]='E';
		dat[43]='S';
		dat[44]='T';            //湿度数值

		dat[45]=0x0d;               //回车
		dat[46]=0x0a;             //换行
		
//		__set_FAULTMASK(1);//关闭全部中断
		WriteToSD_Env(dat,16);
//		__set_FAULTMASK(0);//打开全部中断
		
		printf("write env data\n");
		onlyone = 0;
	}
	if((timer.sec ==0x1e)&&(time_flag==0))             //某时0分30秒 (send_buf[0]==0)&&
	{
		CO2_ppm_TO_ascii();
		pm2_5_TO_ascii();
		Temperature_TO_ascii();
		Humidity_TO_ascii();		

		dat[0]=(timer.hour / 10) + 0x30;
		dat[1]=(timer.hour % 10) + 0x30;	
		dat[2]=0x3A;                //冒号
		dat[3]=(timer.min / 10) + 0x30;
		dat[4]=(timer.min % 10) + 0x30;	              //0分
		dat[5]=0x3A;                //冒号
		dat[6]=(timer.sec / 10) + 0x30;
		dat[7]=(timer.sec % 10) + 0x30;	
		
		dat[8]=0x20;
		dat[9]=0x20;                   //空格

		dat[10]=0x43;
		dat[11]=0x4f;
		dat[12]=0x32;                //CO2
		dat[13]=0x3A;               //冒号
		dat[14]=dat1[0];
		dat[15]=dat1[1];
		dat[16]=dat1[2];
		dat[17]=dat1[3];            //CO2数值
		dat[18]=0x20;
		dat[19]=0x20;                   //空格

		dat[20]=0x50;
		dat[21]=0x4d;
		dat[22]=0x32;
		dat[23]=0x2e;
		dat[24]=0x35;               //pm2.5
		dat[25]=0x3A;               //冒号
		dat[26]=dat2[0];
		dat[27]=dat2[1];
		dat[28]=dat2[2];            //pm数值
		dat[29]=0x20;
		dat[30]=0x20;                   //空格

		dat[31]=0x54;               //T
		dat[32]=0x3a;               //冒号
		dat[33]=dat3[0];
		dat[34]=dat3[1];
		dat[35]=dat3[2];
		dat[36]=dat3[3];            //温度数值
		dat[37]=0x20;
		dat[38]=0x20;                   //空格

		dat[39]=0x48;               //H
		dat[40]=0x3a;               //冒号
		dat[41]=dat4[0];
		dat[42]=dat4[1];
		dat[43]=dat4[2];
		dat[44]=dat4[3];            //湿度数值

		dat[45]=0x0d;               //回车
		dat[46]=0x0a;             //换行
		
//		__set_FAULTMASK	(1);//关闭全部中断
		WriteToSD_Env(dat,16);
//		__set_FAULTMASK(0);//打开全部中断
		
		printf("write env data\n");
		time_flag=1;

	}
	else if((timer.sec ==0x00)&&(time_flag==1))            //某时30分30秒   (send_buf[0]==0x1e)&&(timer.sec ==0x1e)&&(time_flag==1)
	{

		CO2_ppm_TO_ascii();
		pm2_5_TO_ascii();
		Temperature_TO_ascii();
		Humidity_TO_ascii();		
		
		dat[0]=(timer.hour / 10) + 0x30;
		dat[1]=(timer.hour % 10) + 0x30;	
		dat[2]=0x3A;                //冒号
		dat[3]=(timer.min / 10) + 0x30;
		dat[4]=(timer.min % 10) + 0x30;	              //0分
		dat[5]=0x3A;                //冒号
		dat[6]=(timer.sec / 10) + 0x30;
		dat[7]=(timer.sec % 10) + 0x30;	
		
		dat[8]=0x20;
		dat[9]=0x20;                   //空格

		dat[10]=0x43;
		dat[11]=0x4f;
		dat[12]=0x32;                //CO2
		dat[13]=0x3A;               //冒号
		dat[14]=dat1[0];
		dat[15]=dat1[1];
		dat[16]=dat1[2];
		dat[17]=dat1[3];            //CO2数值
		dat[18]=0x20;
		dat[19]=0x20;                   //空格

		dat[20]=0x50;
		dat[21]=0x4d;
		dat[22]=0x32;
		dat[23]=0x2e;
		dat[24]=0x35;               //pm2.5
		dat[25]=0x3A;               //冒号
		dat[26]=dat2[0];
		dat[27]=dat2[1];
		dat[28]=dat2[2];            //pm数值
		dat[29]=0x20;
		dat[30]=0x20;                   //空格

		dat[31]=0x54;               //T
		dat[32]=0x3a;               //冒号
		dat[33]=dat3[0];
		dat[34]=dat3[1];
		dat[35]=dat3[2];
		dat[36]=dat3[3];            //温度数值
		dat[37]=0x20;
		dat[38]=0x20;                   //空格

		dat[39]=0x48;               //H
		dat[40]=0x3a;               //冒号
		dat[41]=dat4[0];
		dat[42]=dat4[1];
		dat[43]=dat4[2];
		dat[44]=dat4[3];            //湿度数值

		dat[45]=0x0d;               //回车
		dat[46]=0x0a;             //换行
		
		__set_FAULTMASK(1);//关闭全部中断
//		__disable_irq();
		WriteToSD_Env(dat,16);
//		__enable_irq();//打开全部中断
		__set_FAULTMASK(0);
		
		printf("write env data\n");
		time_flag=0;
	}
	else
	{
	}

}


//void turn_ascii(void)
//{
//	Send_date();
////	if((send_buf[0]==0)&&(timer.sec ==0x1e)&&(time_flag==0))
//		if(timer.sec ==0x00&&time_flag==0)
//	{
//		         
//						 CO2_ppm_TO_ascii();
//						 pm2_5_TO_ascii();
//						 Temperature_TO_ascii();
//						 Humidity_TO_ascii();		
//		
////      if(send_buf[1]<=0x09)
////	     {
////	        dat[0]=0x30;
////          dat[1]=send_buf[1]+0x30;    //时
////			 }
////			 else if(send_buf[1]<=0x13)
////       {
////			    dat[0]=0x31;
////          dat[1]=send_buf[1]+0x26;    //时
////			 }
////			 else
////			 {
////			   dat[0]=0x32;
////         dat[1]=send_buf[1]+0x1c;    //时
////			 }
////          dat[2]=0x3A;                //冒号
////          dat[3]=0x30;
////          dat[4]=0x30;	              //0分
//          dat[0]=0x20;
//					dat[1]=0x20;
//          dat[2]=0x20;
//					dat[3]=0x20;
//					dat[4]=0x20;
//          dat[5]=0x20;
//     			dat[6]=0x20;                   //空格
//				  dat[7]=0x43;
//				  dat[8]=0x4f;
//				  dat[9]=0x32;                //CO2
//				  dat[10]=0x3A;               //冒号
//				  dat[11]=dat1[0];
//				  dat[12]=dat1[1];
//				  dat[13]=dat1[2];
//				  dat[14]=dat1[3];            //CO2数值
//				  dat[15]=0x20;
//     			dat[16]=0x20;                   //空格
//				  dat[17]=0x50;
//				  dat[18]=0x4d;
//				  dat[19]=0x32;
//				  dat[20]=0x2e;
//					dat[21]=0x35;               //pm2.5
//					dat[22]=0x3A;               //冒号
//				  dat[23]=dat2[0];
//				  dat[24]=dat2[1];
//					dat[25]=dat2[2];            //pm数值
//					dat[26]=0x20;
//     			dat[27]=0x20;                   //空格
//					dat[28]=0x54;               //T
//					dat[29]=0x3a;               //冒号
//					dat[30]=dat3[0];
//					dat[31]=dat3[1];
//					dat[32]=dat3[2];
//					dat[33]=dat3[3];            //温度数值
//					dat[34]=0x20;
//     			dat[35]=0x20;                   //空格
//					dat[36]=0x48;               //H
//					dat[37]=0x3a;               //冒号
//					dat[38]=dat4[0];
//					dat[39]=dat4[1];
//					dat[40]=dat4[2];
//					dat[41]=dat4[3];            //湿度数值
//					dat[42]=0x0d;               //回车
//					dat[43]=0x0a;             //换行
//	    WriteToSD_Env(dat,16);
//time_flag=1;
//   }
////else if((send_buf[0]==0x1e)&&(timer.sec ==0x1e)&&(time_flag==1))
//	 else if(timer.sec ==0x1e&&time_flag==1)
//	{
//		         
//						 CO2_ppm_TO_ascii();
//						 pm2_5_TO_ascii();
//						 Temperature_TO_ascii();
//						 Humidity_TO_ascii();		
//		
////      if(send_buf[1]<=0x09)
////	     {
////	        dat[0]=0x30;
////          dat[1]=send_buf[1]+0x30;    //时
////			 }
////			 else if(send_buf[1]<=0x13)
////       {
////			    dat[0]=0x31;
////          dat[1]=send_buf[1]+0x26;    //时
////			 }
////			 else
////			 {
////			   dat[0]=0x32;
////         dat[1]=send_buf[1]+0x1c;    //时
////			 }
////          dat[2]=0x3A;                //冒号
////          dat[3]=0x33;
////          dat[4]=0x30;	              //30分
// dat[0]=0x20;
//					dat[1]=0x20;
//          dat[2]=0x20;
//					dat[3]=0x20;
//					dat[4]=0x20;
//          dat[5]=0x20;
//     			dat[6]=0x20;                   //空格
//				  dat[7]=0x43;
//				  dat[8]=0x4f;
//				  dat[9]=0x32;                //CO2
//				  dat[10]=0x3A;               //冒号
//				  dat[11]=dat1[0];
//				  dat[12]=dat1[1];
//				  dat[13]=dat1[2];
//				  dat[14]=dat1[3];            //CO2数值
//				  dat[15]=0x20;
//     			dat[16]=0x20;                   //空格
//				  dat[17]=0x50;
//				  dat[18]=0x4d;
//				  dat[19]=0x32;
//				  dat[20]=0x2e;
//					dat[21]=0x35;               //pm2.5
//					dat[22]=0x3A;               //冒号
//				  dat[23]=dat2[0];
//				  dat[24]=dat2[1];
//					dat[25]=dat2[2];            //pm数值
//					dat[26]=0x20;
//     			dat[27]=0x20;                   //空格
//					dat[28]=0x54;               //T
//					dat[29]=0x3a;               //冒号
//					dat[30]=dat3[0];
//					dat[31]=dat3[1];
//					dat[32]=dat3[2];
//					dat[33]=dat3[3];            //温度数值
//					dat[34]=0x20;
//     			dat[35]=0x20;                   //空格
//					dat[36]=0x48;               //H
//					dat[37]=0x3a;               //冒号
//					dat[38]=dat4[0];
//					dat[39]=dat4[1];
//					dat[40]=dat4[2];
//					dat[41]=dat4[3];            //湿度数值
//          dat[42]=0x0d;               //回车
//					dat[43]=0x0a;             //换行
//	     WriteToSD_Env(dat,16);
//time_flag=0;
//   }
//else
//{
//}

//}

extern int times,f,k,g;
void turn_date (void)
{
	if(k<=0x09)
	{
		dat5[1]=0x30;
		dat5[0]=k+0x30;    //日  01-09
	}
	else if(k<=0x13)
	{
		dat5[1]=0x31;
		dat5[0]=k+0x26;    //日  10-19
	}
	else if(k<=0x1D)
	{
		dat5[1]=0x32;
		dat5[0]=k+0x1c;     //日  20-29
	}
	else
	{
		dat5[1]=0x33;
		dat5[0]=k+0x12;     //日  30、31
	}
	if(f<=0x09)
	{
		dat5[3]=0x30;
		dat5[2]=f+0x30;    //月   01-09
	}
	else 
	{
		dat5[3]=0x31;
		dat5[2]=f+0x26;    //月   10-12
	}
}
	
extern tm timer;
void read_data(void)
{
	turn_date();
	if((((f==1|f==3|f==5|f==7|f==8|f==10|f==12)&&(k<=31))|((f==4|f==6|f==9|f==11)&&(k<=30))|((f==2)&&(k<=28)))&&(g==2017))
	{
		GUI_SetFont(&GUI_Font32B_ASCII);
		GUI_DispStringAt("2017.",360,10);      //年
		GUI_DispHexAt(dat5[3],430,10,1);
		GUI_DispHexAt(dat5[2],446,10,1);       //月
		GUI_DispStringAt(".",462,10);
		GUI_DispHexAt(dat5[1],470,10,1);
		GUI_DispHexAt(dat5[0],486,10,1);       //日
		for(times=0;times<16;times++)
		{
			GUI_SetFont(&GUI_Font13_ASCII);
			ReadFromSD_Env(read_dat,16);                      //SD读数据
			GUI_DispStringAt(&read_dat[0],10,80+20*times);    //显示数据
			read_dat[0]=0;                                    //寄存器清0，防止数据不全时后续数据的重复
		}
		for(times=16;times<32;times++)
		{
			ReadFromSD_Env(read_dat,16);                      //SD读数据
			GUI_DispStringAt(&read_dat[0],320,80+20*(times-16));  //显示数据
			read_dat[0]=0;                                    //寄存器清0，防止数据不全时后续数据的重复
		}
		for(times=32;times<48;times++)
		{
			ReadFromSD_Env(read_dat,16);                      //SD读数据
			GUI_DispStringAt(&read_dat[0],630,80+20*(times-32));  //显示数据
			read_dat[0]=0;                                    //寄存器清0，防止数据不全时后续数据的重复
		}
	}
	else
	{
		GUI_DispStringAt("You enter the wrong time,",200,200);
		GUI_DispStringAt("Please enter the time again!",200,260);
	}
}