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

void SIM_Init(void);

#ifdef __cplusplus
}
#endif
#endif //_SIM_800L_H_
