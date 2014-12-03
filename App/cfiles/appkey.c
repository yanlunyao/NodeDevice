/**
  ******************************************************************************
  * @file    
  * @author  yanly
  * @version 
  * @date    
  * @brief   ����ѧϰ�����ļ�
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
*	brief: 100ms���ѧϰ���Ƿ񻹰��£�����Ϊ��ʵ����
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
		//if((learningCode[0]<4)&&(learningCode[1]>0))  //���ڵ�֧�����4��RF����������ֵ00Ϊ��Ч  //û�в��뿪�أ���ʱ����
		{
			Rf315SendMsg((u8 *)&learningCode[0]);//����ѧϰ��
			//learnLedBlinkMode.int8u = LEARN_LED_BLINK;//ѧϰ����˸����Rf315SendMsg������
			
			//�������֮��Ҫ�ð���״̬����
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
	{//�ж������
	}
}
