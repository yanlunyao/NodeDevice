/*
********************************************************************************
  Copyright (C), 2009-2013 GuangdongGuanglian Electronic Technology Co.,Ltd.
  File name:      delay.h
  Description:
  Author:         He Peikai
  Version:        V1.0
  Date:           2013.6.13
  History:
    1. Date:
       Author:
       Modification:
    2. ...
********************************************************************************
*/
#ifndef __OSDELAY_H
#define __OSDELAY_H 			   
#include "stm32f10x.h"  


/*
********************************************************************************
  function declaration
********************************************************************************
*/ 	 
extern void delay_init(u8 sysclk);
extern void delay_ms(u16 nms);
extern void delay_us(u32 nus);

#endif
