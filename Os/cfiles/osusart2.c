/**
  ******************************************************************************
  * @file    osusart2.c 
  * @author  
  * @version 
  * @date    
  * @brief   USART 串口收发均在后台中断中中完成 ,此串口为USART2
  ******************************************************************************
  * @COPYRIGHT
  */ 
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "osusart2.h"
#include "osqtmr.h"

/*valiable*************************************************************************************/
static  volatile  u16               _u0txCnt;
static  volatile  u16               _u0rxCnt;
static  volatile  u8               _u0WrTxRegEn;
static  volatile  u8               _u0RxEn;        //rx enable flag
static  volatile  u8               _u0TxEn;        //tx enable flag
static  volatile  u8               _u0rxErrflag;      //uart line status error flag
static  volatile  u8               _u0TxCharTmr;    /*uart tx timer,soft wotchdog*/ 
static  volatile  u8               _u0RxCharTmr;    /*rx char internal time*/
static  volatile  u16               _u0FrameTmr;     /*rx frame internal time*/
static  volatile  u8               _u0CharTmrRestart;
static  volatile  u16              _u0FrameTmrRestart;   
static  volatile  u16              _u0txCnt;
static  volatile  u16              _u0rxCnt;
//static  volatile  scommmode_t      _u0Mode;
static  volatile  scommTxBuf_t       *_pu0TxPtr;
static  volatile  scommRcvBuf_t       *_pu0RxPtr;
static  volatile  scommRcvBuf_t       _u0RcvBuf;
static  volatile  u8               _u0RcvArea[USART_RX_DATA_SIZE];
static  volatile  uRxBufStatus_t   _u0RcvStates;
static	volatile	u8							 _u0Txing;

					 
USART_TypeDef* COM_USART[COMn] = {EVAL_COM1, EVAL_COM2}; 

GPIO_TypeDef* COM_TX_PORT[COMn] = {EVAL_COM1_TX_GPIO_PORT, EVAL_COM2_TX_GPIO_PORT};
 
GPIO_TypeDef* COM_RX_PORT[COMn] = {EVAL_COM1_RX_GPIO_PORT, EVAL_COM2_RX_GPIO_PORT};
 
const uint32_t COM_USART_CLK[COMn] = {EVAL_COM1_CLK, EVAL_COM2_CLK};

const uint32_t COM_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK, EVAL_COM2_TX_GPIO_CLK};
 
const uint32_t COM_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK, EVAL_COM2_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[COMn] = {EVAL_COM1_TX_PIN, EVAL_COM2_TX_PIN};

const uint16_t COM_RX_PIN[COMn] = {EVAL_COM1_RX_PIN, EVAL_COM2_RX_PIN};		

///* Private typedef -----------------------------------------------------------*/
//typedef enum 
//{
//  COM1 = 0,  //usart2
//  COM2 = 1   //usart1
//} COM_TypeDef;  
/* Private define ------------------------------------------------------------*/
//#ifndef EVAL_COM1_STR
//    #define EVAL_COM1_STR                    "USART2"
//#endif

//#ifndef EVAL_COM2_STR
//    #define EVAL_COM2_STR                    "USART1"
//#endif

//#ifdef USE_USART2
//    #define EVAL_COMx                        EVAL_COM1
//    #define EVAL_COMx_STR                    EVAL_COM1_STR
//#else
//    #define EVAL_COMx                        EVAL_COM2
//    #define EVAL_COMx_STR                    EVAL_COM2_STR
//#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
static void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
static void Usart_NVIC_Configuration(void);
static void UsartTaskInit(void);
static void UsartConfig(void);
static void UsartRstVariable(void);

static  void  _hwuart0_txIntrSer(void);
static  void  _hwuart0_rxIntrSer(void);
static  void  _hwuart0_1msTask(void);
static  void  _hwuart0_ScanRxProcess(void);

  
/* Private functions ---------------------------------------------------------*/

void OsUsartInit(void)
{
	UsartTaskInit();
  Usart_NVIC_Configuration();/* 配置串口中断向量表 */
	UsartConfig();
}
static void UsartTaskInit(void)
{
	UsartRstVariable();
	t_systmr_insertQuickTask(_hwuart0_1msTask, 1, OSTMR_PERIODIC);            
  t_systmr_insertQuickTask(_hwuart0_ScanRxProcess, 10, OSTMR_PERIODIC);   
}
static void UsartConfig(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = 115200;//115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

//#ifdef USE_USART2
  STM_EVAL_COMInit(COM1, &USART_InitStructure);
//#else
//  STM_EVAL_COMInit(COM2, &USART_InitStructure);
//#endif
//  USART_ITConfig(EVAL_COMx, USART_IT_TXE, ENABLE);
	USART_ITConfig(EVAL_COM1, USART_IT_TXE, DISABLE); //add yan141111
  USART_ITConfig(EVAL_COM1, USART_IT_RXNE, ENABLE);
}

/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:    
  *     @arg COM1
  *     @arg COM2  
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
static void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);

  if (COM == COM1)
  {
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }
  else
  {
    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(COM_USART[COM], USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(COM_USART[COM], ENABLE);
}



/******interupt fun handler***********/
void _USART2_IRQHandler(void)
{
  if (USART_GetITStatus(EVAL_COM1_USARTx, USART_IT_RXNE) != RESET)
  {
    /* received data */
		_hwuart0_rxIntrSer();
  }

  /* If overrun condition occurs, clear the ORE flag 
                              and recover communication */
  if (USART_GetFlagStatus(EVAL_COM1_USARTx, USART_FLAG_ORE) != RESET)
  {
    (void)USART_ReceiveData(EVAL_COM1_USARTx);
		_u0rxErrflag = TRUE;
  }

  if(USART_GetITStatus(EVAL_COM1_USARTx, USART_IT_TXE) != RESET)
  {   
    /* Write one byte to the transmit data register */
		//if(_u0WrTxRegEn == TRUE)
		{	
			_hwuart0_txIntrSer();
		}
  }
}
static void Usart_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	
  NVIC_InitStructure.NVIC_IRQChannel = EVAL_COM1_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_USART2_PreemptionPriority;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_USART2_SubPriority;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

static void UsartRstVariable(void)
{
//  _u0TxEn = DISABLE;              /*disable tx*/
//  _u0RxEn = DISABLE;              /*disable rx*/
//  _u0Mode = SCOMM_MODE_NONE;       /*none mode*/

  _u0rxErrflag = 0;
  _u0txCnt = 0;
  _u0rxCnt = 0;
  _u0Txing = FALSE;
//	
  memset((u8 *)_u0RcvArea, 0x00, sizeof(_u0RcvArea));
  memset((u8 *)&_u0RcvBuf, 0x00, sizeof(_u0RcvBuf));
  _u0RcvBuf.pscommRcvContent = (u8 *)_u0RcvArea;
  _pu0RxPtr = &_u0RcvBuf;
  
  _u0RcvStates = URX_BUF_IDLE;
  _pu0TxPtr = NULL;
  _u0TxCharTmr = 0;
  _u0RxCharTmr = 0;
//  _u0FrameTmr = 0;
//  _u0WrTxRegEn = FALSE;
	USART_ITConfig(EVAL_COM1, USART_IT_TXE, DISABLE); //add yan141111
  _u0CharTmrRestart = UART0_CHAR_TMR_RESTART;     //default set
//  _u0FrameTmrRestart = UART0_FRM_TMR_RESTART;     //default set
}

static  void  _hwuart0_rxIntrSer(void)
{
  if(_u0RcvStates == URX_BUF_IDLE || _u0RcvStates == URX_BUF_BUSY) //and (_u0RxEn == ENABLE)) 
	{
	  if(_u0RcvStates == URX_BUF_IDLE)
	  {
	    _u0RcvStates = URX_BUF_BUSY;
	    _u0rxCnt = 0;
	  }	
    _pu0RxPtr->pscommRcvContent[_u0rxCnt] = USART_ReceiveData(EVAL_COM1_USARTx);
		_u0rxCnt++;
		_u0RxCharTmr = _u0CharTmrRestart;  //一直循坏扫描这个值，值为0时表示串口帧接收结束
	}	
	else
	{//clear rx fifo
		(void)USART_ReceiveData(EVAL_COM1_USARTx);
	}
}

static  void  _hwuart0_txIntrSer(void)
{
  if(_pu0TxPtr != NULL)
  {
    if(_u0txCnt < _pu0TxPtr->scommTxLen) 
    {
			/* Write one byte to the transmit data register */
			USART_SendData(EVAL_COM1_USARTx, _pu0TxPtr->pscommTxContent[_u0txCnt]);
			_u0txCnt++;
      _u0TxCharTmr = _u0CharTmrRestart;
    }
    else
    {
//      _u0WrTxRegEn = FALSE;   
			USART_ITConfig(EVAL_COM1_USARTx, USART_IT_TXE, DISABLE); //add yan141111
      _u0Txing = FALSE;
      
//			_u0RxEn = ENABLE;      
//      if(_u0Mode == SCOMM_MODE_MASTER)
//      {/*provision to rx */   
//        _u0rxErrflag = FALSE;
//        _u0FrameTmr = _u0FrameTmrRestart;  /*主机,发送完成,则启动等待计时器等待接收完*/
//      }
    }
  }
	else
	{
		USART_ITConfig(EVAL_COM1, USART_IT_TXE, DISABLE); //add yan141111
	}
}

static  void  _hwuart0_1msTask(void)
{
  if(_u0TxCharTmr != 0)
  {//tx timer timeout
    _u0TxCharTmr--;
    if(_u0TxCharTmr == 0 && _u0Txing == TRUE)  //use delay to judge sent data(one frame) is over
    {
      _u0Txing     = FALSE;
//      _u0WrTxRegEn = FALSE;
			USART_ITConfig(EVAL_COM1, USART_IT_TXE, DISABLE); //add yan141111
      _u0txCnt     = 0;
      _pu0TxPtr    = NULL;
    }
  }
  
  if(_u0RxCharTmr != 0)
  {
    _u0RxCharTmr--;
    if(_u0RxCharTmr == 0)  //use delay to judge receive data(one frame) is over
    {
      if(_u0RcvStates == URX_BUF_BUSY)
      {
        _u0RcvStates = URX_BUF_COMPL;
      }  
    }
  }

//  if(_u0FrameTmr != 0)  //this flag use to control the time of switch master or slave mode, think about use or not in future!!!!
//  {
//    _u0FrameTmr--;
//    if(_u0FrameTmr == 0)
//    {
//      if(_u0Mode == SCOMM_MODE_MASTER)
//      {//master...
//        _u0RxEn = DISABLE;
//        _u0TxEn = ENABLE;
//      }
//      else if(_u0Mode == SCOMM_MODE_SLAVE) 
//      {//slave..
//        _u0RxEn = ENABLE;
//        _u0TxEn = DISABLE;  
//      }      
//    }
//  }  
}


static  void  _hwuart0_ScanRxProcess(void)
{
  if(_u0rxErrflag)
  {//err...clear all variable
    _u0RcvStates = URX_BUF_IDLE;    
    _u0RxCharTmr = TIMEOUT;
    _u0rxCnt = 0;   

//    _u0RxEn = ENABLE;
//    if(_u0Mode == SCOMM_MODE_MASTER)
//    {
//      _u0FrameTmr = _u0FrameTmrRestart;
//    }
//    else
//    {
//      _u0FrameTmr = 0;     /*restart frame timer*/
//    }     
    
    _u0rxErrflag = FALSE;   
  }  
  
  if(_u0RcvStates == URX_BUF_COMPL)
  {
    /*rcv data complete*/
    _pu0RxPtr->scommRcvLen = _u0rxCnt;
    _u0RcvStates = URX_BUF_READY;

//    _u0TxEn = ENABLE;
//    if(_u0Mode == SCOMM_MODE_MASTER)
//    {
//      _u0FrameTmr = 0;
//    }
//    else
//    {
//      _u0FrameTmr = _u0FrameTmrRestart;     /*restart frame timer*/
//    }
  }
}

scommReturn_t  t_hwuart0_ReceiveMessage(scommRcvBuf_t *rxMsg)
{
  volatile  scommReturn_t    __rcv = SCOMM_RET_NONE; 
  
  if(rxMsg != NULL)
  {
		if(_u0Txing == FALSE)
		{	
//	    if(_u0RxEn == TRUE)
//	    {
	      if(_u0RcvStates == URX_BUF_READY)
	      {
	        /*copy uart hardware data to upon layer*/
	        rxMsg->scommRcvLen = _pu0RxPtr->scommRcvLen;
	        memcpy((u8 *)(rxMsg->pscommRcvContent), (u8 *)(_pu0RxPtr->pscommRcvContent), _pu0RxPtr->scommRcvLen);

	        _pu0RxPtr->scommRcvLen = 0;   //reset value
	        _u0RcvStates = URX_BUF_IDLE; 
	        __rcv = SCOMM_RET_OK;
	      }
	      else
	      {
	        __rcv = SCOMM_RET_NOREADY;
	      }
//	    }
//	    else
//	    {
//	      __rcv = SCOMM_RET_TIMEOUT;
//	    }
		}
		else
		{
			__rcv = SCOMM_RET_TXING;
		}	
  }
  else
  {
    __rcv = SCOMM_RET_ERR_PARAM;
  }
  
  return (__rcv);
}
scommReturn_t  t_hwuart0_SendMessage(scommTxBuf_t*  txMsg)
{
//  if(_u0TxEn == ENABLE)
//  {
//    if(_u0Mode == SCOMM_MODE_MASTER)
//    {/*master.. provision to rx */
//    }
//    else
//    {/*slave,tx denote process complete*/
//      _u0FrameTmr = TIMEOUT;  
//    }

    while(u1_hwuart0_txing());
   
    if((txMsg != NULL) && (txMsg->scommTxLen > 0))
    {
      _u0TxCharTmr = _u0CharTmrRestart;
      _pu0TxPtr = txMsg;    /*get tx data pointer*/
      _u0txCnt = 0;//1;   
//      _u0WrTxRegEn = TRUE;    
			USART_ITConfig(EVAL_COM1, USART_IT_TXE, ENABLE); //add yan141111
			_u0Txing = TRUE;

//			/* Write first byte to the transmit data register */
//			USART_SendData(USARTx, _pu0TxPtr->pscommTxContent[0]);//[_u0txCnt-1]);
      return(SCOMM_RET_OK);
    }
    else
    {
      return(SCOMM_RET_ERR_PARAM);
    }
//  }
//  else
//  {
//    return(SCOMM_RET_TIMEOUT);
//  }
}

u8  u1_hwuart0_txing(void)
{
	return ((USART_GetITStatus(EVAL_COM1_USARTx, USART_IT_TXE) == SET) ||(_u0Txing == TRUE));
}
