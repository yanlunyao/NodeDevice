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
#ifndef __APPRF315TX_H
#define __APPRF315TX_H

#include "osrf315tx.h"

typedef void (* FunctionPtr) (u8 id);
#define		SWITCH_INVERSION		1
#define		SWITCH_OPEN					2
#define		SWITCH_CLOSE				3



extern const FunctionPtr send_315M_msg[];


#endif
