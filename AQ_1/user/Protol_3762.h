#ifndef __PROTOL3762_H_
#define __PROTOL3762_H_


#include "stm32f10x.h"   
#include <stdio.h> //�õ�printf�����Ĵ��ڵ��������  ע�⹴ѡMicroLIB  
#include <stdlib.h>
//#include <intrins.h>
//֡λ��
#define  PHead         0
#define  PLen0         1
#define  PLen1         2
#define  PCtr          3  //������

// ��Ϣ��

#define  PR0           4
#define  PR1           5
#define  PR2           6
#define  PR3           7
#define  PR4           8
#define  PR5           9


//��ַ��
#define  PA0_0         10
#define  PA0_1         11
#define  PA0_2         12
#define  PA0_3         13
#define  PA0_4         14
#define  PA0_5         15

#define  PA3_0         16
#define  PA3_1         17
#define  PA3_2         18
#define  PA3_3         19
#define  PA3_4         20
#define  PA3_5         21

//�жϴ����־λ
#define sta_add         0
#define handle_IRQ      1

//Ӧ�ò㹦����
#define  PAfn          10     //�е�ַ��ʱ��PAfn=Pafn+12

//��Ϣ��
#define  PDt1          11  
#define  PDt2          12 

#define  HJ_buf       6
#define  Mor_buf       6  
uint16_t Monitoring_3762(void);
//void re_jzq(void);
uint16_t star_add(void);
//void Delay(vu32 nCount);
//void IRQ_chuli(void);
void Rx_buf(void);
void environment_list (void);
uint16_t Back_time(void);
int BCD_Decimal(uint8_t bcd);
#endif