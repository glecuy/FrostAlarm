/* App/TLY26_ModBus.h
 * Reference:

 TLY25/26/27/28/29/35 Communication protocol User’s manual (Tecnologic S.p.A.)

 */

#ifndef _TLY26_MODBUS_H_
#define _TLY26_MODBUS_H_

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MODBUS_MAX_LEN 32   // TODO

bool TLY26_Init( uint8_t slaveAddress );

bool TLY26_writeWord( uint16_t address, uint16_t Data );

bool TLY26_ReadWords( uint16_t address, uint16_t * pData, uint16_t n );

bool TLY26_WriteWords( uint16_t address, uint16_t * pData, uint16_t n );

bool TLY26_Terminate( uint8_t slaveAddress );

void TLY26_EndOfRx(void);
void TLY26_EndOfTx(void);

#ifdef __cplusplus
}
#endif
#endif //_MODBUS_H_
