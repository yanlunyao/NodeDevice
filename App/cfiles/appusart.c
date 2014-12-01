#include "string.h"
#include "stdio.h"
#include "osrf315tx.h"
#include "appusart.h"
#include "appCommProtocolDef.h"
#include "appsensor.h"

#define MAX_RESPOND_CMD						3    //


static  rxBuffer_t       					_appRsRxBuf;
static  txBuffer_t       					_appRsTxBuf;		
static  rxProtocol_t       				*_pRcvProMsg = NULL;
//static  txProtocol_t       				*_pSndProMsg = NULL;
static  u8                        _txTempBuffer[ZIGBEE_INFO_LENGTH+10];
static  zigbeeCommStatus_t				_commStatus;
//static  cmdType_t                 cmdType;

//------------------------------------------------------------------------------------//
extern bool_t UpRespondConfirmed[5]; //from appsensor.c
extern u8 nodeAddrValue; //from appkey.c

//------------------------------------------------------------------------------------//
static u8  u1_hwuart_Txing(u8 uPort);
static bool_t  AppZigbeeRxMessage(void);
static bool_t AppZigbeeCheckRxMessage(void);
//------------------------------------------------------------------------------------//
//协议命令
static void GetSensorCurrentStatus(void);
static void SensorStatusChangedUpload(void);
static void ControllSmartSocket(void);
//------------------------------------------------------------------------------------//
//命令函数指针，通过命令编号选择命令
const functionP_t normalTransaction[]=
{
	ControllSmartSocket,
	GetSensorCurrentStatus,
	SensorStatusChangedUpload
};
//------------------------------------------------------------------------------------//
void AppUsartInit()
{
	OsPrintf_Init();
	OsUsartInit();
	
//	memset((u1_t *)_rsvBuf, 0x00, sizeof(_rsvBuf));
//  memset((u1_t *)_sndBuf, 0x00, sizeof(_sndBuf));
  memset((u8 *)&_appRsRxBuf, 0x00, sizeof(_appRsRxBuf));
  memset((u8 *)&_appRsTxBuf, 0x00, sizeof(_appRsTxBuf));
  memset(_txTempBuffer, 0x00, sizeof(_txTempBuffer));
//  _appRsRxBuf.elockProMsg = (zigbeePro_t*)_rsvBuf;
//  _appRsTxBuf.elockProMsg = (zigbeePro_t*)_sndBuf;
  _pRcvProMsg = &(_appRsRxBuf.buffer);
//  _pSndProMsg = _appRsTxBuf.zigbeeProMsg;
	
	_commStatus = COM_STATUS_IDLE;
	
}	
//------------------------------------------------------------------------------------//
void AppUsartProcess()
{
	u16 __bufferLen;
  switch(_commStatus)
  {
    case COM_STATUS_IDLE:
			if(_appRsTxBuf.status == TX_BUFFER_READY)
			{
				//有发送数据要发送
				_commStatus = COM_STATUS_SEND;
			}
			else
			{
				_commStatus = COM_STATUS_RECEIVE;
			}	
    break;
    case COM_STATUS_SEND:  //主动发才会进入
			__bufferLen = _appRsTxBuf.dataLen;
		  //_appRsTxBuf.buffer.cmdType = SET_LOAD;
		  memcpy(_txTempBuffer, &(_appRsTxBuf.buffer), __bufferLen);
		  t_osscomm_sendMessage(_txTempBuffer, __bufferLen, USART1_COM);
		  
		  _appRsTxBuf.status = TX_BUFFER_EMPTY; //发送完成后置
			_commStatus = COM_STATUS_RECEIVE;
    break;
    case COM_STATUS_RECEIVE:
			if((AppZigbeeRxMessage()) == TRUE)
			{
				switch(_appRsRxBuf.buffer.info[1])   //接收com数据有两种类型：上行的normal，上行的respond
				{
					case SET_LOAD:
						normalTransaction[_appRsRxBuf.buffer.info[2]]();
						//_commStatus = COM_STATUS_SEND;
						__bufferLen = _appRsTxBuf.dataLen ;
					 // _appRsTxBuf.buffer.cmdType = 0x7e;
						memcpy(_txTempBuffer, &(_appRsTxBuf.buffer), __bufferLen);
						t_osscomm_sendMessage(_txTempBuffer, __bufferLen, USART1_COM); //接收后响应直接发
					break;
          case RESPOND_CMD:
						normalTransaction[_appRsRxBuf.buffer.info[2]](); //上行的respond不用再发
					  _commStatus = COM_STATUS_IDLE;
					break;	
					default:
						_commStatus = COM_STATUS_IDLE;
					break;
					
				}	
			}
			else
			{
				_commStatus = COM_STATUS_IDLE;
			}	
    break;
//    case COM_STATUS_ERROR:
//    break;
//    case COM_STATUS_FINISH:
//      _commStatus = COM_STATUS_IDLE;
//    break;
    default:
      _commStatus = COM_STATUS_IDLE;
    break;
  }
}	
//------------------------------------------------------------------------------------//
//接收串口处理：检验接收数据
static  bool_t  AppZigbeeRxMessage()
{
	if(t_osscomm_ReceiveMessage((u8 *)_pRcvProMsg, (u16 *)&_appRsRxBuf.dataLen, USART1_COM) == SCOMM_RET_OK)
	{
	  if((AppZigbeeCheckRxMessage())==TRUE)
		{
			if(_appRsRxBuf.buffer.info[0] == 0x7e)
			{
				if(_appRsRxBuf.buffer.info[2] < MAX_RESPOND_CMD)
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
static bool_t AppZigbeeCheckRxMessage()
{
	return TRUE;
}	
/********************************************************************************
串口接收函数
********************************************************************************/
scommReturn_t  t_osscomm_ReceiveMessage(u8  *rxData, u16 *rxLen, u8 port)  //upper layer usart receive function  
{
  scommRcvBuf_t    __temp;
  scommReturn_t    __rt;
  
  __temp.pscommRcvContent = rxData;
	
	switch(port)
	{
		case USART1_COM:
			__rt = (scommReturn_t)t_hwuart1_ReceiveMessage(&__temp);
		break;
		case USART2_COM:
			__rt = (scommReturn_t)t_hwuart0_ReceiveMessage(&__temp);
		break;
    default: return SCOMM_RET_PORT_ERR;
	}	
  *rxLen = __temp.scommRcvLen;
  return (__rt);
}
/********************************************************************************
串口发送函数
********************************************************************************/
scommReturn_t  t_osscomm_sendMessage(u8  *txData, u16 txLen, u8 port)
{
	scommTxBuf_t   __temp;
  scommReturn_t  __rt;
  
  __temp.scommTxLen = txLen;
  __temp.pscommTxContent = txData;
	
	switch(port)
	{
		case USART1_COM:
			__rt = (scommReturn_t)t_hwuart1_SendMessage(&__temp);
		break;
		case USART2_COM:
			__rt = (scommReturn_t)t_hwuart0_SendMessage(&__temp);
		break;
    default: return SCOMM_RET_PORT_ERR;
	}	
  while(u1_hwuart_Txing(port));
  return __rt;
}
//------------------------------------------------------------------------------------//
//发送等待
static u8  u1_hwuart_Txing(u8 uPort)
{
  switch(uPort)
  {
    case  USART1_COM:
      return (u1_hwuart1_txing());
    case  USART2_COM:
      return (u1_hwuart0_txing());   
    default:
      return FALSE;
  }
}
/********************************************************************************
串口缓存发送命令
********************************************************************************/
s8 AppRs485CommSendCmd(u16 dataLen, u8 * data)
{
  if(_appRsTxBuf.status != TX_BUFFER_EMPTY)  
  {
		#ifdef DEBUG_PRINTF
    printf("RS485 TX buffer not enough! \r\n");
		#endif
    return(-1);
  }
  if(dataLen > ZIGBEE_INFO_LENGTH)
  {
    return(-2);
  }
  memset(_appRsTxBuf.buffer.info, 0, ZIGBEE_INFO_LENGTH);
  memcpy(_appRsTxBuf.buffer.info, data, dataLen);
	_appRsTxBuf.dataLen = dataLen;
	_appRsTxBuf.status = TX_BUFFER_READY; 
  return(0);
}
//------------------------------------------------------------------------------------//
//协议命令
static void ControllSmartSocket(void)  //控制智能插座
{
	u8 temp[3];
	_appRsTxBuf.buffer.info[0] = 0x7e;
	_appRsTxBuf.buffer.info[1] = 3;
	_appRsTxBuf.buffer.info[2] =1;
	_appRsTxBuf.dataLen = 3;
	
	temp[0] = 0; //smart socket number1
	temp[1] = SMARTSOCKET_INVERT_ADDR;
	temp[2] = nodeAddrValue;
	Rf315SendMsg(temp);
}	
static void GetSensorCurrentStatus(void)
{
	_appRsTxBuf.buffer.info[0] = 0x7e;
	_appRsTxBuf.buffer.info[1] =3;
	_appRsTxBuf.buffer.info[2] =2;
	_appRsTxBuf.dataLen = 3;
}	
static void SensorStatusChangedUpload(void)
{
//	_appRsTxBuf.buffer.info[0] = 0x7e;
//	_appRsTxBuf.buffer.info[1] =3;
//	_appRsTxBuf.dataLen = 3;
	UploadRspondConfirmed(_appRsRxBuf.buffer.info[3]);
}	
//------------------------------------------------------------------------------------//

