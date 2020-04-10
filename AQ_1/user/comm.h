#ifndef _comm_h_
#define _comm_h_

#include "stm32f10x.h"
#include "stdio.h"




extern void comm_init(void);
extern void comm_send_byte(u8 ch);
extern void comm_send_buff(u8 *buf);


#endif
