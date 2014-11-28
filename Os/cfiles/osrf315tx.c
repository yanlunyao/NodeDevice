/*
********************************************************************************
  File name:      osrf315tx.c
  Description:    RF 315MHz wireless sending module, 根据接收芯片周期计算发送周期clk，软件模拟发送，
									算出clk，PREAMBLE_HIGH_TIME = 4clk, PREAMBLE_LOW_TIME = 124clk, DATA_HIGH_H = 12CLK, DATA_HIGH_L = 4CLK,DATA_LOW=4CLK,DATA_LOW_L=12CLK.
  Author:         
  Version:        
  Date:           
  History:
    1. Date:
       Author:
       Modification:
    2. ...
********************************************************************************
*/
//#include "osdelay.h"
#include "osrf315tx.h"
#include "osled.h"

/*define********************************************************************************************/
#define		RF_SEND_TIMES_NUM					5

#define		RF_SEND_HIGH							GPIO_WriteBit(GPIO_RF315_TX_PORT, GPIO_RF315_TX, Bit_SET);
#define		RF_SEND_LOW								GPIO_WriteBit(GPIO_RF315_TX_PORT, GPIO_RF315_TX, Bit_RESET);

//#define		PREAMBLE_HIGH_TIME				170    //unit: us
//#define		PREAMBLE_LOW_TIME				  5300
//#define		DATAH_HIGH_TIME				    520
//#define		DATAH_LOW_TIME				    170
//#define		DATAL_HIGH_TIME				    170
//#define		DATAL_LOW_TIME				    520
/***************************************************************************************************/
typedef enum
{
  RF_IDLE = 0,    //空闲状态，允许发送状态
  RF_TXING_SYNC,  //同步位发送状态
  RF_TXING_DATA   //数据位发送状态         
}_rfSendStatus_st;   

/*valiable********************************************************************************************/
static u8 rfTimerCnt = 0;
static u8 rfDataBit[25] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; //加多一位防止数组溢出
volatile u8 rfDataBitNum = 0;
volatile _rfSendStatus_st rfSendStatus; //中断里会被改变

	
/**static function**************************************************************************************/
static void Rf315StopSendIt(void);
static void Rf315StartSendIt(void);
static void TimerInit_3nd(void);
static void TIM_NVIC_Configuration_3nd(void);
static bool_t Rf315Transmit(u8 *addr);

//static void Rf315StopSend(void);
//static void Rf315StartSend(void);
//	
//static void Rf315StopSend()
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_RF315_TX_STOP_MODE;
//  GPIO_InitStructure.GPIO_Pin = GPIO_RF315_TX;
//  GPIO_Init(GPIO_RF315_TX_PORT, &GPIO_InitStructure);
//}
//static void Rf315StartSend()
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_RF315_TX_SEND_MODE;
//  GPIO_InitStructure.GPIO_Pin = GPIO_RF315_TX;
//	GPIO_InitStructure.GPIO_Speed = GPIO_RF315_TX_SPEED;
//  GPIO_Init(GPIO_RF315_TX_PORT, &GPIO_InitStructure);
//	
//	GPIO_WriteBit(GPIO_RF315_TX_PORT, GPIO_RF315_TX, Bit_SET);
//}

void OsRf315Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_RF315_TX, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_RF315_TX_STOP_MODE;
  GPIO_InitStructure.GPIO_Pin = GPIO_RF315_TX;
  GPIO_Init(GPIO_RF315_TX_PORT, &GPIO_InitStructure);
	
	TimerInit_3nd();
	TIM_NVIC_Configuration_3nd();
}
/*********************************************************************************************************
*	brief: 旧的延时发送rf函数
*********************************************************************************************************/
//void rf315_send(u8 *addr)  
//{
//	u8 i = 0;
//	u8 j = 0;
//	u8 k = 0;
//	u8 tmp[3];

//	Rf315StartSend();
//	
//	for(k = 0; k < RF_SEND_TIMES_NUM; k++)		// cycle to send 5 times
//	{
//		for(i = 0; i < 3; i++)
//		{
//			tmp[i] = *(addr + i);
//		}
//		delay_us(PREAMBLE_HIGH_TIME);			//同步位
//		RF_SEND_LOW;
//		delay_us(PREAMBLE_LOW_TIME);
//		RF_SEND_HIGH;
//		for(j = 0; j < 3; j++)
//		{
//			for(i = 0; i < 8; i++)
//			{
//				if(tmp[j] & 0x80)		// DATA (H)     //高位先发？？？yan
//				{
//					delay_us(DATAH_HIGH_TIME);
//					RF_SEND_LOW;
//					delay_us(DATAH_LOW_TIME);
//					RF_SEND_HIGH;
//				}
//				else					// DATA (L)
//				{
//					delay_us(DATAL_HIGH_TIME);
//					RF_SEND_LOW;
//					delay_us(DATAL_LOW_TIME);
//					RF_SEND_HIGH;
//				}
//				tmp[j] <<= 1;
//			}
//		}
//	}

//	Rf315StopSend();
//}
/*
*********************************************************************************************************
*	brief: 315发射的定时器,时间为  1clk = 83us （300k，12v ，1bit data=1.31ms，4clk=332us）
  定时器默认不开启中断，当发射315信号时才开启。
*********************************************************************************************************
*/
static void TimerInit_3nd(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_TIMER_3ND, ENABLE);
  

  TIM_TimeBaseStructure.TIM_Period = (83 - 1); 
 
  TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);   

  TIM_TimeBaseStructure.TIM_ClockDivision = 0;

  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIMER_CHANNEL_3ND, &TIM_TimeBaseStructure);

  TIM_ClearITPendingBit(TIMER_CHANNEL_3ND, TIM_IT_Update);

  /* TIM IT enable */
  //TIM_ITConfig(TIMER_CHANNEL_3ND, TIM_IT_Update, ENABLE); //发射时开启

  /* TIM5 enable counter */
  TIM_Cmd(TIMER_CHANNEL_3ND, ENABLE);
}

/*
*********************************************************************************************************
*	brief: 315发射的定时器,时间为  1clk = 83us （300k，12v ，1bit data=1.31ms，4clk=332us）
  定时器rf发送中断
*********************************************************************************************************
*/	
void _TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIMER_CHANNEL_3ND, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIMER_CHANNEL_3ND, TIM_IT_Update);
		
		rfTimerCnt++;
		if(rfSendStatus == RF_TXING_SYNC) //发送同步位
		{
		  if(rfTimerCnt> 128)
			{
				RF_SEND_HIGH;
				rfSendStatus = RF_TXING_DATA;
				rfTimerCnt = 0;
			}
			else
			{
				if(rfTimerCnt>4)
				{
					RF_SEND_LOW;
				}
				else
				{
					//RF_SEND_HIGH;
				}
			}	
		}
		else if(rfSendStatus == RF_TXING_DATA)  //发送数据位
		{
			if(rfDataBit[rfDataBitNum] == 1) //发送高位
			{
				if(rfTimerCnt>12)
				{
					RF_SEND_LOW;
				}
			}
			else // 发送低位
			{
				if(rfTimerCnt>4)
				{
					RF_SEND_LOW;
				}
			}	
			if(rfTimerCnt == 16)
			{
				rfTimerCnt = 0;
				rfDataBitNum++;
			}
			if(rfDataBitNum == 24) //发送到data最后一位了，表示rf发送完毕，此时关闭中断，复位
			{
				Rf315StopSendIt();
			}	
		}	
	}		
}
/*
*********************************************************************************************************
*	brief: rf信号脚置为输入，停止定时发送中断
*********************************************************************************************************
*/	
static void Rf315StopSendIt()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_RF315_TX_STOP_MODE;
  GPIO_InitStructure.GPIO_Pin = GPIO_RF315_TX;
  GPIO_Init(GPIO_RF315_TX_PORT, &GPIO_InitStructure);//浮点输入
	
	rfDataBitNum = 0;
	rfSendStatus = RF_IDLE;
	TIM_ITConfig(TIMER_CHANNEL_3ND, TIM_IT_Update, DISABLE); 
}
/*
*********************************************************************************************************
*	brief: rf信号脚置为输入，停止定时发送中断
*********************************************************************************************************
*/	
static void Rf315StartSendIt()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_RF315_TX_SEND_MODE;
  GPIO_InitStructure.GPIO_Pin = GPIO_RF315_TX;
	GPIO_InitStructure.GPIO_Speed = GPIO_RF315_TX_SPEED;
  GPIO_Init(GPIO_RF315_TX_PORT, &GPIO_InitStructure);//输出
	
	RF_SEND_HIGH;
	rfDataBitNum = 0;
	rfSendStatus = RF_TXING_SYNC; //开始发送时，首先发送同步位信号
	TIM_ITConfig(TIMER_CHANNEL_3ND, TIM_IT_Update, ENABLE); 
}
static void TIM_NVIC_Configuration_3nd(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIMER_IRQ_CHANNEL_3ND;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIMER_PreemptionPriority_3ND;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIMER_SubPriority_3ND;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
  
}
/*********************************************************************************************************
*	brief: 发送rf
*********************************************************************************************************/
static bool_t Rf315Transmit(u8 *addr)
{
	u8 i = 0;
	u8 j = 0;
	u8 tmp[3];
	u8 bitNum =0;
	
	//判断rf发射状态是否空闲，空闲才允许发送
	if(rfSendStatus != RF_IDLE)
	{
		return FALSE;
	}	
	//判断数据位，存在rfDataBit数组缓冲区
	for(i = 0; i < 3; i++)
	{
		tmp[i] = *(addr + i);
	}
	for(j = 0; j < 3; j++)
	{
		for(i = 0; i < 8; i++)
		{
			if(tmp[j] & 0x80)		// DATA (H)     
			{
				rfDataBit[bitNum] = 1;
			}
			else					// DATA (L)
			{
				rfDataBit[bitNum] = 0;
			}
			tmp[j] <<= 1;
			bitNum++;
		}
	}
	//开始发送rf信号
	Rf315StartSendIt();
	return TRUE;
}
/*********************************************************************************************************
*	brief: 调用发送rf函数，发送若干次
*********************************************************************************************************/
void Rf315SendMsg(u8 *data)
{
	u8 i;
	for(i=0;i<RF_SEND_TIMES_NUM;i++)
	{
		while(Rf315Transmit(data)!= TRUE);
	}
	learnLedBlinkMode.int8u = LEARN_LED_BLINK;//学习灯闪烁
}	

