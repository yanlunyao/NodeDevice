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
#ifndef __OSRF315TX_H
#define __OSRF315TX_H

#include "hw_config.h"

#define	SMARTSOCKET_TOTAL_OPEN_ADDR										1
#define	SMARTSOCKET_TOTAL_CLOSE_ADDR									2
#define	SMARTSOCKET_INVERT_ADDR										    3



//extern void rf315_send(u8 *addr);
extern void OsRf315Init(void);
extern void Rf315SendMsg(u8 *data);

#endif
