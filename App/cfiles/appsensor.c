#include <stdio.h>
#include <string.h>
#include "hw_config.h"
#include "osadc.h"
#include "ostmr.h"
#include "osprintf.h"
#include "appsensor.h"
#include "appusart.h"


/*****************
define 
*****************/
#define	 	SENSOR_NORMAL_MIN_VALUE							1380
#define		SENSOR_NORMAL_MAX_VALUE							1480
#define		SENSOR_ALARM_OPEN_THRESHOLD_VALUE		4000
#define		SENSOR_SHORT_THRESHOLD_VALUE				20
#define		SENSOR_PARALLEL_THRESHOLD_VALUE

#define		SENSOR_NUM													5
#define   REUPLOAD_SENSOR_STATUS_CONTER				20  //2s

/*
********************************************************************************
  variable declaration
********************************************************************************
*/
// sensor channel
static u8 s_channel = 0;

// the buf of sensor state of old and new
static u8 s_old_status_buf[5] = {0, 0, 0, 0, 0};
static u8 s_new_status_buf[5] = {0, 0, 0, 0, 0};

u8  g_sensor_timing_flg = 0;		// sensor timing flag
u16 g_sensor_cnt = 0;				// sensor count

//upload sensor event
typedef struct
{
	bool_t 		needUpRespondConfirm;
	bool_t 		uploadFlag;
	u8     		Counter;
	u8     		SensorMsg;	
}uploadSensorEvent_t;	
static uploadSensorEvent_t  uploadSensorStsEvent[SENSOR_NUM];
//bool_t UpRespondConfirmed[SENSOR_NUM];
/////////////////////////////////////////////////////////////////////


static void sensor_alarm_check(u8 ch, u8 *status);
static void App100msSensorTask(void);
static void sensor_scan(void);
static void ResetUploadEventValue(void);
static void AppSensorUploadHandle(void);
static void UploadSensorMsg(u8 channel);


static void ResetUploadEventValue(void)
{
	memset(uploadSensorStsEvent, 0, sizeof(uploadSensorEvent_t));
//	memset(UpRespondConfirmed, 1, sizeof(UpRespondConfirmed));
}
static void App100msSensorTask(void)
{
	u8 i;
	//传感器状态变化事件计数
	if(g_sensor_timing_flg)
	{
		g_sensor_cnt++;
		if(g_sensor_cnt >= 65534)
		{
			g_sensor_cnt = 65534;
		}
	}
	else
	{
		g_sensor_cnt = 0;	
	}
	//上传事件计数
	for(i=0; i<SENSOR_NUM; i++)  
	{
		if(uploadSensorStsEvent[i].Counter)
		{
			uploadSensorStsEvent[i].Counter --;
			if(uploadSensorStsEvent[i].Counter==0)
			{
				if(uploadSensorStsEvent[i].needUpRespondConfirm)
					uploadSensorStsEvent[i].uploadFlag = TRUE;
			}	
		}	
	}
}
//int hhhh = 0;
//static void App400msSensorTask(void)
//{
//	hhhh = ~hhhh;
//}
void AppSensorInit(void)
{
	OsAdcInit();
	t_ostmr_insertTask(App100msSensorTask, 100, OSTMR_PERIODIC);  // 100 ms task
//	t_ostmr_insertTask(App400msSensorTask, 400, OSTMR_PERIODIC);  // 100 ms task
	ResetUploadEventValue();
}

static void sensor_alarm_check(u8 ch, u8 *status)
{
	u16 adcx;
	u8 status_tmp;

	adcx = 0;
	status_tmp = 0;
	adcx = get_adc_average(ch, 100); //采样次数平均值
		
//	#ifdef DEBUG_PRINTF
//	if((ch == ADC_SENSOR1_CHANNEL)&&(hhhh))
//	{
//	  hhhh = 0;
//		printf("ch%d adc is %d\r\n",ch,adcx);
//	}
//	#endif
	if(adcx != 65535)
	{
		if(((adcx > SENSOR_NORMAL_MIN_VALUE) && (adcx < SENSOR_NORMAL_MAX_VALUE)) || (adcx < SENSOR_SHORT_THRESHOLD_VALUE))	// normal
		{
			s_channel++;		// scan next sensor
			*status = SENSOR_NORMAL_STATE;
			g_sensor_timing_flg = 0;
			g_sensor_cnt = 0;
		}
		else
		{
			if(adcx > SENSOR_ALARM_OPEN_THRESHOLD_VALUE)					// alarm/open
			{
				status_tmp = SENSOR_ALARM_STATE;	
			}
//			else if((adcx < SENSOR_SHORT_THRESHOLD_VALUE))			// short  门磁合上adc值=短路值，所以等同于normal
//			{
//				status_tmp = SENSOR_SHORT_STATE;	
//			}
			else							// parallel
			{
				status_tmp = SENSOR_PARALLEL_STATE;
			}
			
			if(status_tmp == *status)
			{
				s_channel++;		
				g_sensor_timing_flg = 0;
				g_sensor_cnt = 0;	
			}
			else
			{
				g_sensor_timing_flg = 1;		
				if(g_sensor_cnt > 10)			
				{
					g_sensor_timing_flg = 0;	
					g_sensor_cnt = 0;
					*status = status_tmp;
					s_channel++;	
				}
			}
		}
	}
}
static void sensor_scan(void)
{
	const u8 __numToChannel[] = {ADC_SENSOR1_CHANNEL, ADC_SENSOR2_CHANNEL, ADC_SENSOR3_CHANNEL, ADC_SENSOR4_CHANNEL,ADC_SENSOR5_CHANNEL};																						
	if(s_channel >= SENSOR_NUM)
	{
		s_channel = 0;
	}
	sensor_alarm_check(__numToChannel[s_channel], &s_new_status_buf[s_channel]); 
}
//void AppSensorProcess(void)
//{
//	u8 i, j;
//	u8 tempData[30];
//	u16 tempLen;
//	sensor_scan();		// scan all the sensors
//	if(s_channel == SENSOR_NUM)
//	{
//		for(i = 0; i < 5; i++)
//		{
//			if(s_new_status_buf[i] != s_old_status_buf[i])
//			{
//				if(s_new_status_buf[i] ==  SENSOR_ALARM_STATE)
//				{	
////					for(j=0;j<5;j++)
////					{
////						//send_315M_msg(i-3,2);
////					}
////					#ifdef DEBUG_PRINTF
////					printf("sensor %d be alarm \r\n",i+1);
////					#endif
//					
//					//if(UpRespondConfirmed[i])  //如果确认标记为false，说明上次的确认还没收到，这个时候不发送数据?
//					{	

//						#ifdef 	DEBUG_PRINTF
//						printf("sensor %d be alarm \r\n",i+1);
//					  #endif
//						//...
//						UpRespondConfirmed[i] = FALSE;//置确认标记为false  
//						uploadSensorStsEvent[i].SensorMsg = SENSOR_ALARM_STATE;//记录此消息		
//						uploadSensorStsEvent[i].Counter = REUPLOAD_SENSOR_STATUS_CONTER; //开启2s定时
//						uploadSensorStsEvent[i].uploadFlag = FALSE;//置上传标记为false	
//						//上传消息
//						tempData[0] = 0x7e;
//						tempData[1] = 1;
//						tempData[2] = 2;
//						tempData[3] = i;
//						tempData[4] = uploadSensorStsEvent[i].SensorMsg;
//						tempLen = 5;
//						AppRs485CommSendCmd(tempLen, tempData);
//						
//					}
//				}
//				else
//				{
////					for(j=0;j<5;j++)
////					{
////						//send_315M_msg(i-3,3);	//ok
////					}
////					#ifdef DEBUG_PRINTF
////					printf("sensor %d be nomal \r\n",i+1);
////					#endif
//					
//					//if(UpRespondConfirmed[i])  //如果确认标记为false，说明上次的确认还没收到，这个时候不发送数据?
//					{	
//					//...
//						UpRespondConfirmed[i] = FALSE;//置确认标记为false  
//						uploadSensorStsEvent[i].SensorMsg = SENSOR_NORMAL_STATE;//记录此消息		
//						uploadSensorStsEvent[i].Counter = REUPLOAD_SENSOR_STATUS_CONTER; //开启2s定时
//						uploadSensorStsEvent[i].uploadFlag = FALSE;//置上传标记为false	
//						
//												//上传消息
//						tempData[0] = 0x7e;
//						tempData[1] = 1;
//						tempData[2] = 2;
//						tempData[3] = i;
//						tempData[4] = uploadSensorStsEvent[i].SensorMsg;
//						tempLen = 5;
//						AppRs485CommSendCmd(tempLen, tempData);
//					}
//				}
//				s_old_status_buf[i] = s_new_status_buf[i];	
//			}
//		}
//	}
//	AppSensorUploadHandle();
//}
//static void AppSensorUploadHandle()
//{
//	u8 i;
//	u8 tempData[30]; 
//	u16 tempLen;
//	for(i=0; i<SENSOR_NUM; i++)
//	{
//		if(UpRespondConfirmed[i] != TRUE) //respond confirmed 在串口接收里置TRUE
//		{
//			if(uploadSensorStsEvent[i].uploadFlag)
//			{	
//				//上传对应sensor的消息  
//				//上传消息
//				tempData[0] = 0x7e;
//				tempData[1] = 1;
//				tempData[2] = 2;
//				tempData[3] = i;
//				tempData[4] = uploadSensorStsEvent[i].SensorMsg;
//				tempLen = 5;
//				AppRs485CommSendCmd(tempLen, tempData);				
//				#ifdef DEBUG_PRINTF
//				printf("sensor %d be alarm \r\n",i+1);
//				#endif
//				//置上传标记为FALSE,开启定时
//				uploadSensorStsEvent[i].uploadFlag = FALSE;
//				uploadSensorStsEvent[i].Counter = REUPLOAD_SENSOR_STATUS_CONTER;		
//			}
//		}
//		else
//		{
//			//上传回复已经确认了，不需要复位上传标记和计数器。
//		}	
//	}
//}	
static void UploadSensorMsg(u8 channel)
{
	u8 tempData[30];
	u16 tempLen;
	tempData[0] = 0x7e;
	tempData[1] = 1;
	tempData[2] = 2;
	tempData[3] = channel;
	tempData[4] = uploadSensorStsEvent[channel].SensorMsg;
	tempLen = 5;
	AppRs485CommSendCmd(tempLen, tempData);
}	
void AppSensorProcess(void)
{
	u8 i;
	sensor_scan();
	if(s_channel <= SENSOR_NUM)
	{
		i = s_channel -1;
		if(s_new_status_buf[i] != s_old_status_buf[i])
		{
			if(s_new_status_buf[i] ==  SENSOR_ALARM_STATE)
			{	
				#ifdef 	DEBUG_PRINTF
				printf("sensor %d be alarm \r\n",i+1);
				#endif
				uploadSensorStsEvent[i].SensorMsg = SENSOR_ALARM_STATE;//记录此消息		
			}
			else
			{
				uploadSensorStsEvent[i].SensorMsg = SENSOR_NORMAL_STATE;//记录此消息		
			}
			uploadSensorStsEvent[i].needUpRespondConfirm = TRUE;//置需要确认
			uploadSensorStsEvent[i].Counter = REUPLOAD_SENSOR_STATUS_CONTER; //开启2s定时
		  uploadSensorStsEvent[i].uploadFlag = FALSE;//置上传标记为false	
			UploadSensorMsg(i);//上传消息
			s_old_status_buf[i] = s_new_status_buf[i];	
		}
	}
	AppSensorUploadHandle();
}
static void AppSensorUploadHandle()
{
	u8 i;
	i = s_channel -1;
	if(uploadSensorStsEvent[i].needUpRespondConfirm) //respond confirmed 在串口接收里置FASLE
	{
		if(uploadSensorStsEvent[i].uploadFlag)
		{	
			//上传对应sensor的消息  
			UploadSensorMsg(i);			
			#ifdef DEBUG_PRINTF
			printf("sensor %d be alarm \r\n",i+1);
			#endif
			//置上传标记为FALSE,开启定时
			uploadSensorStsEvent[i].uploadFlag = FALSE;
			uploadSensorStsEvent[i].Counter = REUPLOAD_SENSOR_STATUS_CONTER;		
		}
	}
	else
	{
		//上传回复已经确认了，不需要复位上传标记和计数器。
	}	
}	

u8 *GetCurrentSensorStatus(u8 *status)
{
  memcpy(status, s_old_status_buf, sizeof(s_old_status_buf));
	return status;
}
void UploadRspondConfirmed(u8 sensorNum)
{
	uploadSensorStsEvent[sensorNum].needUpRespondConfirm = FALSE;
}	
//static void sensor_alarm_check(u8 ch, u8 *status)
//{
//	u16 adcx;
//	u8 status_tmp;

//	adcx = 0;
//	status_tmp = 0;
//	adcx = get_adc_average(ch, 100); //采样次数平均值

////	#ifdef DEBUG_PRINTF
////	if((ch == ADC_SENSOR1_CHANNEL)&&(hhhh))
////	{
////	  hhhh = 0;
////		printf("ch%d adc is %d\r\n",ch,adcx);
////	}
////	#endif
//		
//	if(adcx != 65535)
//	{
//		if((adcx > SENSOR_NORMAL_MIN_VALUE) && (adcx < SENSOR_NORMAL_MAX_VALUE))	// normal
//		{
//			s_channel++;		// scan next sensor
//			*status = SENSOR_NORMAL_STATE;
//			g_sensor_timing_flg = 0;
//			g_sensor_cnt = 0;
//		}
//		else
//		{
//			if(adcx > SENSOR_ALARM_OPEN_THRESHOLD_VALUE)					// alarm/open
//			{
//				status_tmp = SENSOR_ALARM_STATE;	
//			}
//			else if((adcx < SENSOR_SHORT_THRESHOLD_VALUE))			// short  门磁合上adc值=短路值，所以等同于normal
//			{
//				status_tmp = SENSOR_SHORT_STATE;	
//			}
//			else							// parallel
//			{
//				status_tmp = SENSOR_PARALLEL_STATE;
//			}
//			
//			if(status_tmp == *status)
//			{
//				s_channel++;		// scan next sensor
//				g_sensor_timing_flg = 0;
//				g_sensor_cnt = 0;	
//			}
//			else
//			{
//				g_sensor_timing_flg = 1;		// timing start
//				if(g_sensor_cnt > 10)			// timing > 1000ms
//				{
//					g_sensor_timing_flg = 0;	// timing stop
//					g_sensor_cnt = 0;
//					*status = status_tmp;
//					s_channel++;	// scan next sensor
//				}
//			}
//		}
//	}
//}
