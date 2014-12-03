#include "string.h"
#include "stdio.h"
#include "osrf315tx.h"
#include "appusarttest.h"
#include "appCommProtocolDef.h"



#define MAX_RESPOND_CMD_TEST						1    //


static  rxBufferTest_t       					_appRsRxBufTest;
static  txBufferTest_t       					_appRsTxBufTest;		
static  rxProtocolTest_t       				*_pRcvProMsgTest = NULL;
static  u8                        _txTempBufferTest[200];
static  u8				_commStatusTest;

//------------------------------------------------------------------------------------//
extern u8 nodeAddrValue; //from appkey.c

//------------------------------------------------------------------------------------//
static bool_t  AppUsartTestRxMessage(void);
static bool_t AppUsartTestCheckRxMessage(void);
//------------------------------------------------------------------------------------//
//协议命令
static void ControllSmartSocket(void);
//------------------------------------------------------------------------------------//
//命令函数指针，通过命令编号选择命令
const functionP_t normalTransactionTest[]=
{
	ControllSmartSocket,
};
//------------------------------------------------------------------------------------//
void AppUsart1Init()
{
	OsPrintf_Init();
//	memset((u1_t *)_rsvBuf, 0x00, sizeof(_rsvBuf));
//  memset((u1_t *)_sndBuf, 0x00, sizeof(_sndBuf));
  memset((u8 *)&_appRsRxBufTest, 0x00, sizeof(_appRsRxBufTest));
  memset((u8 *)&_appRsTxBufTest, 0x00, sizeof(_appRsTxBufTest));
  memset(_txTempBufferTest, 0x00, sizeof(_txTempBufferTest));
//  _appRsRxBufTest.elockProMsg = (zigbeePro_t*)_rsvBuf;
//  _appRsTxBufTest.elockProMsg = (zigbeePro_t*)_sndBuf;
  _pRcvProMsgTest = &(_appRsRxBufTest.buffer);
//  _pSndProMsg = _appRsTxBufTest.zigbeeProMsg;
	
	_commStatusTest = 1;
	
}	
//------------------------------------------------------------------------------------//
void AppUsart1Process()
{
	u16 __bufferLen;
  if(_commStatusTest == 1)
  {
		if((AppUsartTestRxMessage()) == TRUE)
		{
			switch(_appRsRxBufTest.buffer.info[1])  
			{
				case SET_LOAD:
					normalTransactionTest[_appRsRxBufTest.buffer.info[2]]();
					__bufferLen = _appRsTxBufTest.dataLen ;
					memcpy(_txTempBufferTest, &(_appRsTxBufTest.buffer), __bufferLen);
					t_osscomm_sendMessage(_txTempBufferTest, __bufferLen, USART1_COM); 
				break;
				case RESPOND_CMD:
				break;	
				default:
				break;
					
			}	
		}
  }
}	
//------------------------------------------------------------------------------------//
//接收串口处理：检验接收数据
static  bool_t  AppUsartTestRxMessage()
{
	if(t_osscomm_ReceiveMessage((u8 *)_pRcvProMsgTest, (u16 *)&_appRsRxBufTest.dataLen, USART1_COM) == SCOMM_RET_OK)
	{
	  if((AppUsartTestCheckRxMessage())==TRUE)
		{
			if(_appRsRxBufTest.buffer.info[0] == 0x7e)
			{
				if(_appRsRxBufTest.buffer.info[2] < MAX_RESPOND_CMD_TEST)
				{}
				else
				{
					return FALSE;
				}	
			}	
			else
			{
				return FALSE;
			}	
		}	
	}
	else
	{
		return FALSE;
	}	
	return TRUE;
}
static bool_t AppUsartTestCheckRxMessage()
{
	return TRUE;
}	
//------------------------------------------------------------------------------------//
//协议命令
static void ControllSmartSocket(void)  //控制智能插座
{
	u8 temp[3];
	_appRsTxBufTest.buffer.info[0] = 0x7e;
	_appRsTxBufTest.buffer.info[1] = 3;
	_appRsTxBufTest.buffer.info[2] =1;
	_appRsTxBufTest.dataLen = 3;
	
	
	temp[0] = _appRsRxBufTest.buffer.info[3]; //smart socket number1
	temp[1] = _appRsRxBufTest.buffer.info[4];
	temp[2] = nodeAddrValue; //
	Rf315SendMsg(temp);
}
//------------------------------------------------------------------------------------//

