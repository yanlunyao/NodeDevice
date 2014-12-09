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
static  CommStatus_t				_commStatus;
//static  cmdType_t                 cmdType;
//------------------------------------------------------------------------------------//
//extern variables------------------------------------------------------------------------------------//
extern u8 nodeAddrValue; //from appkey.c
//------------------------------------------------------------------------------------//
static bool_t AppZigbeeCheckRxMessage(void);
static  bool_t  AppZigbeeRxMessage(void);
//------------------------------------------------------------------------------------//
//Э������
static void GetSensorCurrentStatus(void);
static void SensorStatusChangedUpload(void);
static void ControllSmartSocket(void);
//------------------------------------------------------------------------------------//
//�����ָ�룬ͨ��������ѡ������
const functionP_t normalTransaction[]=
{
	ControllSmartSocket,
	GetSensorCurrentStatus,
	SensorStatusChangedUpload
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
				//�з�������Ҫ����
				_commStatus = COM_STATUS_SEND;
			}
			else
			{
				_commStatus = COM_STATUS_RECEIVE;
			}	
    break;
    case COM_STATUS_SEND:  //�������Ż����
			__bufferLen = _appRsTxBuf.dataLen;
		  //_appRsTxBuf.buffer.cmdType = SET_LOAD;
		  memcpy(_txTempBuffer, &(_appRsTxBuf.buffer), __bufferLen);
		  t_osscomm_sendMessage(_txTempBuffer, __bufferLen, USART2_COM);
		  
		  _appRsTxBuf.status = TX_BUFFER_EMPTY; //������ɺ���
			_commStatus = COM_STATUS_RECEIVE;
    break;
    case COM_STATUS_RECEIVE:
			if((AppZigbeeRxMessage()) == TRUE)
			{
				switch(_appRsRxBuf.buffer.info[1])   //����com�������������ͣ����е�normal�����е�respond
				{
					case SET_LOAD:
						normalTransaction[_appRsRxBuf.buffer.info[2]]();
						//_commStatus = COM_STATUS_SEND;
						__bufferLen = _appRsTxBuf.dataLen ;
					 // _appRsTxBuf.buffer.cmdType = 0x7e;
						memcpy(_txTempBuffer, &(_appRsTxBuf.buffer), __bufferLen);
						t_osscomm_sendMessage(_txTempBuffer, __bufferLen, USART2_COM); //���պ���Ӧֱ�ӷ�
					break;
          case RESPOND_CMD:
						normalTransaction[_appRsRxBuf.buffer.info[2]](); //���е�respond�����ٷ�
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
//���մ��ڴ��������������
static  bool_t  AppZigbeeRxMessage()
{
	if(t_osscomm_ReceiveMessage((u8 *)_pRcvProMsg, (u16 *)&_appRsRxBuf.dataLen, USART2_COM) == SCOMM_RET_OK)
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
���ڻ��淢������
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
//Э������
static void ControllSmartSocket(void)  //�������ܲ���
{
	u8 temp[3];
	
	temp[0] = _appRsRxBuf.buffer.info[3]; //smart socket number1
	temp[1] = _appRsRxBuf.buffer.info[4];	//smart socket operation
	temp[2] = _appRsRxBuf.buffer.info[5]; //node addr
	//temp[2] = nodeAddrValue; 
	if((temp[0]>=4)||(temp[1]==0)||(temp[1]>=4)||(temp[2]>=32))
	{
		return;
	}
	if(RecordSmartSocketOperation(temp[0],temp[1])!= TRUE)
	{
		return ;
	}	
	Rf315SendMsg(temp);
	//��������
	_appRsTxBuf.buffer.info[0] = 0x7e;
	_appRsTxBuf.buffer.info[1] = 3;
	_appRsTxBuf.buffer.info[2] =0x7e;
	_appRsTxBuf.dataLen = 3;
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

