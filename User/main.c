#include "stdio.h"
#include "string.h"
#include "ostmr.h"
#include "osqtmr.h"
#include "osled.h"
#include "osiwdg.h"
#include "appsensor.h"
#include "appkey.h"
#include "apprf315tx.h"
#include "appusart.h"

//#define USE_WATCHDOG

static void AppInit(void);

int main()
{
	AppInit();
	#ifdef DEBUG_PRINTF
  printf("start program \r\n");
	#endif
	while(1)
	{
		AppSensorProcess();
		AppKeyProcess();	
		AppUsartProcess();
		#ifdef 	USE_WATCHDOG
		IwdgFeed();
		#endif
	}
}

static void AppInit()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断优先级分组
	b_ostmr_init();   //多定时器任务10ms
	b_systmr_init();  //多定时器任务1ms
	OsRf315Init();      
	//delay_init(72);	 
	OsLedInit(); 
	AppUsartInit();
	AppSensorInit();
	AppKeyDialSwitchInit();
	#ifdef USE_WATCHDOG
	IwdgInit();
	#endif
	
//	lenth = strlen((char *)sendData);
//	t_osscomm_sendMessage(sendData,lenth , USART1_COM);
//	t_ostmr_insertTask(task500ms,500,OSTMR_PERIODIC);
}	
//int secEnable =0;
//int counter =0;
//int keyNum;

//void  apprdr_delay(u32 dely)  function not correct
//{
//  volatile  u32  __i;
//  
//  for(; dely > 0; dely--)
//  {
//    for(__i = 0; __i < 7000; __i++);//10000
//  }
//}
//static void Delay_ARMJISHU(__IO uint32_t nCount)
//{
//  for (; nCount != 0; nCount--);
////}
//void app1s_task()
//{
//	secEnable = ~secEnable;
//}
//void app1ms_task()
//{
//	//secEnable ++;
//}


//u16 ADCConvertedValueLocal, Precent = 0, Voltage = 0;
//u8 sendData[200] = "this is test";
//u8 receiveData[200];
//u16 recvLength;
//u16 lenth;

//void task500ms()
//{
//	secEnable = ~secEnable;
//}	

