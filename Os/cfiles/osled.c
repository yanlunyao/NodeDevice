/**
  ******************************************************************************
  * @file    
  * @author  yanly
  * @version 
  * @date    
  * @brief   
  ******************************************************************************/
	
#include "osled.h"
#include "ostmr.h"


#define LIGHTON					Bit_RESET
#define LIGTHOFF				Bit_SET	
#define	LED_STATUS_OPEN	(GPIO_ResetBits(LED_STATUS_PORT, GPIO_LED_STATUS))
#define	LED_STATUS_CLOSE (GPIO_SetBits(LED_STATUS_PORT, GPIO_LED_STATUS))

static u8 _sysLed250msCounter;
union_byte learnLedBlinkMode;

static void OsLedBlinkTask_250ms(void);
static void LearnLedLight(BitAction lightStatus);

void OsLedInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_GPIO_LED , ENABLE);  //RCC_APB2Periph_AFIO
  
  /* LED1 pins configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_LED_STATUS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LED_STATUS_PORT, &GPIO_InitStructure);
	
	/* LED2 pins configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_LED_LEARN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LED_LEARN_PORT, &GPIO_InitStructure);
	
	learnLedBlinkMode.int8u = LEARN_LED_ALWAYS_OFF;
	t_ostmr_insertTask(OsLedBlinkTask_250ms, 250, OSTMR_PERIODIC);
	
	LED_STATUS_CLOSE;
	LearnLedLight(LIGTHOFF);//关learn led
}
static void LearnLedLight(BitAction lightStatus)
{
	GPIO_WriteBit(LED_LEARN_PORT, GPIO_LED_LEARN, lightStatus);
}	
/********************************************************************************
@brief ：250ms定时器扫描一次，判断bit值
******************************************************************************/
void OsLedBlinkTask_250ms()
{
	static u8 statusLedCnt =0;
	static u8 statusLedInvertEd = 0;
	if(learnLedBlinkMode.int8u != LEARN_LED_ALWAYS_OFF)
	{	
		 switch(_sysLed250msCounter++)
		{
				case 0:
				if(learnLedBlinkMode.ByteBit8.b0)
				{
					LearnLedLight(LIGHTON);
				}
				else
				{
					LearnLedLight(LIGTHOFF);
				}
			break;
			case 1:
				if(learnLedBlinkMode.ByteBit8.b1)
				{
					LearnLedLight(LIGHTON);
				}
				else
				{
					LearnLedLight(LIGTHOFF);
				}
			break;
			case 2:
				if(learnLedBlinkMode.ByteBit8.b2)
				{
					LearnLedLight(LIGHTON);
				}
				else
				{
					LearnLedLight(LIGTHOFF);
				}
			break;
			case 3:
				if(learnLedBlinkMode.ByteBit8.b3)
				{
					LearnLedLight(LIGHTON);
				}
				else
				{
					LearnLedLight(LIGTHOFF);
				}
			break;
			case 4:
				if(learnLedBlinkMode.ByteBit8.b4)
				{
					LearnLedLight(LIGHTON);
				}
				else
				{
					LearnLedLight(LIGTHOFF);
				}
			break;
			case 5:
				if(learnLedBlinkMode.ByteBit8.b5)
				{
					LearnLedLight(LIGHTON);
				}
				else
				{
					LearnLedLight(LIGTHOFF);
				}
			break;
			case 6:
				if(learnLedBlinkMode.ByteBit8.b6)
				{
					LearnLedLight(LIGHTON);
				}
				else
				{
					LearnLedLight(LIGTHOFF);
				}
			break;
			case 7:
				if(learnLedBlinkMode.ByteBit8.b7)
				{
					LearnLedLight(LIGHTON);
				}
				else
				{
					LearnLedLight(LIGTHOFF);
				}
				_sysLed250msCounter = 0;
				learnLedBlinkMode.int8u = LEARN_LED_ALWAYS_OFF;
			break;
			default:
				learnLedBlinkMode.int8u = LEARN_LED_ALWAYS_OFF;
				_sysLed250msCounter = 0;
			break;
		}
	}
	else
	{
		LearnLedLight(LIGTHOFF);
	}	
	statusLedCnt++;
	if(statusLedCnt  ==4)
	{
		statusLedCnt =0;
		statusLedInvertEd = ~statusLedInvertEd;
		if(statusLedInvertEd)
		{
			LED_STATUS_OPEN;
		}
		else
		{
			LED_STATUS_CLOSE;
		}	
	}	
	
}	




