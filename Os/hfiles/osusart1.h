/**
  ******************************************************************************
  * @file    osprintf.h
  * @author  yanly
  * @version 
  * @date    
  * @brief   usart interupt implement send and receive (hardware os layer)
  ******************************************************************************
  */ 

#ifndef __OSUSART1_H
#define __OSUSART1_H

/* Includes ------------------------------------------------------------------*/
#include "hw_usart.h"

//#define DEBUG_PRINTF  

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/



extern void OsPrintf_Init(void);
extern scommReturn_t  t_hwuart1_ReceiveMessage(scommRcvBuf_t *rxMsg);
extern scommReturn_t  t_hwuart1_SendMessage(scommTxBuf_t*  txMsg);
extern u8  u1_hwuart1_txing(void);
/**
  * @}
  */ 

/**
  * @}
  */ 

#endif
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
