/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  */ 

#ifndef __HW_USART_H
#define __HW_USART_H

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"




#define USART_TX_DATA_SIZE      0xFF
#define USART_RX_DATA_SIZE      0xFF   //21
#define UART1_TX_BUF_SIZE       0xff
#define UART1_RX_BUF_SIZE       0xff  

#define UART0_FRM_TMR_RESTART     12       /*uart1 frame timer 12ms*/
#define UART0_CHAR_TMR_RESTART    4        /*uart1 char timer 4ms*//*此时间应大于接收8byte数据所需时间,所以不同波特率
                                                该值也应不一样.值为4时,bps须大于2400bps,值为2时,bps须大于4800*/

/* Private macro -------------------------------------------------------------*/
typedef struct
{
  u16              scommTxLen;                         /*tx data length*/
  u8              *pscommTxContent;
}scommTxBuf_t;

typedef struct
{
  u16              scommRcvLen;
  u8              *pscommRcvContent;
}scommRcvBuf_t;
typedef enum
{
  URX_BUF_IDLE = 0,
  URX_BUF_BUSY,        /**/
  URX_BUF_COMPL,       /*receiver complete, but frame format haven't been checken*/          
  URX_BUF_READY        /*receiver complete, and frame format ok*/
}uRxBufStatus_t;    

typedef enum
{
  UTX_BUF_IDLE = 0,
  UTX_BUF_BUSY,        /**/
  UTX_BUF_COMPL,       /*receiver complete, but frame format haven't been checken*/          
  UTX_BUF_READY        /*receiver complete, and frame format ok*/
}uTxBufStatus_t;  

typedef enum
{
  SCOMM_RET_NONE = 0,
  SCOMM_RET_ERR_PARAM,
	SCOMM_RET_TXING,
  SCOMM_RET_TIMEOUT,
  SCOMM_RET_NOREADY,
  SCOMM_RET_OK,
  SCOMM_RET_PORT_ERR,
}scommReturn_t;

/* Private macro -------------------------------------------------------------*/
typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;  

extern USART_TypeDef* COM_USART[COMn]; 

extern GPIO_TypeDef* COM_TX_PORT[COMn] ;
 
extern GPIO_TypeDef* COM_RX_PORT[COMn] ;
 
extern const uint32_t COM_USART_CLK[COMn];

extern const uint32_t COM_TX_PORT_CLK[COMn];
 
extern const uint32_t COM_RX_PORT_CLK[COMn] ;

extern const uint16_t COM_TX_PIN[COMn];

extern const uint16_t COM_RX_PIN[COMn];		



#endif



