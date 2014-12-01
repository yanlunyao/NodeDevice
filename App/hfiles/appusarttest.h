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
#ifndef __APPUSARTTEST_H
#define __APPUSARTTEST_H

#include "osusart.h"

//-------------------------------------------------------------------------------//
#pragma pack(1)
typedef struct
{
  u8 info[ZIGBEE_INFO_LENGTH];
} txProtocolTest_t;

typedef struct
{
  u8 info[ZIGBEE_INFO_LENGTH]; 
} rxProtocolTest_t;

typedef struct
{
	u16          dataLen;
  txProtocolTest_t buffer;	
} txBufferTest_t;

typedef struct
{
	u16					 dataLen;
  rxProtocolTest_t buffer;
} rxBufferTest_t;
#pragma pack()
//-------------------------------------------------------------------------------//



extern void AppUsartTestInit(void);
extern void AppUsartTestProcess(void);
extern s8 AppRs485CommSendCmd(u16 dataLen, u8 * data);
#endif 
