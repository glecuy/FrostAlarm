/* App/SIM_800L.h
 * Reference:

 TLY25/26/27/28/29/35 Communication protocol User’s manual (Tecnologic S.p.A.)

 */

#ifndef _SIM_800L_H_
#define _SIM_800L_H_

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

void SIM_EndOfRx(void);
void SIM_EndOfTx(void);

bool SIM_Ack(void);

bool SIM_CheckSimStatus(void);

bool SIM_ConfigureForText(void);

bool SIM_CheckSMS(void);

int16_t SIM_ReadSignalQuality( void );
int16_t SIM_GetSignalQuality( void );

bool SIM_WriteText( char * text );
bool SIM_StartMessage( char * phoneNumber );
bool SIM_WriteEndOfMessage( void );

void SIM_FlushRxComm( char * tag );

#ifdef __cplusplus
}
#endif
#endif //_SIM_800L_H_
