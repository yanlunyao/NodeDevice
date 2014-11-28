
//#include "apprf315tx.h"


//u8 g_node_num;

//void send_channel1_315M_msg(u8 channel_id);
//void send_channel2_315M_msg(u8 channel_id);
//void send_channel3_315M_msg(u8 channel_id);
//void send_channel4_315M_msg(u8 channel_id);

////for instance: (* (send_315M_msg[0]))(1);
//const FunctionPtr send_315M_msg[]= 
//{
//	send_channel1_315M_msg,												//0			
//	send_channel2_315M_msg,												//1					
//	send_channel3_315M_msg,												//2		
//	send_channel4_315M_msg,										    //3
//};

///********************************************************************************
//  Function:			send_channel1_315M_msg(u8 channel_id)
//  Description:		根据通道1对应的命令ID封装315无线发送模块发送的信息
//  Input:			channel_id
//  Output:			
//  Return:			
//  Others:			
//********************************************************************************/
//void send_channel1_315M_msg(u8 channel_id)
//{
//	u8 data_buf[3];

//	switch(channel_id)
//	{
//		case 1://单开单关
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0x1;
//			data_buf[2] = 0x1;
//			rf315_send(data_buf);
//			//printf("***channel1 send config id = 1***\r\n");
//			break;
//		}
//		case 2://总开
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0x2;
//			data_buf[2] = 0x2;
//			rf315_send(data_buf);
//			//printf("***channel1 send config id = 2***\r\n");
//			break;
//		}
//		case 3://总关
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0x3;
//			data_buf[2] = 0x3;
//			rf315_send(data_buf);
//			//printf("***channel1 send config id = 3***\r\n");
//			break;
//		}
//		default:break;
//	}//switch(id)
//}

///********************************************************************************
//  Function:			send_channel2_315M_msg(u8 channel_id)
//  Description:		根据通道2对应的命令ID封装315无线发送模块发送的信息
//  Input:			channel_id
//  Output:			
//  Return:			
//  Others:			
//********************************************************************************/
//void send_channel2_315M_msg(u8 channel_id)
//{
//	u8 data_buf[3];

//	switch(channel_id)
//	{
//		case 1://单开单关
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0x4;
//			data_buf[2] = 0x4;
//			rf315_send(data_buf);
//			//printf("***channel2 send config id = 4***\r\n");
//			break;
//		}
//		case 2://总开
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0x5;
//			data_buf[2] = 0x5;
//			rf315_send(data_buf);
//			//printf("***channel2 send config id = 5***\r\n");
//			break;
//		}
//		case 3://总关
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0x6;
//			data_buf[2] = 0x6;
//			rf315_send(data_buf);
//			//printf("***channel2 send config id = 6***\r\n");
//			break;
//		}
//		default:break;
//	}//switch(id)
//}

///********************************************************************************
//  Function:			send_channel3_315M_msg(u8 channel_id)
//  Description:		根据通道3对应的命令ID封装315无线发送模块发送的信息
//  Input:			channel_id
//  Output:			
//  Return:			
//  Others:			
//********************************************************************************/
//void send_channel3_315M_msg(u8 channel_id)
//{
//	u8 data_buf[3];

//	switch(channel_id)
//	{
//		case 1://单开单关
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0x7;
//			data_buf[2] = 0x7;
//			rf315_send(data_buf);
//			//printf("***channel3 send config id = 7***\r\n");
//			break;
//		}
//		case 2://总开
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0x8;
//			data_buf[2] = 0x8;
//			rf315_send(data_buf);
//			//printf("***channel3 send config id = 8***\r\n");
//			break;
//		}
//		case 3://总关
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0x9;
//			data_buf[2] = 0x9;
//			rf315_send(data_buf);
//			//printf("***channel3 send config id = 9***\r\n");
//			break;
//		}
//		default:break;
//	}//switch(id)
//}

///********************************************************************************
//  Function:			send_channel4_315M_msg(u8 channel_id)
//  Description:		根据通道4对应的命令ID封装315无线发送模块发送的信息
//  Input:			channel_id
//  Output:			
//  Return:			
//  Others:			
//********************************************************************************/
//void send_channel4_315M_msg(u8 channel_id)
//{
//	u8 data_buf[3];

//	switch(channel_id)
//	{
//		case 1://单开单关
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0xa;
//			data_buf[2] = 0xa;
//			rf315_send(data_buf);
//			//printf("***channel4 send config id = a***\r\n");
//			break;
//		}
//		case 2://总开
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0xb;
//			data_buf[2] = 0xb;
//			rf315_send(data_buf);
//			//printf("***channel4 send config id = b***\r\n");
//			break;
//		}
//		case 3://总关
//		{
//			//data_buf[0] = 0x0;
//			data_buf[0] =g_node_num;
//			data_buf[1] = 0xc;
//			data_buf[2] = 0xc;
//			rf315_send(data_buf);
//			//printf("***channel4 send config id = c***\r\n");
//			break;
//		}
//		default:break;
//	}//switch(id)
//}

