/**
  ******************************************************************************
  * @file    osprintf.c 
  * @author  www.armjishu.com
  * @version STM32F10X_STDPERIPH_VERSION V3.3.0
  * @date    
  * @brief   USART  串口收发均在后台中断中中完成 ，测试串口为USART1
  ******************************************************************************
  * @COPYRIGHT
  *
  *
  * <h2><center>&copy; COPYRIGHT 2009-2011 WWW.ARMJISHU.COM 神舟系列STM32开发板</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "osqtmr.h"
#include "osprintf.h"

static  volatile  u8               _u1WrTxRegEn;
static  volatile  u8               _u1RxEn;        //rx enable flag
static  volatile  u8               _u1TxEn;        //tx enable flag
static  volatile  u8               _u1rxErrflag;      //uart line status error flag
static  volatile  u8               _u1TxCharTmr;    /*uart tx timer,soft wotchdog*/ 
static  volatile  u8               _u1RxCharTmr;    /*rx char internal time*/
static  volatile  u16               _u1FrameTmr;     /*rx frame internal time*/
static  volatile  u8               _u1CharTmrRestart;
static  volatile  u16              _u1FrameTmrRestart;   
static  volatile  u16              _u1txCnt;
static  volatile  u16              _u1rxCnt;
//static  volatile  scommmode_t      _u1Mode;
static  volatile  scommTxBuf_t       *_pu1TxPtr;
static  volatile  scommRcvBuf_t       *_pu1RxPtr;
static  volatile  scommRcvBuf_t       _u1RcvBuf;
static  volatile  u8               _u1RcvArea[UART1_RX_BUF_SIZE];
static  volatile  uRxBufStatus_t   _u1RcvStates;
static	volatile	u8							 _u1Txing;


uint8_t  USART_Tx_Buffer [USART_TX_DATA_SIZE+1];
volatile u32 USART_Tx_ptr_Out = 0;
volatile u32 USART_Tx_ptr_Store = 0;
volatile u32 TxCounter = 0;

uint8_t  USART_Rx_Buffer [USART_RX_DATA_SIZE];
volatile uint32_t USART_Rx_ptr_in = 0;
volatile uint32_t USART_Rx_Done = 0;




void USART_SendBufferData(void);
void USART_StoreBufferData(uint8_t ch);

static void Usart1TaskInit(void);
static void Usart1Config(void);
static  void  _hwuart1_txIntrSer(void);
static  void  _hwuart1_rxIntrSer(void);
static  void  _hwuart1_1msTask(void);
static  void  _hwuart1_ScanRxProcess(void);
static  void  Usart1RstVariable(void);

static void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
static void Printf_NVIC_Configuration(void);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
	
#ifdef DEBUG_PRINTF	
void OsPrintf_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	 /* 配置串口中断向量表 */
  Printf_NVIC_Configuration();

  USART_InitStructure.USART_BaudRate = 115200;//115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM2, &USART_InitStructure);

  USART_ITConfig(EVAL_COM2, USART_IT_TXE, ENABLE);

  USART_ITConfig(EVAL_COM2, USART_IT_RXNE, ENABLE);

  USART_Tx_ptr_Out = 0;
  USART_Tx_ptr_Store = 0;
  TxCounter = 0;
  USART_Rx_ptr_in = 0;
  USART_Rx_Done = 0;
}
#else
void OsPrintf_Init(void)
{
	Usart1TaskInit();
  Printf_NVIC_Configuration();/* 配置串口中断向量表 */
	Usart1Config();
}
#endif
#ifndef DEBUG_PRINTF	
static void Usart1TaskInit(void)
{
	Usart1RstVariable();
	t_systmr_insertQuickTask(_hwuart1_1msTask, 1, OSTMR_PERIODIC);            
  t_systmr_insertQuickTask(_hwuart1_ScanRxProcess, 10, OSTMR_PERIODIC);   
}
static void Usart1Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

//#ifdef USE_USART2
//  STM_EVAL_COMInit(COM1, &USART_InitStructure);
//#else
  STM_EVAL_COMInit(COM2, &USART_InitStructure);
//#endif
  /* Enable the EVAL_COM1 Transmoit interrupt: this interrupt is generated when the 
     EVAL_COM1 transmit data register is empty */  
//  USART_ITConfig(EVAL_COM2, USART_IT_TXE, ENABLE);
	USART_ITConfig(EVAL_COM2, USART_IT_TXE, DISABLE); //add yan141111

  /* Enable the EVAL_COM1 Receive interrupt: this interrupt is generated  
     when the EVAL_COM1 receive data register is not empty */
  USART_ITConfig(EVAL_COM2, USART_IT_RXNE, ENABLE);
}
#endif
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  * @by     WWW.ARMJISHU.COM 
  */
//PUTCHAR_PROTOTYPE
//{
//  /* ARMJISHU 零等待的Printf串口打印函数将数据
//            写入Buffer后立即返回(Buffer未满时) */
//  USART_StoreBufferData((uint8_t) ch);

//  return ch;
//}
PUTCHAR_PROTOTYPE
{
  /* ARMJISHU 零等待的Printf串口打印函数将数据
            写入Buffer后立即返回(Buffer未满时) */
  USART_StoreBufferData((uint8_t) ch);

  return ch;
}


/**
  * @brief  put char to the Tx Buffer
            将来自printf的数据写入Buffer,
  * @param  None
  * @retval None
  * @by     WWW.ARMJISHU.COM 
  */
void USART_StoreBufferData(uint8_t ch)
{
    while(TxCounter == USART_TX_DATA_SIZE);
    if(TxCounter < USART_TX_DATA_SIZE)
    {
        /* Write one byte to the transmit data register */
        USART_Tx_Buffer[USART_Tx_ptr_Store++] = ch;
        USART_Tx_ptr_Store = USART_Tx_ptr_Store & 0xFF;
        TxCounter++;
    }
    USART_ITConfig(EVAL_COM2, USART_IT_TXE, ENABLE);
}


/**
  * @brief  put char to the HyperTerminal in the interrupt
            在串口中断中发送Buffer中的数据,Buffer空时关闭中断
  * @param  None
  * @retval None
  * @by     WWW.ARMJISHU.COM 
  */
void USART_SendBufferData(void)
{
    if(TxCounter > 0)
    {
        /* Write one byte to the transmit data register */
        USART_SendData(EVAL_COM2, USART_Tx_Buffer[USART_Tx_ptr_Out++]);
        TxCounter--;
        USART_Tx_ptr_Out = USART_Tx_ptr_Out & 0xFF;
    }
    else
    {
      /* Disable the EVAL_COM1 Transmit interrupt */
      USART_ITConfig(EVAL_COM2, USART_IT_TXE, DISABLE);
    }
}
void USART_GetInputString(void)
{
    uint8_t mychar = 0;
    
    mychar = USART_ReceiveData(EVAL_COM2);
    if (USART_Rx_Done == 0)
    {
        if (mychar == '\r')
        {
             USART_Rx_Buffer[USART_Rx_ptr_in] = '\0';
             USART_Rx_Done = 1;
        }
        else if (mychar == '\b') /* Backspace */
        {
            if (USART_Rx_ptr_in > 0)
            {
              printf("\b \b");
              USART_Rx_ptr_in--;
            }
        }
        else
        {
            USART_Rx_Buffer[USART_Rx_ptr_in++] = mychar;
            printf("%c", mychar); /* 回显字符 */
        }

         if (USART_Rx_ptr_in >= (USART_RX_DATA_SIZE - 1))
        {
          /**/
            USART_Rx_Buffer[USART_Rx_ptr_in] = '\0';
            USART_Rx_Done = 1;
        }
    }
}

/**
  * @brief  Clear USART Rx Buffer
            将串口接收Buffer清空，以便接收新的数据
  * @param  None
  * @retval None
  * @by     WWW.ARMJISHU.COM 
  */
void USART_Rx_Buffer_Clear(void)
{
    USART_Rx_ptr_in = 0;
    USART_Rx_Done = 0;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  printf("\n\r !!! Wrong parameter value detected\n");
  printf("\n\r - file %s", file);
  printf("\n\r - line %lu", line);

#if 0
  /* Infinite loop */
  while (1)
  {
  }
#endif
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 



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

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);

  if (COM == COM1)
  {
    /* Enable the USART2 Pins Software Remapping */
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }
  else
  {
    /* Enable the USART1 Pins */
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
#ifdef DEBUG_PRINTF	
void _USART1_IRQHandler(void)
{
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* received data */
    USART_GetInputString();
  }

  /* If overrun condition occurs, clear the ORE flag 
                              and recover communication */
  if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
  {
    (void)USART_ReceiveData(USART1);
  }

  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    USART_SendBufferData();
  }
}
#else
void _USART1_IRQHandler(void)
{
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* received data */
    //USART_GetInputString();
		_hwuart1_rxIntrSer();
  }

  /* If overrun condition occurs, clear the ORE flag 
                              and recover communication */
  if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
  {
    (void)USART_ReceiveData(USART1);
		_u1rxErrflag = TRUE;
  }

  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    //USART_SendBufferData();
		//if(_u1WrTxRegEn == TRUE)
		{	
			_hwuart1_txIntrSer();
		}
  }
}
#endif

static void Printf_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = EVAL_COM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_USART1_PreemptionPriority;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_USART1_SubPriority;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

static void Usart1RstVariable(void)
{
//  _u1TxEn = DISABLE;              /*disable tx*/
//  _u1RxEn = DISABLE;              /*disable rx*/
//  _u1Mode = SCOMM_MODE_NONE;       /*none mode*/

  _u1rxErrflag = 0;
  _u1txCnt = 0;
  _u1rxCnt = 0;
  _u1Txing = FALSE;
//	
  memset((u8 *)_u1RcvArea, 0x00, sizeof(_u1RcvArea));
  memset((u8 *)&_u1RcvBuf, 0x00, sizeof(_u1RcvBuf));
  _u1RcvBuf.pscommRcvContent = (u8 *)_u1RcvArea;
  _pu1RxPtr = &_u1RcvBuf;
  
  _u1RcvStates = URX_BUF_IDLE;
  _pu1TxPtr = NULL;
  _u1TxCharTmr = 0;
  _u1RxCharTmr = 0;
//  _u1FrameTmr = 0;
//  _u1WrTxRegEn = FALSE;
	USART_ITConfig(EVAL_COM2, USART_IT_TXE, DISABLE); //add yan141111
  _u1CharTmrRestart = UART0_CHAR_TMR_RESTART;     //default set
//  _u1FrameTmrRestart = UART0_FRM_TMR_RESTART;     //default set
}

static  void  _hwuart1_rxIntrSer(void)
{
  if(_u1RcvStates == URX_BUF_IDLE || _u1RcvStates == URX_BUF_BUSY) //and (_u1RxEn == ENABLE)) 
	{
	  if(_u1RcvStates == URX_BUF_IDLE)
	  {
	    _u1RcvStates = URX_BUF_BUSY;
	    _u1rxCnt = 0;
	  }	
    _pu1RxPtr->pscommRcvContent[_u1rxCnt] = USART_ReceiveData(USART1);
		_u1rxCnt++;
		_u1RxCharTmr = _u1CharTmrRestart;  //一直循坏扫描这个值，值为0时表示串口帧接收结束
	}	
	else
	{//clear rx fifo
		(void)USART_ReceiveData(USART1);
	}
}

static  void  _hwuart1_txIntrSer(void)
{
  if(_pu1TxPtr != NULL)
  {
    if(_u1txCnt < _pu1TxPtr->scommTxLen) 
    {
			/* Write one byte to the transmit data register */
			USART_SendData(USART1, _pu1TxPtr->pscommTxContent[_u1txCnt]);
			_u1txCnt++;
      _u1TxCharTmr = _u1CharTmrRestart;
    }
    else
    {
//      _u1WrTxRegEn = FALSE;   
			USART_ITConfig(EVAL_COM2, USART_IT_TXE, DISABLE); //add yan141111
      _u1Txing = FALSE;
      
//			_u1RxEn = ENABLE;      
//      if(_u1Mode == SCOMM_MODE_MASTER)
//      {/*provision to rx */   
//        _u1rxErrflag = FALSE;
//        _u1FrameTmr = _u1FrameTmrRestart;  /*主机,发送完成,则启动等待计时器等待接收完*/
//      }
    }
  }
	else
	{
		USART_ITConfig(EVAL_COM2, USART_IT_TXE, DISABLE); //add yan141111
	}
}

static  void  _hwuart1_1msTask(void)
{
  if(_u1TxCharTmr != 0)
  {//tx timer timeout
    _u1TxCharTmr--;
    if(_u1TxCharTmr == 0 && _u1Txing == TRUE)  //use delay to judge sent data(one frame) is over
    {
      _u1Txing     = FALSE;
//      _u1WrTxRegEn = FALSE;
			USART_ITConfig(EVAL_COM2, USART_IT_TXE, DISABLE); //add yan141111
      _u1txCnt     = 0;
      _pu1TxPtr    = NULL;
    }
  }
  
  if(_u1RxCharTmr != 0)
  {
    _u1RxCharTmr--;
    if(_u1RxCharTmr == 0)  //use delay to judge receive data(one frame) is over
    {
      if(_u1RcvStates == URX_BUF_BUSY)
      {
        _u1RcvStates = URX_BUF_COMPL;
      }  
    }
  }

//  if(_u1FrameTmr != 0)  //this flag use to control the time of switch master or slave mode, think about use or not in future!!!!
//  {
//    _u1FrameTmr--;
//    if(_u1FrameTmr == 0)
//    {
//      if(_u1Mode == SCOMM_MODE_MASTER)
//      {//master...
//        _u1RxEn = DISABLE;
//        _u1TxEn = ENABLE;
//      }
//      else if(_u1Mode == SCOMM_MODE_SLAVE) 
//      {//slave..
//        _u1RxEn = ENABLE;
//        _u1TxEn = DISABLE;  
//      }      
//    }
//  }  
}


static  void  _hwuart1_ScanRxProcess(void)
{
  if(_u1rxErrflag)
  {//err...clear all variable
    _u1RcvStates = URX_BUF_IDLE;    
    _u1RxCharTmr = TIMEOUT;
    _u1rxCnt = 0;   

//    _u1RxEn = ENABLE;
//    if(_u1Mode == SCOMM_MODE_MASTER)
//    {
//      _u1FrameTmr = _u1FrameTmrRestart;
//    }
//    else
//    {
//      _u1FrameTmr = 0;     /*restart frame timer*/
//    }     
    
    _u1rxErrflag = FALSE;   
  }  
  
  if(_u1RcvStates == URX_BUF_COMPL)
  {
    /*rcv data complete*/
    _pu1RxPtr->scommRcvLen = _u1rxCnt;
    _u1RcvStates = URX_BUF_READY;

//    _u1TxEn = ENABLE;
//    if(_u1Mode == SCOMM_MODE_MASTER)
//    {
//      _u1FrameTmr = 0;
//    }
//    else
//    {
//      _u1FrameTmr = _u1FrameTmrRestart;     /*restart frame timer*/
//    }
  }
}

scommReturn_t  t_hwuart1_ReceiveMessage(scommRcvBuf_t *rxMsg)
{
  volatile  scommReturn_t    __rcv = SCOMM_RET_NONE; 
  
  if(rxMsg != NULL)
  {
		if(_u1Txing == FALSE)
		{	
//	    if(_u1RxEn == TRUE)
//	    {
	      if(_u1RcvStates == URX_BUF_READY)
	      {
	        /*copy uart hardware data to upon layer*/
	        rxMsg->scommRcvLen = _pu1RxPtr->scommRcvLen;
	        memcpy((u8 *)(rxMsg->pscommRcvContent), (u8 *)(_pu1RxPtr->pscommRcvContent), _pu1RxPtr->scommRcvLen);

	        _pu1RxPtr->scommRcvLen = 0;   //reset value
	        _u1RcvStates = URX_BUF_IDLE; 
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
scommReturn_t  t_hwuart1_SendMessage(scommTxBuf_t*  txMsg)
{
//  if(_u1TxEn == ENABLE)
//  {
//    if(_u1Mode == SCOMM_MODE_MASTER)
//    {/*master.. provision to rx */
//    }
//    else
//    {/*slave,tx denote process complete*/
//      _u1FrameTmr = TIMEOUT;  
//    }

    while(u1_hwuart1_txing());
   
    if((txMsg != NULL) && (txMsg->scommTxLen > 0))
    {
      _u1TxCharTmr = _u1CharTmrRestart;
      _pu1TxPtr = txMsg;    /*get tx data pointer*/
      _u1txCnt = 0;//1;   
//      _u1WrTxRegEn = TRUE;    
			USART_ITConfig(EVAL_COM2, USART_IT_TXE, ENABLE); //add yan141111
			_u1Txing = TRUE;

//			/* Write first byte to the transmit data register */
//			USART_SendData(USART1, _pu1TxPtr->pscommTxContent[0]);//[_u1txCnt-1]);
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

u8  u1_hwuart1_txing(void)
{
	return ((USART_GetITStatus(USART1, USART_IT_TXE) == SET) ||(_u1Txing == TRUE));
}
