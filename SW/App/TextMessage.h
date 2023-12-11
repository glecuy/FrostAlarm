/* TextMessage.h



 */

#ifndef _TEXT_MESSAGE_H_
#define _TEXT_MESSAGE_H_

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

bool TextDefaultConfig(void);
bool TextSendStatusMessage(char * title);

bool TextIncomingMessageProcess(char * pMess);

#ifdef __cplusplus
}
#endif
#endif //_TEXT_MESSAGE_H_
