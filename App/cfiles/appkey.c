/**
  ******************************************************************************
  * @file    
  * @author  yanly
  * @version 
  * @date    
  * @brief   处理学习按键文件
  ******************************************************************************/
	
#include "oskey.h"
#include "osqtmr.h"
#include "osled.h"
#include "osrf315tx.h"
#include "appkey.h"
#include "osusart.h"   //debug141203


/*define********************************************************************************************************/	
#define  KEY_STATUS_IDLE																0
#define  KEY_STATUS_PRESSING														1	
#define	 KEY_STATUS_REAL_PRESS													2
/*variable********************************************************************************************************/	
static u8 learnKeyStatus;
u8 nodeAddrValue;

/*********************************************************************************************************
*	brief: 100ms检测学习键是否还按下，有则为真实按下
*********************************************************************************************************/
void AppSanKeyTask_10ms()
{
	static u8 shakeTimeCnt =0;
	if(learnKeyStatus == KEY_STATUS_PRESSING)
	{
		if(LEARN_KEY_PRESSED)
		{	
			shakeTimeCnt++;
			if(shakeTimeCnt == ANTI_SHAKE_TIME)
			{
				learnKeyStatus = KEY_STATUS_REAL_PRESS;
				shakeTimeCnt = 0;
			}	
		}
		else
		{
			learnKeyStatus = KEY_STATUS_IDLE;
			shakeTimeCnt = 0;
		}
	}
}
void AppKeyDialSwitchInit()
{
	OsKeyInit();
	OsDialSwitchInit();
	t_systmr_insertQuickTask(AppSanKeyTask_10ms, 10, OSTMR_PERIODIC);
	learnKeyStatus = KEY_STATUS_IDLE;
	nodeAddrValue = 0x0e;//OsGetNodeAddr(); 
}
void AppKeyProcess()
{
	u8 temp[8] = {1,2,3,4,5,6,7,8};
	if(learnKeyStatus == KEY_STATUS_REAL_PRESS)
	{
		u8 learningCode[3];
		learningCode[0] = OsGetSmartSocketAddr();
		learningCode[1] = OsGetSmartControlSwitchValue();
		learningCode[2] = nodeAddrValue;
		//if((learningCode[0]<4)&&(learningCode[1]>0))  //本节点支持最多4个RF插座，控制值00为无效  //没有拨码开关，暂时屏蔽
		{
			Rf315SendMsg((u8 *)&learningCode[0]);//发送学习码
			//learnLedBlinkMode.int8u = LEARN_LED_BLINK;//学习灯闪烁放在Rf315SendMsg函数里
			
			//操作完成之后要置按键状态空闲
			learnKeyStatus = KEY_STATUS_IDLE;
			//--------------------------------------------------------------------------------//
			t_osscomm_sendMessage(temp, sizeof(temp), USART2_COM); //
		}	
	}
  else if(learnKeyStatus == KEY_STATUS_IDLE)
	{
		if(LEARN_KEY_PRESSED)
		{
			learnKeyStatus = KEY_STATUS_PRESSING;
		}
	}
	else
	{//中断里操作
	}
}
