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
#include "osprintf.h"

#define  USART1_COM		0
#define  USART2_COM		1
//串口1为0，串口2为1

#define ZIGBEE_INFO_LENGTH				USART_TX_DATA_SIZE-0
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
} zigbeeCommStatus_t;

//typedef enum
//{
//  SEND_MASTER =0,
//  SEND_RESPOND,
//} cmdType_t;
typedef void (* functionP_t) (void);

#pragma pack(1)
typedef struct
{
//  u1_t comId;
//  u1_t msgType;
//  u1_t cmd;
//  u8 cmdType;
//  u2_t dataLen;
//  u2_t sequence;
//  u1_t readerStatus;
  u8 info[ZIGBEE_INFO_LENGTH];
//  u1_t LRC;
} txProtocol_t;

typedef struct
{
//  u1_t comId;
//  u1_t msgType;
//  u1_t cmd;
//  u8 cmdType;
//  u2_t dataLen;
//  u2_t sequence;
//  u1_t respondType;
  u8 info[ZIGBEE_INFO_LENGTH]; 
//  u1_t LRC;
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
extern scommReturn_t  t_osscomm_ReceiveMessage(u8  *rxData, u16 *rxLen, u8 port);  //upper layer usart receive function  
extern scommReturn_t  t_osscomm_sendMessage(u8  *txData, u16 txLen, u8 port); //upper layer usart send function  
extern void AppUsartProcess(void);
extern s8 AppRs485CommSendCmd(u16 dataLen, u8 * data);
#endif 
