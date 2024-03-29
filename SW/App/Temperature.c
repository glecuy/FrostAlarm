/* Temperature.c
   Temperatue values management

   Tempearture unit is 10th of degree C

 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32f1xx_hal.h"

#include "UserData.h"
#include "SIM_800L.h"
#include "Temperature.h"
#include "UART_Printf.h"

#define TEMP_16_UNDEF INT16_MIN
#define TEMP_HISTORY_DEPTH_PTS (256)
#define TEMP_HISTORY_DEPTH_SEC (36*3600)
#define TEMP_TICK (5)

#define TEMP_HISTORY_N_MAX (TEMP_HISTORY_DEPTH_SEC/(TEMP_HISTORY_DEPTH_PTS)/TEMP_TICK)


//#define DEBUG 1

#ifdef DEBUG
  #define TEMP_Debug(...) UART_printf(__VA_ARGS__)
#else
  #define TEMP_Debug(...) (void)0
#endif

int16_t T1, T2;

uint16_t TempLow, TempHigh, TempError;


extern UserData_t PermanentData;

struct
{
    int16_t Values[TEMP_HISTORY_DEPTH_PTS];
    int16_t N;
    int16_t Index;
    int32_t Acc;
}TempHistory;


/* Create history with a sliding window (depth)
 * N_Max is the number of temp samples used for 1 record
 * ex:
 * For 24 hours history depth:
 * N_Max = (24*3600)/256  1 Sec ssamples (337)
 * For 36 hours history depth:
 * N_Max = (36*3600)/256  5 Sec ssamples (101)
 *
*/
void Temp_HistoryInit( void ){
    int i;

    for ( i=0 ; i<TEMP_HISTORY_DEPTH_PTS ; i++ )
    {
        TempHistory.Values[i] = TEMP_16_UNDEF;
    }
    TempHistory.Index = 0;
    TempHistory.N = 0;
    TempHistory.Acc = 0;

    TempLow  = 0;
    TempHigh = 0;
}

/* Add value to history
 * Params T Temp in 10th degrees
 *
 **********************************************/
void Temp_HistoryAddValue( int16_t T ){

    TempHistory.Acc += (int32_t)T;
    TempHistory.N++;

    /* Test to jump to next table entry */
    if ( TempHistory.N > TEMP_HISTORY_N_MAX ){
        TempHistory.Values[TempHistory.Index] = TempHistory.Acc / TEMP_HISTORY_N_MAX;
        TempHistory.Index++;
        if ( TempHistory.Index >= TEMP_HISTORY_DEPTH_PTS )
            TempHistory.Index = 0;
        TempHistory.N   = 0;
        TempHistory.Acc = 0;
        UART_printf("TempHistory.Index=%d\r\n", TempHistory.Index );
    }
}


int16_t Temp_HistoryGetMax( void )
{
    int i;
    int16_t max = INT16_MIN;

    for ( i=0 ; i< TEMP_HISTORY_DEPTH_PTS ; i++ )
    {
        if (
                (TempHistory.Values[i] != TEMP_16_UNDEF)
            &&  (TempHistory.Values[i] > max)
            )
        {
            max = TempHistory.Values[i];
        }
    }
    if ( max == INT16_MIN )
        return -999;
    else
        return max;
}

int16_t Temp_HistoryGetMin( void )
{
    int i;
    int16_t min = INT16_MAX;

    for ( i=0 ; i< TEMP_HISTORY_DEPTH_PTS ; i++ )
    {
        if (
                (TempHistory.Values[i] != TEMP_16_UNDEF)
            &&  (TempHistory.Values[i] < min)
            )
        {
            min = TempHistory.Values[i];
        }
    }
    if ( min == INT16_MAX )
        return +999;
    else
        return min;
}

// Format temp value from tenth of degree to fixed point string
// 172  ==> "17.2"
// str MUST be 8 char long !
char * formatTemp( int16_t t, char * str ){

    snprintf(str, 8, "%d.%d", t/10, abs(t%10) );
    return str;
}


int16_t Temp_GetT1( void )
{
    return T1;
}

int16_t Temp_GetT2( void )
{
    return T2;
}

int16_t Temp_GetT_Case( void )
{
    extern int16_t T_Case;
    return T_Case;
}


void Temp_NewValues( int16_t temp1, int16_t temp2){

    Temp_HistoryAddValue( temp1 );
    T1 = temp1;
    T2 = temp2;
    //UART_printf( "T1=%d, T2=%d\r\n", (int)temp1, (int)temp2 );
}

void Temp_SetError( void ){
    T1 = TEMP_16_UNDEF;
    T2 = TEMP_16_UNDEF;
}


/*
 * Test T1 temperature against defined thresholds
 * 3 in a raw shall trig an alarm
 ***************************************************/
TEMP_TH_e Temp_AlarmsCheck( void ){

    if ( T1 == TEMP_16_UNDEF || T2 == TEMP_16_UNDEF ){
        TempError++;
        TEMP_Debug("TempError = %u\r\n", TempError);
    } else {
        TempError = 0;
        if( T1 < PermanentData.L_Thresholds ){
            TempLow++;
            TEMP_Debug("TempLow = %u\r\n", TempLow);
        } else {
            TempLow = 0;
        }
        if( T1 > PermanentData.H_Thresholds ){
            TempHigh++;
        } else {
            TempHigh = 0;
        }
    }

    if ( TempHigh >= 3 ){
        return TEMP_HIGH;
    }
    else if ( TempLow >= 3 ){
        return TEMP_LOW;
    }
    else if ( TempError >= 3 ){
        return TEMP_ERROR;
    }
    else{
        return TEMP_NORMAL;
    }
}
