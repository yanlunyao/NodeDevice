/*
  ******************************************************************************
  * @file    osadc.c
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  ******************************************************************************
  */
#include <stdlib.h>
#include "osadc.h"
#include "osqtmr.h"
//#include "osdelay.h"
/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void ADC_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
                        
  GPIO_InitStructure.GPIO_Pin = ADC_SENSOR1_PIN |ADC_SENSOR2_PIN |ADC_SENSOR3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_SENSOR123_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_SENSOR4_PIN |ADC_SENSOR5_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_SENSOR45_PORT, &GPIO_InitStructure);
}

void OsAdcInit()
{
	ADC_InitTypeDef ADC_InitStructure;      //ADC初始化结构体声明
	//
	ADC_GPIO_Configuration();
	
	  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_GPIO_ADC, ENABLE);
  
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  //独立的转换模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;		  
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ADC外部开关，SWSTART
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //对齐方式,ADC为12位中，右对齐方式
  ADC_InitStructure.ADC_NbrOfChannel = 1;	 //开启通道数，1个
  ADC_Init(ADC1, &ADC_InitStructure);

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);//convention on external event enable
	
  /* ADC1 regular  ADC通道组， 通道 采样顺序1，转换时间 */ 
  ADC_RegularChannelConfig(ADC1, ADC_SENSOR1_CHANNEL, 1, ADC_ZONE_SAMPLE_TIME);
	ADC_RegularChannelConfig(ADC1, ADC_SENSOR2_CHANNEL, 1, ADC_ZONE_SAMPLE_TIME);
	ADC_RegularChannelConfig(ADC1, ADC_SENSOR3_CHANNEL, 1, ADC_ZONE_SAMPLE_TIME);
  ADC_RegularChannelConfig(ADC1, ADC_SENSOR4_CHANNEL, 1, ADC_ZONE_SAMPLE_TIME);
	ADC_RegularChannelConfig(ADC1, ADC_SENSOR5_CHANNEL, 1, ADC_ZONE_SAMPLE_TIME);

//  /* Enable ADC1 DMA */
//  ADC_DMACmd(ADC1, ENABLE);	  //ADC命令，使能
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);  //开启ADC1
  
  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);	  //重新校准
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));  //等待重新校准完成
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);		//开始校准
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));	   //等待校准完成
  /* Start ADC1 Software Conversion */ 
//  ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//连续转换开始，ADC通过DMA方式不断的更新RAM区。
}

/*
********************************************************************************
  Function:			get_adc
  Description:		get adc value once
  Input:			ch:channel 0~16
  Output:			
  Return:			
  Others:			
********************************************************************************
*/
u16 get_adc(u8 ch)   
{
	// set the conversion sequence	  		 
	ADC1->SQR3 &= 0XFFFFFFE0;		// rules for ch sequence 1 channel
	ADC1->SQR3 |= ch;		  			    
	ADC1->CR2 |= 1 << 22;       	// start the rule conversion channel
	while(!(ADC1->SR & 1 << 1));	// waiting for the end of conversion   

	return ADC1->DR;				// return the adc value	


}

/*
********************************************************************************
  Function:			get_adc_average
  Description:		Access to multiple sampling average
  Input:			ch:channel 0~16,  times:number of times
  Output:			
  Return:			average
  Others:			
********************************************************************************
*/
u16 get_adc_average(u8 ch, u8 times)
{
	u32 tmp_val1, tmp_val2;
	u8 t;

	tmp_val1 = 0;
	tmp_val2 = 0;
	
	tmp_val1 += get_adc(ch);
	//delay_ms(1);
	systmr_quickWait(1);

	for(t = 0; t < times; t++)
	{
		tmp_val2 += get_adc(ch);
	}

	tmp_val2 = tmp_val2 / times;
	if(abs(tmp_val1 - tmp_val2) < 6) //误差从4调到6,add yan141120
	{
		return (tmp_val1);	
	}
	else
	{
		return 65535;
	}
}

//void GetADCConvertedValueInPtr(u8 ch, u32 *ptr)
//{
//	u8 t;
//	//*ptr = get_adc(ch);
//	for(t = 0; t < 100; t++)
//	{
//		*ptr = get_adc(ch);
//		ptr ++;
//	}
//}

