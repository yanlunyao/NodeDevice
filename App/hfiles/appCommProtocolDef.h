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
#define CMD_SUCESS_WITH_DATA           (0x01)
#define CMD_FAIL                       (0x02)
#define CMD_FAIL_WITH_DATA             (0x03)
#define MAX_CMD_RSP_TYPE               (0x03)

//commandType
#define  REQ_INQ                       (0x00)
#define  SET_LOAD                      (0x01)
#define  VERIFICATION                  (0x02)
#define  RESPOND_CMD                       (0x03)
#define  CANCEL                        (0x04)

#endif /* APPCOMMPROTOCOLDEF_H_ */
