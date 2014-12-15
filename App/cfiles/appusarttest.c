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
static void SmartSocketLearn(void);
static void UploadAlarmTest(void);
//------------------------------------------------------------------------------------//
//命令函数指针，通过命令编号选择命令
const functionP_t normalTransactionTest[]=
{
	UploadAlarmTest,
	SmartSocketLearn
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
			switch(_appRsRxBufTest.buffer.cmdType)  
			{
				case SET_LOAD:
					normalTransactionTest[_appRsRxBufTest.buffer.cmdNum]();
					__bufferLen = _appRsTxBufTest.dataLen ;
					memcpy(_txTempBufferTest, &(_appRsTxBufTest.buffer), __bufferLen);
					t_osscomm_sendMessage(_txTempBufferTest, __bufferLen, USART1_COM); 
					memset(&(_appRsTxBufTest.buffer), 0, sizeof(_appRsTxBufTest.buffer));
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
			return TRUE;
		}	
	}
	else
	{
		return FALSE;
	}	
	return FALSE;
}
static bool_t AppUsartTestCheckRxMessage()
{
	if(memcmp(_appRsRxBufTest.buffer.cmdString, CMDSTRING, sizeof(CMDSTRING))==0) 
	{
		if(_appRsRxBufTest.buffer.cmdNum <MAX_CMD_NUM) 
		{
			if(_appRsRxBufTest.buffer.cmdType <3) 
			{
				return TRUE;
			}	
		}
	}	
	return FALSE;
}	
//------------------------------------------------------------------------------------//
//协议命令
static void SmartSocketLearn(void)  //控制智能插座
{
	u8 temp[3];
	
	temp[0] = _appRsRxBufTest.buffer.info[1]; //smart socket number1
	temp[1] = _appRsRxBufTest.buffer.info[2];	//smart socket operation
	temp[2] = _appRsRxBufTest.buffer.info[0]; //node addr
	//temp[2] = nodeAddrValue; 
	if((temp[0]>=SOCKET_NUM)||(temp[1]==0)||(temp[1]>3)||(temp[2]>31)) //node addr ：5 pin，2^5=32
	{
		_appRsTxBufTest.buffer.cmdNum = _appRsRxBufTest.buffer.cmdNum; 
		_appRsTxBufTest.buffer.cmdType = RESPOND_CMD; 
		_appRsTxBufTest.buffer.info[0] = CMD_FAIL; 
		memcpy(_appRsTxBufTest.buffer.cmdString, CMDSTRING, sizeof(CMDSTRING));
		_appRsTxBufTest.dataLen = 2+1+sizeof(CMDSTRING);
		return;
	}
	if(RecordSmartSocketOperation(temp[0],temp[1])!= TRUE)
	{
		return ;
	}	
	Rf315SendMsg(temp);
	//返回数据
	_appRsTxBufTest.buffer.cmdNum = _appRsRxBufTest.buffer.cmdNum; 
	_appRsTxBufTest.buffer.cmdType = RESPOND_CMD; 
	_appRsTxBufTest.buffer.info[0] = CMD_SUCESS; 
	memcpy(_appRsTxBufTest.buffer.cmdString, CMDSTRING, sizeof(CMDSTRING)); 
 	_appRsTxBufTest.buffer.info[1] = _appRsRxBufTest.buffer.info[0]; 
	_appRsTxBufTest.buffer.info[2] = _appRsRxBufTest.buffer.info[1]; 
	_appRsTxBufTest.buffer.info[3] = _appRsRxBufTest.buffer.info[2]; 
	_appRsTxBufTest.dataLen = 2+4+sizeof(CMDSTRING);
}
static void UploadAlarmTest(void)
{
}	
//------------------------------------------------------------------------------------//

