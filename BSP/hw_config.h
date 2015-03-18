/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief  
  ******************************************************************************
  * @copy
  */ 
	
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "stm32f10x.h"

//led 
#define RCC_GPIO_LED                                RCC_APB2Periph_GPIOC //|RCC_APB2Periph_GPIOD
#define LED_STATUS_PORT															GPIOC                              
#define LED_LEARN_PORT															GPIOC    //GPIOD  //modify 150318
#define GPIO_LED_STATUS                             GPIO_Pin_7    
#define GPIO_LED_LEARN                              GPIO_Pin_7
//#define GPIO_LED_ALL                                GPIO_LED1 |GPIO_LED2 //|GPIO_LED3 |GPIO_LED4 

//key
#define RCC_KEY                                     RCC_APB2Periph_GPIOC
#define GPIO_KEY_PORT                               GPIOC
//#define GPIO_KEY1                                   GPIO_Pin_1
//#define GPIO_KEY2                                   GPIO_Pin_2
//#define GPIO_KEY3                                   GPIO_Pin_3 
//#define GPIO_KEY4                                   GPIO_Pin_4 
//#define	GPIO_KEY_ALL																GPIO_KEY1|GPIO_KEY2|GPIO_KEY3|GPIO_KEY4
#define	GPIO_LEARN_KEY															GPIO_Pin_1

//dial switch
#define RCC_DIALSWITCH                              RCC_APB2Periph_GPIOD
#define GPIO_DIALSWITCH_PORT                        GPIOD
#define	GPIO_SMART_SOCKET_ADDR1											GPIO_Pin_3   //数字高的为高位
#define	GPIO_SMART_SOCKET_ADDR2											GPIO_Pin_4
#define	GPIO_SMART_SOCKET_ADDR3											GPIO_Pin_10
#define	GPIO_SMART_SOCKET_CONTROL1									GPIO_Pin_11
#define	GPIO_SMART_SOCKET_CONTROL2									GPIO_Pin_12
#define	GPIO_NODE_ADDR1														  GPIO_Pin_13
#define	GPIO_NODE_ADDR2														  GPIO_Pin_14
#define	GPIO_NODE_ADDR3															GPIO_Pin_15
#define	GPIO_NODE_ADDR4														  GPIO_Pin_5//GPIO_Pin_1
#define	GPIO_NODE_ADDR5															GPIO_Pin_4//GPIO_Pin_2

//rf315
#define RCC_RF315_TX                                RCC_APB2Periph_GPIOA
#define GPIO_RF315_TX_PORT                          GPIOA    
#define GPIO_RF315_TX                               GPIO_Pin_6
#define GPIO_RF315_TX_STOP_MODE											GPIO_Mode_IN_FLOATING
#define GPIO_RF315_TX_SEND_MODE											GPIO_Mode_Out_PP
#define GPIO_RF315_TX_SPEED										      GPIO_Speed_50MHz

//adc
#define RCC_GPIO_ADC                                RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB
//#define GPIO_ADC_ZONE5_6_PORT                       GPIOC
//#define GPIO_ADC_ZONE7_8_PORT                       GPIOB
#define GPIO_SENSOR123_PORT                         GPIOC
#define GPIO_SENSOR45_PORT                          GPIOB
//#define	ADC_ZONE5_PIN																GPIO_Pin_5
//#define	ADC_ZONE6_PIN																GPIO_Pin_0
//#define	ADC_ZONE7_PIN																GPIO_Pin_0
//#define	ADC_ZONE8_PIN																GPIO_Pin_1
//#define	ADC_ZONE5_CHANNEL														ADC_Channel_15
//#define	ADC_ZONE6_CHANNEL														ADC_Channel_10
//#define	ADC_ZONE7_CHANNEL														ADC_Channel_8
//#define	ADC_ZONE8_CHANNEL														ADC_Channel_9
#define	ADC_SENSOR1_PIN														  GPIO_Pin_4
#define	ADC_SENSOR2_PIN														  GPIO_Pin_5
#define	ADC_SENSOR3_PIN														  GPIO_Pin_0
#define	ADC_SENSOR4_PIN															GPIO_Pin_0
#define	ADC_SENSOR5_PIN															GPIO_Pin_1
#define	ADC_SENSOR1_CHANNEL													ADC_Channel_14
#define	ADC_SENSOR2_CHANNEL													ADC_Channel_15
#define	ADC_SENSOR3_CHANNEL													ADC_Channel_10
#define	ADC_SENSOR4_CHANNEL													ADC_Channel_8
#define	ADC_SENSOR5_CHANNEL													ADC_Channel_9
#define	ADC_ZONE_SAMPLE_TIME												ADC_SampleTime_71Cycles5

//timer3
#define	RCC_TIMER_1ST																RCC_APB1Periph_TIM3
#define	TIMER_CHANNEL_1ST														TIM3
#define	TIMER_IRQ_CHANNEL_1ST												TIM3_IRQn
#define	NVIC_TIMER_PreemptionPriority_1ST						0
#define	NVIC_TIMER_SubPriority_1ST									1
//timer2
#define	RCC_TIMER_2ND																RCC_APB1Periph_TIM2
#define	TIMER_CHANNEL_2ND														TIM2
#define	TIMER_IRQ_CHANNEL_2ND												TIM2_IRQn
#define	NVIC_TIMER_PreemptionPriority_2ND						0     //中断优先级最高
#define	NVIC_TIMER_SubPriority_2ND									0

//timer4
#define	RCC_TIMER_3ND																RCC_APB1Periph_TIM4
#define	TIMER_CHANNEL_3ND														TIM4
#define	TIMER_IRQ_CHANNEL_3ND												TIM4_IRQn
#define	NVIC_TIMER_PreemptionPriority_3ND						0    
#define	NVIC_TIMER_SubPriority_3ND									2

//#define ADC_CH8   8   	// PB0 channel 8
//#define ADC_CH9   9   	// PB1 channel 9
//#define ADC_CH10  10  	// PC0 channel 10
//#define ADC_CH11  11  	// PC1 channel 11
//#define ADC_CH12  12  	// PC2 channel 12
//#define ADC_CH13  13  	// PC3 channel 13					  
//#define ADC_CH14  14  	// PC4 channel 14
//#define ADC_CH15  15  	// PC5 channel 15		 


//usart
#define	NVIC_USART2_PreemptionPriority					1
#define	NVIC_USART2_SubPriority									0

#define	NVIC_USART1_PreemptionPriority					1
#define	NVIC_USART1_SubPriority									1

#define COMn                             2
/**
 * @brief Definition for COM port1, connected to USART2 (USART2 pins remapped on GPIOD)
 */ 
#define EVAL_COM1_STR                    "USART2"
#define EVAL_COM1                        USART2
#define EVAL_COM1_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM1_TX_PIN                 GPIO_Pin_2
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_RX_PIN                 GPIO_Pin_3
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_USART_IRQn             USART2_IRQn

   
/**
 * @brief Definition for COM port2, connected to USART1
 */ 
#define EVAL_COM2_STR                    "USART1"
#define EVAL_COM2                        USART1
#define EVAL_COM2_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM2_TX_PIN                 GPIO_Pin_9
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_RX_PIN                 GPIO_Pin_10
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_IRQn                   USART1_IRQn




#endif /* __HW_CONFIG_H */

