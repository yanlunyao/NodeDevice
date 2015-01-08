/*
----------------------------------------------------------------------------
File   :appCommProtocolDef.h
Purpose:
Version:
History:
Name          Date          Comment
*/
#ifndef APPCOMMPROTOCOLDEF_H_
#define APPCOMMPROTOCOLDEF_H_

/*
************************************************
protocol info
************************************************/

//respond type
#define CMD_SUCESS                     (0x00)
//#define CMD_SUCESS_WITH_DATA           (0x01)
#define CMD_FAIL                       (0x01)
//#define CMD_FAIL_WITH_DATA             (0x03)
//#define MAX_CMD_RSP_TYPE               (0x03)

//commandType
#define	UPLOAD_CMD                     (0x00)
#define SET_LOAD                       (0x01)
#define RESPOND_CMD                    (0x02)
#define MAX_CMDTYPE                    (0x03)

//cmd
#define	UPLOAD_ALARM_STATE_CHANGE_CMD	  0x00
#define	CONTROLL_SMART_SOCKET_CMD	  		0x01
#define CHECK_SMART_SOCKET_CMD					0x02
#define MAX_CMD_NUM						          3    



#endif /* APPCOMMPROTOCOLDEF_H_ */
