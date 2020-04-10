#include "stm32f10x.h"
#include "led.h"
#include "mstimer.h"

/* LED */
static const gpio_t LED_TAB[] =
{
	GPIO_PIN(GPIOF, 5), 	/* SWITCH LED */
	GPIO_PIN(GPIOE, 1), 	/* LED2 */
	GPIO_PIN(GPIOG, 14), 	/* LED2_1 	runing led */
	GPIO_PIN(GPIOB, 9),  	/* LED3     */
	GPIO_PIN(GPIOF, 7),  	/* LED3_1  uart2 recv led*/
	GPIO_PIN(GPIOF, 8),  	/* LED4  	uart2 send led */
	GPIO_PIN(GPIOF, 6), 	/* LED4_1 */
};

/**
 * @history 
 * v1.0 old version:key configure 
 * v1.1 modify LE4_1 -> PF6     LED3_1 -> PF7   LED4 -> PF8 
 */

uint8_t LED_STATUS=0;
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;//LED3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化LED3
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//LED2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成推挽输出
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化LED2
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13 | GPIO_Pin_14;//LED2_1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成推挽输出
 	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化LED4_1,LED3_1,LED4,LED2_1
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;//SWITCH LED, LED4_1,LED3_1,LED4,
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成推挽输出
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化SWITCH LED
	
	LED_STATUS = 0;

	GPIO_SetBits(GPIOE,GPIO_Pin_1);
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	GPIO_SetBits(GPIOG,GPIO_Pin_14);

	GPIO_SetBits(GPIOF,GPIO_Pin_5);
	
	GPIO_SetBits(GPIOF,GPIO_Pin_6);
	GPIO_SetBits(GPIOF,GPIO_Pin_7);
	GPIO_SetBits(GPIOF,GPIO_Pin_8);
    
    GPIO_ResetBits(GPIOG,GPIO_Pin_13);
}


/*******************************************************************************
* Function Name  : hw_led_init
* Description    : 接收到载波数据，接收指示灯闪烁一次。LED3_1
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void hw_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint8_t n;

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    for (n = 0; n < countof(LED_TAB); n++)
    {
        RCC_APB2PeriphClockCmd(LED_TAB[n].RCC_APB2Periph, ENABLE);

        GPIO_InitStructure.GPIO_Pin = LED_TAB[n].GPIO_Pin;
        GPIO_Init(LED_TAB[n].GPIOx, &GPIO_InitStructure);
        GPIO_SetBits(LED_TAB[n].GPIOx, LED_TAB[n].GPIO_Pin);
    }
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13 | GPIO_Pin_14;//LED2_1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成推挽输出
 	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化LED4_1,LED3_1,LED4,LED2_1
}

void hw_led_on(uint32_t n)
{
    if(n < countof(LED_TAB))
    {
        GPIO_ResetBits(LED_TAB[n].GPIOx, LED_TAB[n].GPIO_Pin);
    }
}

void hw_led_off(uint32_t n)
{
    if(n < countof(LED_TAB))
    {
        GPIO_SetBits(LED_TAB[n].GPIOx, LED_TAB[n].GPIO_Pin);
    }
}



/*******************************************************************************
* Function Name  : carrier_receive
* Description    : 接收到载波数据，接收指示灯闪烁一次。LED3_1
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void carrier_receive(void)
{
    extern int flag_receive;
    static int receive_step=0;
    static uint16_t receive_t=0;
	
	if(flag_receive==1)
	{
        switch(receive_step)
		{
			case 0:
				    MSTimerSet(receive_t, 700);    
//				    GPIO_ResetBits(GPIOG,GPIO_Pin_10);
						hw_led_on(UART2_RECV_LED);
				    receive_step++;
			break;
		  case 1:
				    if(MSTimerCheck(receive_t))
						{
						  hw_led_off(UART2_RECV_LED);
							flag_receive=0;
							receive_step=0;
						}
			break;
		
			default:
				    receive_step = 0 ;
			      flag_receive=0;
			break;
		}
	}
}

/*******************************************************************************
* Function Name  : carrier_transmit
* Description    : 发送到载波数据，发送指示灯闪烁一次。led4
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void carrier_transmit(void)
{
    extern int flag_transmit;
	  static int transmit_step=0;
		static uint16_t transmit_t=0;
	if(flag_transmit==1)
	{
    switch(transmit_step)
		{
			case 0:
				    MSTimerSet(transmit_t, 700);    
				    hw_led_on(UART2_SEND_LED);
				    transmit_step++;
			break;
		  case 1:
				    if(MSTimerCheck(transmit_t))
						{
						  hw_led_off(UART2_SEND_LED);
							flag_transmit=0;
							transmit_step=0;
						}
			break;
		
			default:
				    transmit_step = 0 ;
			      flag_transmit=0;
			break;
		}
	}
}


/*******************************************************************************
* Function Name  : LED_Run
* Description    : 运行指示灯每2s闪烁一次。
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LED_Run(void) 
{
	static int led_step=0;
	static uint16_t led_t=0;
	carrier_receive();
	carrier_transmit();
	switch(led_step)
	{
		case 0:
			MSTimerSet(led_t, 1000);    
			LED_Toggle();
			led_step++;
		break;
		case 1:
			if(MSTimerCheck(led_t))
			{
				led_step=0;
			}
		break;
			
		default:
			led_step = 0 ;
		break;
	} 
}

void LED_Toggle(void)
{
		if(LED_STATUS == 1)
		{
				LED_Reset_GREEN();
		}
		else if(LED_STATUS == 0)
		{
				LED_Set_GREEN();
		}
		else
		{
		
		}
}



void LED_Set_GREEN(void)
{
	hw_led_on(RUNING_LED);
	LED_STATUS = 1;
}
//拉低绿灯，灯亮
void LED_Reset_GREEN(void)
{
	hw_led_off(RUNING_LED);
	LED_STATUS = 0;
}
