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
#ifndef __APPUSART_H
#define __APPUSART_H

#include "osusart.h"


#define CMDSTRING		"String"
/******************************************************************************/
//#pragma  pack  (1) 
////定义的协议结构体
//typedef struct
//{
////  u8    commID;
////  u8    MsgType;
////  u8    command;
////  u8    cmdType;
////  u16   dataLength;
////  u16   sequenceNum;
//  u8    content[ZIGBEE_COM_DATA_LENGTH];
//}zigbeePro_t;
//typedef struct
//{
//  u16                msgLen;
//  zigbeePro_t*       zigbeeProMsg;     
//}zigbeeMsg_t;
//#pragma  pack  () 

typedef enum
{
  COM_STATUS_IDLE =0,
  COM_STATUS_SEND,
  COM_STATUS_RECEIVE,
  COM_STATUS_ERROR,
  COM_STATUS_FINISH
} CommStatus_t;

//typedef enum
//{
//  SEND_MASTER =0,
//  SEND_RESPOND,
//} cmdType_t;


#pragma pack(1)
typedef struct
{
  u8 cmdNum;
  u8 cmdType;
	u8 cmdString[sizeof(CMDSTRING)];
  u8 info[ZIGBEE_INFO_LENGTH];
} txProtocol_t;

typedef struct
{
  u8 cmdNum;
  u8 cmdType;
	u8 cmdString[sizeof(CMDSTRING)];
  u8 info[ZIGBEE_INFO_LENGTH]; 
} rxProtocol_t;

typedef struct
{
  enum
  {
    TX_BUFFER_EMPTY,
    TX_BUFFER_READY,
    TX_BUFFER_FULL
  }            status;
	u16          dataLen;
  txProtocol_t buffer;	
} txBuffer_t;

typedef struct
{
//  enum
//  {
//    RX_BUFFER_EMPTY,
//    RX_BUFFER_READY
//  }            status;
	u16					 dataLen;
  rxProtocol_t buffer;
} rxBuffer_t;
#pragma pack()


extern void AppUsartInit(void);
extern void AppUsartProcess(void);
extern s8 AppRs485CommSendCmd(u8 cmdNum, u8 cmdType, u16 dataLen, u8 * data);
#endif 
