#include "osusart.h"


//------------------------------------------------------------------------------------//
static u8  u1_hwuart_Txing(u8 uPort);


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
