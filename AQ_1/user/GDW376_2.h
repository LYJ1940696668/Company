///**\file GDW376_2.H**********************************
//*
//* \brief  管理GDW376_2协议的接收任务的头文件
//*
//*  该文件包含了所有关于接收GDW376_2协议的状态机的函数声明等
//* 
//*\par 作者
//* 
//* 
//*\par (修订历史) 
//* 2016-02-18
//*
//*\par 版权 
//*
//*************************************************/
//#ifndef __GDW376_2_H__
//#define __GDW376_2_H__

//#ifdef          __cplusplus
//extern  "C" {                               /* C declarations in C++     */
//#endif
//  
///*******************************************************************************
//* Macro			 : GDW3762_RX_BUF_LEN
//* Description    : define the GDW376.2 Receive buffer length.
//*******************************************************************************/
///** \brief define the GDW376.2 Receive buffer length.*/
//#define GDW3762_RX_BUF_LEN	2048

///*******************************************************************************
//* Macro			 : GDW3762_TX_BUF_LEN
//* Description    : define the GDW376.2 Transmit buffer length.
//*******************************************************************************/
///** \brief define the GDW376.2 Transmit buffer length.*/
//#define GDW3762_TX_BUF_LEN	2048

///*******************************************************************************
//* Variable Name  : GDW3762TxBuf
//* Description    : pointer to the data to be sent.
//*******************************************************************************/
///** \brief pointer to the data to be sent.*/
//extern unsigned char GDW3762TxBuf[];

///*******************************************************************************
//* Variable Name  : GDW3762RxBuf
//* Description    : buffer to store received data.
//*******************************************************************************/
///** \brief buffer to store received data.*/
//extern unsigned char GDW3762RxBuf[];


///*******************************************************************************
//* Function Name  : GDW3762Receive
//* Description    : Receive a GDW376.2 frame form serial port.
//* Input          : None
//* Output         : None
//* Return         : 0		waiting
//*				 : other	OK
//*******************************************************************************/
//unsigned char GDW3762Receive(void);
//void GDW3762Confirm(void);
//void  GDW3762Deny(unsigned char error_code);

//#ifdef          __cplusplus
//}                                           /* End of C declarations     */
//#endif
//  
//#endif // #ifndef __GDW376_2_H__






/****************自己程序*****************************/
/**\file GDW376_2.H**********************************
*
* \brief  管理GDW376_2协议的接收任务的头文件
*
*  该文件包含了所有关于接收GDW376_2协议的状态机的函数声明等
* 
*\par 作者
* 
* 
*\par (修订历史) 
* 2016-02-18
*
*\par 版权 
*
*************************************************/
#ifndef __GDW376_2_H__
#define __GDW376_2_H__

#ifdef          __cplusplus
extern  "C" {                               /* C declarations in C++     */
#endif
  
/*******************************************************************************
* Macro			 : GDW3762_RX_BUF_LEN
* Description    : define the GDW376.2 Receive buffer length.
*******************************************************************************/
/** \brief define the GDW376.2 Receive buffer length.*/
#define GDW3762_RX_BUF_LEN	2048

/*******************************************************************************
* Macro			 : GDW3762_TX_BUF_LEN
* Description    : define the GDW376.2 Transmit buffer length.
*******************************************************************************/
/** \brief define the GDW376.2 Transmit buffer length.*/
#define GDW3762_TX_BUF_LEN	2048

/*******************************************************************************
* Variable Name  : GDW3762TxBuf
* Description    : pointer to the data to be sent.
*******************************************************************************/
/** \brief pointer to the data to be sent.*/
extern unsigned char GDW3762TxBuf[];

/*******************************************************************************
* Variable Name  : GDW3762RxBuf
* Description    : buffer to store received data.
*******************************************************************************/
/** \brief buffer to store received data.*/
extern unsigned char GDW3762RxBuf[];


/*******************************************************************************
* Function Name  : GDW3762Receive
* Description    : Receive a GDW376.2 frame form serial port.
* Input          : None
* Output         : None
* Return         : 0		waiting
*				 : other	OK
*******************************************************************************/
unsigned int GDW3762Receive(void);

unsigned char CheckSum(unsigned char *buf,unsigned int len);
#ifdef          __cplusplus
}                                           /* End of C declarations     */
#endif
  
#endif // #ifndef __GDW376_2_H__


