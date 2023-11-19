/* Temperature.h
   Temperatue values management


 */

#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include "stdbool.h"

void Temp_HistoryInit( void );


void Temp_NewValues( int16_t temp1, int16_t temp2);

int16_t Temp_HistoryGetMax( void );
int16_t Temp_HistoryGetMin( void );

int16_t Temp_LowThreshold( int16_t value );
int16_t Temp_HighThreshold( int16_t value );

bool Temp_AlarmsCheck( void );

#endif // _TEMPERATURE_H_
