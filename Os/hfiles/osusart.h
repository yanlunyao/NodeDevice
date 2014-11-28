/**
  ******************************************************************************
  * @file    osprintf.h
  * @author  yanly
  * @version 
  * @date    
  * @brief   usart interupt implement send and receive (hardware os layer)
  ******************************************************************************
  */ 

#ifndef __OSUSART_H
#define __OSUSART_H

/* Includes ------------------------------------------------------------------*/
#include "hw_usart.h"

#define EVAL_COM1_USARTx    USART2


extern void OsUsartInit(void);
extern scommReturn_t  t_hwuart0_ReceiveMessage(scommRcvBuf_t *rxMsg);
extern scommReturn_t  t_hwuart0_SendMessage(scommTxBuf_t*  txMsg);
extern u8  u1_hwuart0_txing(void);

#endif

