/* Temperature.c
   Temperatue values management

   Tempearture unit is 10th of degree C

 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "stm32f1xx_hal.h"

#include "SIM_800L.h"
#include "UART_Printf.h"

#define TEMP_16_UNDEF INT16_MIN
#define TEMP_HISTORY_DEPTH_PTS (256)
#define TEMP_HISTORY_DEPTH_SEC (36*3600)
#define TEMP_TICK (5)

#define TEMP_HISTORY_N_MAX (TEMP_HISTORY_DEPTH_SEC/(TEMP_HISTORY_DEPTH_PTS)/TEMP_TICK)

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
 * N_Max = (24*3600)/256  1 Sec ssamples
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



int16_t Temp_LowThreshold( int16_t value ){

    return value;
}


int16_t Temp_HighThreshold( int16_t value ){

    return value;
}



void Temp_NewValues( int16_t temp1, int16_t temp2){

    Temp_HistoryAddValue( temp1 );
    UART_printf( "T1=%d, T2=%d\r\n", (int)temp1, (int)temp2 );
}


bool Temp_AlarmsCheck( void ){
    return false;
}
