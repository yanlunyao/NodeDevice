/*
  ******************************************************************************
  * @file    osadc.c
  * @author  yanly
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  */
	
#include "osiwdg.h"

static void IwdgConfig(u8 pres, u16 reload);

void IwdgInit()
{
	IwdgConfig(7, 2000); //feed dog time  = 12.8s
	IWDG_ReloadCounter();
	IWDG_Enable();
}
void IwdgFeed(void)
{
  IWDG_ReloadCounter();
}
static void IwdgConfig(u8 pres, u16 reload)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //Enables write access to IWDG_PR and IWDG_RLR registers.
	IWDG_SetPrescaler(pres);
	IWDG_SetReload(reload);
}
