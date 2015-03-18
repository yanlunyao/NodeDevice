/**
  ******************************************************************************
  * @file    
  * @author  yanly
  * @version 
  * @date    
  * @brief   按键和拨码开关 os层放一起
  ******************************************************************************/
	
#include "oskey.h"

/* Values magic to the Board keys */
//#define  NOKEY  0
//#define  KEY1   1
//#define  KEY2   2
//#define  KEY3   3
//#define  KEY4   4
//#define  KEY5   5

void OsKeyInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_KEY, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_LEARN_KEY;
  GPIO_Init(GPIO_KEY_PORT, &GPIO_InitStructure);

}
void OsDialSwitchInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_DIALSWITCH, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_SMART_SOCKET_ADDR1|GPIO_SMART_SOCKET_ADDR2|GPIO_SMART_SOCKET_ADDR3| \
																GPIO_SMART_SOCKET_CONTROL1|GPIO_SMART_SOCKET_CONTROL2| \
																GPIO_NODE_ADDR1|GPIO_NODE_ADDR2|GPIO_NODE_ADDR3|GPIO_NODE_ADDR4|GPIO_NODE_ADDR5;
  GPIO_Init(GPIO_DIALSWITCH_PORT, &GPIO_InitStructure);
}
/*********************************************************************************************************
*	brief: 获取拨码开关值： 智能插座地址，智能插座控制值，节点地址
*********************************************************************************************************/
u8 OsGetSmartSocketAddr()  //123
{
	u8 __temp,__temp1,__temp2,__temp3;
	__temp=__temp1=__temp2=__temp3=0;
	__temp1 = (GPIO_ReadInputDataBit(GPIO_DIALSWITCH_PORT, GPIO_SMART_SOCKET_ADDR3))<<2;
	__temp2 = (GPIO_ReadInputDataBit(GPIO_DIALSWITCH_PORT, GPIO_SMART_SOCKET_ADDR2)) <<1;	
	__temp3 = GPIO_ReadInputDataBit(GPIO_DIALSWITCH_PORT, GPIO_SMART_SOCKET_ADDR1);
  __temp = __temp1|__temp2|__temp3 ;
  return __temp;	
}
u8 OsGetSmartControlSwitchValue() //45
{
	u8 __temp,__temp1,__temp2;
	__temp=__temp1=__temp2=0;
	__temp1 = (GPIO_ReadInputDataBit(GPIO_DIALSWITCH_PORT, GPIO_SMART_SOCKET_CONTROL2)) <<1;	
	__temp2 = GPIO_ReadInputDataBit(GPIO_DIALSWITCH_PORT, GPIO_SMART_SOCKET_CONTROL1);
  __temp = __temp1|__temp2 ;
  return __temp;	
}
u8 OsGetNodeAddr()   //678
{
	u8 __temp,__temp1,__temp2,__temp3,__temp4,__temp5;
	__temp=__temp1=__temp2=__temp3=__temp4=__temp5=0;
	__temp1 = (GPIO_ReadInputDataBit(GPIOA, GPIO_NODE_ADDR5))<<4;
	__temp2 = (GPIO_ReadInputDataBit(GPIOA, GPIO_NODE_ADDR4))<<3;
	__temp3 = (GPIO_ReadInputDataBit(GPIO_DIALSWITCH_PORT, GPIO_NODE_ADDR3))<<2;
	__temp4 = (GPIO_ReadInputDataBit(GPIO_DIALSWITCH_PORT, GPIO_NODE_ADDR2)) <<1;	
	__temp5 = GPIO_ReadInputDataBit(GPIO_DIALSWITCH_PORT, GPIO_NODE_ADDR1);  //modify bug 150318
  __temp = __temp1|__temp2|__temp3|__temp4|__temp5 ;	
	return __temp;
}

/*******************************************************************************
* Function Name  : ReadKeyDown
* Description    : Reads key from demoboard.
* Return         : Return KEY1, KEY2, KEY3, KEY4 or NOKEY
*******************************************************************************/
//u8 ReadKeyDown(void)
//{
//  /* 1 key is pressed */
//  if(KEY_PRESSED(1))
//  {
//		ostmr_wait(ANTI_SHAKE_TIME);
//		if(KEY_PRESSED(1))
//			return KEY1; 
//  }	
//  /* 2 key is pressed */
//  if(KEY_PRESSED(2))
//  {
//		ostmr_wait(ANTI_SHAKE_TIME);
//		if(KEY_PRESSED(2))
//			return KEY2; 
//  }
//  /* 3 key is pressed */
//  if(KEY_PRESSED(3))
//  {
//		ostmr_wait(ANTI_SHAKE_TIME);
//		if(KEY_PRESSED(3))
//			return KEY3; 
//  }
//  /* 4 key is pressed */
//  if(KEY_PRESSED(4))
//  {
//		ostmr_wait(ANTI_SHAKE_TIME);
//		if(KEY_PRESSED(4))
//			return KEY4; 
//  }

//  /* No key is pressed */
//  return NOKEY;
//  
//}
