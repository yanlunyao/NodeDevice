/*
********************************************************************************
  Copyright (C), 2009-2013 GuangdongGuanglian Electronic Technology Co.,Ltd.
  File name:      osdelay.c		
  Description:		systick
  Author:         yan modified from He Peikai
  Version:       
  Date:           
  History:
********************************************************************************
*/
#include "osdelay.h"


// if use ucos, then include the header file for the below
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"		// ucos used
#endif


/*
********************************************************************************
  variable declaration
********************************************************************************
*/
static u8  fac_us = 0;		// us delay times the multiplier		   
static u16 fac_ms = 0;		// ms delay times the multiplier

// if OS_CRITICAL_METHOD define, then use ucosII
#ifdef OS_CRITICAL_METHOD
void SysTick_Handler(void)	//systick interrupt service function, the ucos used
{				   
	OSIntEnter();		// enter the interrupt
    OSTimeTick();       // invoke the ucos clock service program             
    OSIntExit();        // triggers the task switching soft interrupt
}
#endif


/*
********************************************************************************
  Function:			delay_init
  Description:		Delay initialize
  Input:			sysclk:system clock
  Output:			
  Return:			
  Others:			
********************************************************************************
*/			   
void delay_init(u8 sysclk)
{
#ifdef OS_CRITICAL_METHOD 	// if OS_CRITICAL_METHOD define, then use ucosII
	u32 reload;
#endif
 	SysTick->CTRL &= ~(1 << 2);	// SYSTICK use external clock source
	fac_us = sysclk / 8;
	    
#ifdef OS_CRITICAL_METHOD 		// if OS_CRITICAL_METHOD define, then use ucosII
	reload = SYSCLK / 8;		// every second counts
	reload *= 1000000 / OS_TICKS_PER_SEC;	//according OS_TICKS_PER_SEC set overflow time

	fac_ms = 1000 / OS_TICKS_PER_SEC;	// the ucos can delay the minimum unit
	SysTick->CTRL |= 1 << 1;   			// SYSTICK interrupt enable
	SysTick->LOAD = reload; 			// every 1/OS_TICKS_PER_SEC second interrupt once	
	SysTick->CTRL |= 1 << 0;   			// SYSTICK enable    
#else
	fac_ms = (u16)fac_us * 1000;		// not ucos, each ms need to the number of systick clock 
#endif
}								    


// if OS_CRITICAL_METHOD define, then use ucosII
#ifdef OS_CRITICAL_METHOD
/*
********************************************************************************
  Function:			delay_us
  Description:		
  Input:			nus:the us number
  Output:			
  Return:			
  Others:			
********************************************************************************
*/			    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt = 0;
	u32 reload = SysTick->LOAD;	// LOAD value    	 
	ticks = nus * fac_us; 		// Need of ticks	  		 
	tcnt = 0;
	OSSchedLock();				// Stop the ucos scheduling, to prevent interrupting us delay
	told = SysTick->VAL;        // the first counter value
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if(tnow < told)
			{
				tcnt += told - tnow;
			}
			else
			{
				tcnt += reload - tnow + told;
			}	    
			told = tnow;
			if(tcnt >= ticks) 
			{
				break;
			}
		}  
	};
	OSSchedUnlock();	// enable the ucos scheduling									    
}


/*
********************************************************************************
  Function:			delay_ms
  Description:		
  Input:			nms:the ms number
  Output:			
  Return:			
  Others:			
********************************************************************************
*/	
void delay_ms(u16 nms)
{	
	if(OSRunning == TRUE)	    
	{		  
		if(nms >= fac_ms)
		{
   			OSTimeDly(nms / fac_ms);
		}
		nms %= fac_ms;  
	}
	delay_us((u32)(nms * 1000));
}
#else


// not use ucos
/*
********************************************************************************
  Function:			delay_us
  Description:		
  Input:			nus:the us number
  Output:			
  Return:			
  Others:			
********************************************************************************
*/			    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD = nus * fac_us;	// time to load	  		 
	SysTick->VAL = 0x00;        	// empty the counter
	SysTick->CTRL = 0x01 ;      	// start from bottom	 
	do
	{
		temp = SysTick->CTRL;
	}
	while((temp&0x01) && !(temp & (1 << 16)));	// waiting for time over 
	SysTick->CTRL = 0x00;       // shut off the counter
	SysTick->VAL = 0X00;       // empty the counter	 
}


/*
********************************************************************************
  Function:			delay_ms
  Description:		
  Input:			nms:the ms number
  Output:			
  Return:			
  Others:			
********************************************************************************
*/
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD = (u32)nms * fac_ms;// time to load(SysTick->LOAD is 24bit)  //max nms is 0xffffff/fac_ms = 1864
	SysTick->VAL = 0x00;           // empty the counter
	SysTick->CTRL = 0x01;          // start from bottom 
	do
	{
		temp = SysTick->CTRL;
	}
	while((temp & 0x01) && !(temp & (1 << 16)));	// waiting for time over 
	SysTick->CTRL = 0x00;		// shut off the counter
	SysTick->VAL = 0X00;		// empty the counter 	    
} 
#endif
