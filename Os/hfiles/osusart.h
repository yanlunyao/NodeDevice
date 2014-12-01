/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief  
  ******************************************************************************
  * @copy
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OSUSART_H
#define __OSUSART_H

#include "osusart1.h"
#include "osusart2.h"

#define  USART1_COM		0
#define  USART2_COM		1
//串口1为0，串口2为1

#define ZIGBEE_INFO_LENGTH				USART_TX_DATA_SIZE-0



typedef void (* functionP_t) (void);
/******************************************************************************/
extern scommReturn_t  t_osscomm_ReceiveMessage(u8  *rxData, u16 *rxLen, u8 port);  //upper layer usart receive function  
extern scommReturn_t  t_osscomm_sendMessage(u8  *txData, u16 txLen, u8 port); //upper layer usart send function  

#endif 
