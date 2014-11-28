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
#ifndef __OSADC_H
#define __OSADC_H
#include "hw_config.h"


extern void OsAdcInit(void);
extern u16 get_adc_average(u8 ch, u8 times);
//extern void GetADCConvertedValueInPtr(u8 ch, u32 *ptr);

#endif 
