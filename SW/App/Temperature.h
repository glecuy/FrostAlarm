/* Temperature.h
   Temperatue values management


 */

#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include "stdbool.h"

void Temp_HistoryInit( void );


typedef enum{
    TEMP_NORMAL,
    TEMP_HIGH,
    TEMP_LOW
}  TEMP_TH_e;

void Temp_NewValues( int16_t temp1, int16_t temp2);

int16_t Temp_HistoryGetMax( void );
int16_t Temp_HistoryGetMin( void );

int16_t Temp_GetT1( void );
int16_t Temp_GetT2( void );

char * formatTemp( int16_t t, char * str );

TEMP_TH_e Temp_AlarmsCheck( void );

#endif // _TEMPERATURE_H_
