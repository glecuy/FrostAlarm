/* TextMessage.h



 */

#ifndef _TEXT_MESSAGE_H_
#define _TEXT_MESSAGE_H_

#include "stdint.h"
#include "stdbool.h"

#include "Temperature.h"

#ifdef __cplusplus
extern "C" {
#endif

bool TextDefaultConfig(void);

bool TextSendInitialMessage(void);
bool TextSendStatusMessage(char * title);
bool TextSendAlarm(TEMP_TH_e status, int userNo );

bool TextIncomingMessageProcess(char * pMess);

#ifdef __cplusplus
}
#endif
#endif //_TEXT_MESSAGE_H_
