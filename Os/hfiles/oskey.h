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
#ifndef __OSKEY_H
#define __OSKEY_H

#include "hw_config.h"

#define	 ANTI_SHAKE_TIME	10   //100ms 

//#define  KEY_PRESSED(keyNum)  (!GPIO_ReadInputDataBit(GPIO_KEY_PORT, GPIO_KEY##keyNum))
#define  LEARN_KEY_PRESSED	(!GPIO_ReadInputDataBit(GPIO_KEY_PORT, GPIO_LEARN_KEY))

extern void OsKeyInit(void);
extern void OsDialSwitchInit(void);
//extern u8 ReadKeyDown(void);
extern u8 OsGetSmartSocketAddr(void);
extern u8 OsGetSmartControlSwitchValue(void);
extern u8 OsGetNodeAddr(void);

#endif
