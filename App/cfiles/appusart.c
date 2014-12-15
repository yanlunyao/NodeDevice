#include "string.h"
#include "stdio.h"
#include "osrf315tx.h"
#include "appusart.h"
#include "appCommProtocolDef.h"
#include "appsensor.h"




static  rxBuffer_t       					_appRsRxBuf;
static  txBuffer_t       					_appRsTxBuf;		
static  rxProtocol_t       				*_pRcvProMsg = NULL;
//static  txProtocol_t       				*_pSndProMsg = NULL;
static  u8                        _txTempBuffer[ZIGBEE_INFO_LENGTH+10];
static  CommStatus_t				_commStatus;
//static  cmdType_t                 cmdType;
//------------------------------------------------------------------------------------//
//extern variables------------------------------------------------------------------------------------//
extern u8 nodeAddrValue; //from appkey.c
//------------------------------------------------------------------------------------//
static bool_t AppZigbeeCheckRxMessage(void);
static  bool_t  AppZigbeeRxMessage(void);
//------------------------------------------------------------------------------------//
//协议命令
static void GetSensorCurrentStatus(void);
static void SensorStatusChangedUpload(void);
static void ControllSmartSocket(void);
//------------------------------------------------------------------------------------//
//命令函数指针，通过命令编号选择命令
const functionP_t normalTransaction[]=
{
	SensorStatusChangedUpload,
	ControllSmartSocket
};
//------------------------------------------------------------------------------------//
void AppUsartInit()
{
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
		
		  memcpy(_txTempBuffer, &(_appRsTxBuf.buffer), __bufferLen);
		  t_osscomm_sendMessage(_txTempBuffer, __bufferLen, USART2_COM);
		  memset(&(_appRsTxBuf.buffer), 0, sizeof(_appRsTxBuf.buffer));
		
		  _appRsTxBuf.status = TX_BUFFER_EMPTY; //发送完成后置
			_commStatus = COM_STATUS_RECEIVE;
    break;
    case COM_STATUS_RECEIVE:
			if((AppZigbeeRxMessage()) == TRUE)
			{
				switch(_appRsRxBuf.buffer.cmdType)   //接收com数据有两种类型：上行的normal，上行的respond
				{
					case SET_LOAD:
						normalTransaction[_appRsRxBuf.buffer.cmdNum]();
						__bufferLen = _appRsTxBuf.dataLen ;
					
						memcpy(_txTempBuffer, &(_appRsTxBuf.buffer), __bufferLen);
						t_osscomm_sendMessage(_txTempBuffer, __bufferLen, USART2_COM); //接收后响应直接发
					  memset(&(_appRsTxBuf.buffer), 0, sizeof(_appRsTxBuf.buffer));
					
					break;
          case RESPOND_CMD:
						normalTransaction[_appRsRxBuf.buffer.cmdNum](); //上行的respond不用再发
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
	if(t_osscomm_ReceiveMessage((u8 *)_pRcvProMsg, (u16 *)&_appRsRxBuf.dataLen, USART2_COM) == SCOMM_RET_OK)
	{
	  if((AppZigbeeCheckRxMessage())==TRUE)
		{
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		return FALSE;
	}
}
static bool_t AppZigbeeCheckRxMessage()
{
	if(memcmp(_appRsRxBuf.buffer.cmdString, CMDSTRING, sizeof(CMDSTRING))==0)
	{
		if(_appRsRxBuf.buffer.cmdNum <MAX_CMD_NUM)
		{
			if(_appRsRxBuf.buffer.cmdType <3)
			{
				return TRUE;
			}	
		}
	}	
	return FALSE;
}	

/********************************************************************************
串口缓存发送命令
********************************************************************************/
s8 AppRs485CommSendCmd(u8 cmdNum, u8 cmdType, u16 dataLen, u8 * data)
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
	if(cmdNum >MAX_CMD_NUM-1)
	{
		return(-3);
	}	
	if(cmdType >MAX_CMDTYPE-1)
	{
		return (-4);
	}	
  memset(_appRsTxBuf.buffer.info, 0, ZIGBEE_INFO_LENGTH);
	_appRsTxBuf.buffer.cmdNum = cmdNum;
	_appRsTxBuf.buffer.cmdType = cmdType;
	memcpy(_appRsTxBuf.buffer.cmdString, CMDSTRING, sizeof(CMDSTRING));
	memcpy(_appRsTxBuf.buffer.info, data, dataLen);
	_appRsTxBuf.dataLen = dataLen+2+sizeof(CMDSTRING);
	_appRsTxBuf.status = TX_BUFFER_READY; 
  return(0);
}
//------------------------------------------------------------------------------------//
//协议命令
static void ControllSmartSocket(void)  //控制智能插座
{
	u8 temp[3];
	
	temp[0] = _appRsRxBuf.buffer.info[1]; //smart socket number1
	temp[1] = _appRsRxBuf.buffer.info[2];	//smart socket operation
	temp[2] = _appRsRxBuf.buffer.info[0]; //node addr
	//temp[2] = nodeAddrValue; 
	if((temp[0]>=SOCKET_NUM)||(temp[1]==0)||(temp[1]>3)||(temp[2]>31)) //node addr ：5 pin，2^5=32
	{
		_appRsTxBuf.buffer.cmdNum = _appRsRxBuf.buffer.cmdNum;
		_appRsTxBuf.buffer.cmdType = RESPOND_CMD;
		_appRsTxBuf.buffer.info[0] = CMD_FAIL;
		memcpy(_appRsTxBuf.buffer.cmdString, CMDSTRING, sizeof(CMDSTRING));
		_appRsTxBuf.dataLen = 2+1+sizeof(CMDSTRING);
		return;
	}
	if(RecordSmartSocketOperation(temp[0],temp[1])!= TRUE)
	{
		return ;
	}	
	Rf315SendMsg(temp);
	//返回数据
	_appRsTxBuf.buffer.cmdNum = _appRsRxBuf.buffer.cmdNum;
	_appRsTxBuf.buffer.cmdType = RESPOND_CMD;
	_appRsTxBuf.buffer.info[0] = CMD_SUCESS;
	memcpy(_appRsTxBuf.buffer.cmdString, CMDSTRING, sizeof(CMDSTRING));
 	_appRsTxBuf.buffer.info[1] = _appRsRxBuf.buffer.info[0];
	_appRsTxBuf.buffer.info[2] = _appRsRxBuf.buffer.info[1];
	_appRsTxBuf.buffer.info[3] = _appRsRxBuf.buffer.info[2];
	_appRsTxBuf.dataLen = 2+4+sizeof(CMDSTRING);
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
	if((_appRsRxBuf.buffer.info[0] ==CMD_SUCESS)&&(_appRsRxBuf.buffer.info[1] <SENSOR_NUM))  //参数错误不做处理，软件会继续重发此命令
	{
		UploadRspondConfirmed(_appRsRxBuf.buffer.info[1]);
	}
}	
//------------------------------------------------------------------------------------//

