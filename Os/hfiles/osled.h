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
#ifndef __OSLED_H
#define __OSLED_H

#include "hw_config.h"

#define LEARN_LED_ALWAYS_OFF                    0x00  //--------
#define LEARN_LED_BLINK                         0xaa  //+-+-+-+-
#define LEARN_LED_NORMAL												0xf0	//++++----

extern union_byte learnLedBlinkMode;
extern void OsLedInit(void);



#endif /* __OSLED_H */

