///*******************崔老师程序*****************************/
//#include "GDW376_2.h"
//#include "bsp.h"



//#define F1	0x01
//#define F2	0x02
//#define F3	0x04
//#define F4	0x08
//#define F5	0x10
//#define F6	0x20
//#define F7	0x40
//#define F8	0x80

//#define Deny_outTime		0
//#define Deny_invalidData1	1
//#define Deny_lengthFault	2
//#define Deny_calFault		3
//#define Deny_infNull		4
//#define Deny_formFault		5
//#define Deny_numRepeat		6
//#define Deny_numNull		7
//#define Deny_aplNonack		8

///** \brief pointer to the data to be sent.*/
//extern unsigned char GDW3762TxBuf[GDW3762_TX_BUF_LEN];

///** \brief buffer to store received data.*/
//extern unsigned char GDW3762RxBuf[GDW3762_RX_BUF_LEN];	


///** \brief 完成GDW376_2协议的接收
// *
// *  \param NONE
// *
// *  \return  接收状态：\n
// *           0: 等待	\n
// *		    else: 接收到完整的GDW376_2协议帧
// */

//unsigned char GDW3762Receive()
//{
//	static unsigned int RxPos = 0;					// 接收位置
//	unsigned char i, sum,len;

//	if(UARTReceive(PLCPORT, GDW3762RxBuf + RxPos, 1))	// 从串口缓冲区读出数据
//	{								
//		RxPos ++;		  	
//	}
//	
//	while((RxPos > 2) && (GDW3762RxBuf[0] != 0x68))
//	{
//											  		// 找帧头
//		for(i = 1;i < RxPos;i ++)		 			// 将缓冲区中的数据往前移
//		{
//			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i];						
//		}
//		RxPos --;
//	}
//	
//	
//	if(RxPos <= 2 || GDW3762RxBuf[0] != 0x68)		
//	{											 	// 没有找到帧头，直接返回
//		return 0;
//	}
//	
//	len = GDW3762RxBuf[1] + GDW3761RxBuf[2] * 256 ; 
//	if(len < 4 || len > GDW3761_RX_BUF_LEN)	// 长度不合法，重新找帧头
//	{						
//		for(i = 1;i < RxPos;i ++)		 	
//		{
//			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i];						
//		}
//		RxPos --; 
//		return 0;		
//	}
//	
//	if(RxPos < len)									        // 长度不够，继续等待
//	{
//		return 0;
//	}
//								  
//	if(GDW3762RxBuf[len - 1] != 0x16)				// 找到帧头，长度足够，检查结束符是否合法
//	{
//		for(i = 1;i < RxPos;i ++)
//		{
//			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i];						
//		}
//		RxPos --;
//		return 0;  					
//	}							 				
//			
//	sum = 0;				  						// 结束符合法，计算校验和
//	for(i = 3;i < len - 2;i ++)
//	{
//		sum += GDW3762RxBuf[i];					 
//	}	
//									 
//	if(sum != GDW3762RxBuf[len - 2])				// 校验和不合法，将缓冲区中的数据往前移
//	{
//		for(i = 1;i < RxPos;i ++)
//		{
//			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i]; 						
//		}
//		RxPos --;
//		return 0;  					
//	}
//	else
//	{
//		RxPos = 0;
//		return len; 
//	} 	
//}




/*******************自己程序**********************/

#include "GDW376_2.h"
#include "usart1.h"
//#include "bsp.h"




/** \brief pointer to the data to be sent.*/
unsigned char GDW3762TxBuf[GDW3762_TX_BUF_LEN];

/** \brief buffer to store received data.*/
unsigned char GDW3762RxBuf[GDW3762_RX_BUF_LEN];	


/** \brief 完成GDW376_2协议的接收
 *
 *  \param NONE
 *
 *  \return  接收状态：\n
 *           0: 等待	\n
 *		    else: 接收到完整的GDW376_2协议帧
 */

unsigned char CheckSum(unsigned char *buf,unsigned int len)
{ 
	unsigned int i;
	unsigned char ret=0;	 	
	for(i = 0;i < len;i ++)
	{
		ret += buf[i];	 
	}
	return ret;
}

//unsigned int GDW3762Receive(unsigned char ch)
//{
//	static unsigned int RxPos = 0;					// 接收位置
//	unsigned char i, sum;
//	unsigned int len;
//	
//	if(UARTReceive(ch, GDW3762RxBuf + RxPos, 1))	// 从串口缓冲区读出数据
//	{								
//		RxPos ++;		  	
//	}
//	
//	while(RxPos > 6 && GDW3762RxBuf[0] != 0x68)
//	{
//											  		// 找帧头
//		for(i = 1;i < RxPos;i ++)		 			// 将缓冲区中的数据往前移
//		{
//			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i];						
//		}
//		RxPos --;
//	}
//	
//	
//	if(RxPos < 6 || GDW3762RxBuf[0] != 0x68)		
//	{											 	// 没有找到帧头，直接返回
//		return 0;
//	}
//	
//	len = GDW3762RxBuf[1] + GDW3762RxBuf[2] * 256 ; 
//	if(len < 4 || len > GDW3762_RX_BUF_LEN)	// 长度不合法，重新找帧头
//	{						
//		for(i = 1;i < RxPos;i ++)		 	
//		{
//			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i];						
//		}
//		RxPos --; 
//		return 0;		
//	}
//	
//	if(RxPos < len)									        // 长度不够，继续等待
//	{
//		return 0;
//	}
//								  
//	if(GDW3762RxBuf[len - 1] != 0x16)				// 找到帧头，长度足够，检查结束符是否合法
//	{
//		for(i = 1;i < RxPos;i ++)
//		{
//			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i];						
//		}
//		RxPos --;
//		return 0;  					
//	}							 				
//			
//	sum = 0;				  						// 结束符合法，计算校验和
//	for(i = 3;i < len - 2;i ++)
//	{
//		sum += GDW3762RxBuf[i];					 
//	}	
//									 
//	if(sum != GDW3762RxBuf[len - 2])				// 校验和不合法，将缓冲区中的数据往前移
//	{
//		for(i = 1;i < RxPos;i ++)
//		{
//			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i]; 						
//		}
//		RxPos --;
//		return 0;  					
//	}
//	else
//	{
//		RxPos = 0;
//		return len; 
//	} 	
//}

/***376.2数据接收*******/
unsigned int GDW3762Receive(void)
//unsigned int USART1_IRQHandler(void)
{
	static unsigned int RxPos = 0;					// 接收位置
	unsigned char i, sum;
	unsigned int len;
	
	if(UARTReceive(GDW3762RxBuf + RxPos, 1))	// 从串口缓冲区读出数据
	{								
		RxPos ++;		  	
	}
	
	while(RxPos > 6 && GDW3762RxBuf[0] != 0x68)
	{
											  		// 找帧头
		for(i = 1;i < RxPos;i ++)		 			// 将缓冲区中的数据往前移
		{
			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i];						
		}
		RxPos --;
	}
	
	
	if(RxPos < 6 || GDW3762RxBuf[0] != 0x68)		
	{											 	// 没有找到帧头，直接返回
		return 0;
	}
	
	len = GDW3762RxBuf[1] + GDW3762RxBuf[2] * 256 ; 
	if(len < 4 || len > GDW3762_RX_BUF_LEN)	// 长度不合法，重新找帧头
	{						
		for(i = 1;i < RxPos;i ++)		 	
		{
			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i];						
		}
		RxPos --; 
		return 0;		
	}
	
	if(RxPos < len)									        // 长度不够，继续等待
	{
		return 0;
	}
								  
	if(GDW3762RxBuf[len - 1] != 0x16)				// 找到帧头，长度足够，检查结束符是否合法
	{
		for(i = 1;i < RxPos;i ++)
		{
			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i];						
		}
		RxPos --;
		return 0;  					
	}							 				
			
	sum = 0;				  						// 结束符合法，计算校验和
	for(i = 3;i < len - 2;i ++)
	{
		sum += GDW3762RxBuf[i];					 
	}	
									 
	if(sum != GDW3762RxBuf[len - 2])				// 校验和不合法，将缓冲区中的数据往前移
	{
		for(i = 1;i < RxPos;i ++)
		{
			GDW3762RxBuf[i - 1] = GDW3762RxBuf[i]; 						
		}
		RxPos --;
		return 0;  					
	}
	else
	{
		RxPos = 0;
		return len; 
	}	
}


/** \brief 完成DLT645-2007协议（确认）的发送
 *
 *  \param error_code：错误码
 *
 *  \return  NONE
 */

//void GDW3762Confirm(void)
//{
//	unsigned char i = 0;
//	GDW3762TxBuf[i++]=0x68;
//	GDW3762TxBuf[i++]=0x15;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0x81;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0x01;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0xFF;
//	GDW3762TxBuf[i++]=0xFF;
//	GDW3762TxBuf[i++]=0xFF;
//	GDW3762TxBuf[i++]=0xFF;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=0x00;
//	GDW3762TxBuf[i++]=CheckSum(&GDW3762TxBuf[3], i - 4);;
//	GDW3762TxBuf[i++]=0x16;
//	
////	UARTSend(PLCPORT, GDW3762TxBuf, i); 
//	UARTSend(GDW3762TxBuf, i); 
//}



/** \brief 完成DLT645-2007协议（否认）的发送
 *
 *  \param error_code：错误码
 *
 *  \return  NONE
 */
//	void  GDW3762Deny(unsigned char error_code)
//	{
//	unsigned char i = 0;
//	
//	GDW3762TxBuf[i++] = 0x68;
//	GDW3762TxBuf[i++] = 0x10;
//	GDW3762TxBuf[i++] = 0x00;
//	GDW3762TxBuf[i++] = 0x81;	
//	GDW3762TxBuf[i++] = 0x00;
//	GDW3762TxBuf[i++] = 0x00;
//	GDW3762TxBuf[i++] = 0x00;
//	GDW3762TxBuf[i++] = 0x00;
//	GDW3762TxBuf[i++] = 0x00;
//	GDW3762TxBuf[i++] = 0x00;
//	GDW3762TxBuf[i++] = 0x00;
//	GDW3762TxBuf[i++] = 0x02;		
//	GDW3762TxBuf[i++] = 0x00;
//					
//	GDW3762TxBuf[i++] = error_code;
//	GDW3762TxBuf[i++] = CheckSum(&GDW3762TxBuf[3], i - 4);
//	GDW3762TxBuf[i++] = 0x16;
//	
//	UARTSend(GDW3762TxBuf, i); 
//	}







