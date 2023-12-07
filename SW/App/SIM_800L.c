/* SIM_800L.c
 * Reference:


    SIM800Lconnected to UART1

 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32f1xx_hal.h"

#include <stdarg.h>
#include <stdio.h>

#include "SIM_800L.h"
#include "UART_Printf.h"

extern UART_HandleTypeDef huart1;

#define DEBUG 1

#ifdef DEBUG
  #define SIM_Debug(...) UART_printf(__VA_ARGS__)
#else
  #define SIM_Debug(...) (void)0
#endif

#define SIM_MAIN_LOOP_DELAY  1000
#define SIM_RX_BUFFER_SIZE (512)
#define SIM_RX_LINE_SIZE   (256)
//uint8_t RxLine[32];
uint8_t RxBuffer[SIM_RX_BUFFER_SIZE];
uint8_t RxLine[SIM_RX_LINE_SIZE];

#define SIM_TX_BUFFER_SIZE   (40)  // TX via IT
uint8_t TxBuffer[SIM_TX_BUFFER_SIZE];

uint8_t SignalQuality;

typedef enum{
    SIM_OK,
    SIM_OK_WITH_DATA,
    SIM_ERROR,
    SIM_ERROR_WITH_DATA,
    SIM_TIME_OUT
}  SIM_resp_e;


SIM_resp_e SIM_WaitForData( int timeOut ){
    uint8_t Rx[2];

    int RxLineLen = 0;
    uint8_t *pRxBuffer = RxBuffer;
    HAL_StatusTypeDef rc;


    while(1) {
        rc = HAL_UART_Receive(&huart1, Rx, 1, 2*timeOut);
        int c = Rx[0];
        //SIM_Debug( "%02X ", (int)c );
        if ( rc == HAL_TIMEOUT ) {
            SIM_Debug( "return Time Out\r\n" );
            return SIM_TIME_OUT;
        }

        if ( c == '\r' ){
            continue;
        }
        if ( c == '\n' ) {
            // End of the line received
            if (
                RxLineLen == 2 &&
                RxLine[0] == 'O' && RxLine[1] == 'K'
            ) {
                RxLineLen = 0;
                int len = (pRxBuffer-RxBuffer);
                if ( len > 0  ){
                    RxBuffer[len] = '\0';
                    SIM_Debug( "OK(%d): \"%s\"\r\n",len, RxBuffer );
                    return SIM_OK_WITH_DATA;
                }
                else{
                    SIM_Debug( "return OK\r\n" );
                    return SIM_OK;
                }
            }
            if (
                RxLineLen == 5 &&
                RxLine[0] == 'E' && RxLine[1] == 'R' && RxLine[2] == 'R' && RxLine[3] == 'O' && RxLine[4] == 'R'
            ) {
                RxLineLen = 0;
                int len = (pRxBuffer-RxBuffer);
                if ( len > 0 ){
                    RxBuffer[len] = '\0';
                    SIM_Debug( "ERROR(%d): \"%s\"\r\n",len, RxBuffer );
                    return SIM_ERROR_WITH_DATA;
                }
                else{
                    SIM_Debug( "return ERREUR\r\n" );
                    return SIM_ERROR;
                }
            }
            if ( RxLineLen > 0 ){;
                RxLine[RxLineLen] = 0;
                if ( (pRxBuffer+RxLineLen) < RxBuffer+SIM_RX_BUFFER_SIZE-3){
                    memcpy( pRxBuffer, RxLine, RxLineLen );
                    pRxBuffer += RxLineLen;
                    pRxBuffer[0]='\r';
                    pRxBuffer[1]='\n';
                    pRxBuffer += 2;
                }
                else{
                    UART_printf( "Rx Buffer too small!\r\n" );
                }
                RxLineLen = 0;
            }
        }
        else{
            RxLine[RxLineLen] = c;
            if (RxLineLen < SIM_RX_BUFFER_SIZE)
                ++RxLineLen;
        }
    }


    return SIM_OK;
}



// Send command or continuation (e.g. SMS text)
SIM_resp_e SIM_SendCommand( const char* cmd ){
    HAL_UART_Transmit(&huart1, (uint8_t*)"AT", 2, 100 );

    if (cmd) {
        SIM_Debug( "CMD = AT%s\r\n", cmd );
        HAL_UART_Transmit(&huart1, (uint8_t*)cmd, strlen(cmd), 100 );
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)"\r", 1, 100 );
    return SIM_WaitForData(1000);
}


bool SIM_Wait( void ){
#if 0
    SIM_WaitForData( SIM_MAIN_LOOP_DELAY );
#else
    SIM_SendCommand("I");        //Identification
#endif
    // Return true if data available for parsing
    return true;
}


bool SIM_WriteText( char * text ){
    HAL_StatusTypeDef st;

    st = HAL_UART_Transmit(&huart1, (uint8_t*)text, strlen(text), 100 );

    return ( st == HAL_OK );
}

bool SIM_WriteText_f( const char *fmt, ...){
    va_list ap;
    HAL_StatusTypeDef st;

    va_start(ap, fmt);
    vsnprintf( (char*)TxBuffer, SIM_TX_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    SIM_Debug( "WriteText_f: %s\r\n", (char*)TxBuffer );

    st = HAL_UART_Transmit(&huart1, TxBuffer, strlen((char*)TxBuffer), 100 );

    return ( st == HAL_OK );
}


bool SIM_StartMessage( char * phoneNumber ){

    return SIM_WriteText_f("AT+CMGS=\"%s\"\r", phoneNumber);
}


bool SIM_WriteEndOfMessage( void ){
    HAL_StatusTypeDef st;

    uint8_t eot = 0x1A;

    st = HAL_UART_Transmit(&huart1, &eot, 1, 100 );

    return ( st == HAL_OK );
}


void SIM_FlushRxComm( char * tag ){
    HAL_UART_Receive(&huart1, RxLine, 32, 200);
    UART_printf( "%s: %s\r", tag, RxLine );
}


bool SIM_SendSms( char * TextMess ){


    return true;
}


bool SIM_IsReady( char * Response ){
    // Search ": READY"

    if ( strstr( Response, ": READY") != NULL )
        return true;
    else
        return false;

}
/*
Value  RSSI dBm  Condition
2      -109      Marginal
3      -107      Marginal
4      -105      Marginal
5      -103      Marginal
6      -101      Marginal
7      -99       Marginal
8      -97       Marginal
9      -95       Marginal
10     -93       OK
11     -91       OK
12     -89       OK
13     -87       OK
14     -85       OK
15     -83       Good
16     -81       Good
17     -79       Good
18     -77       Good
19     -75       Good
20     -73       Excellent
21     -71       Excellent
22     -69       Excellent
23     -67       Excellent
24     -65       Excellent
25     -63       Excellent
26     -61       Excellent
27     -59       Excellent
28     -57       Excellent
29     -55       Excellent
30     -53       Excellent
*****************************/
int16_t SIM_ReadSignalQuality( void ){
    // Parse +CSQ: 27,0
    int16_t value = -1;

    SIM_SendCommand("+CSQ");    //Signal quality

    // TODO
    char * ptr = strstr((char*)RxBuffer, "CSQ:" );
    if ( ptr != NULL ){
        ptr += 4;
        value = atoi(ptr);
    }
    UART_printf( "SignalQuality = %d\r\n", value );

    SignalQuality = (uint8_t)value;
    return value;
}


int16_t SIM_GetSignalQuality( void ){
    return (int16_t)SignalQuality;
}

/****

ATI
AT+CPIN?
AT+CPIN="1234"
AT
AT+CSQ

AT+CSCS="GSM"
AT+CMGF=1
AT+CNMI=1,2,0,0,0
AT+CMGDA="DEL ALL"
AT&W


AT+CMGL="ALL"
******************************/
bool SIM_Ack(void){
    SIM_resp_e r;
    LED_SIM_OK_on();

    // HW reset ??
    UART_printf( "Configuring SIM card ...\r\n" );

    SIM_SendCommand("E0");       //Disable echo
    SIM_SendCommand("I");        //Identification

    HAL_Delay(500);
    r = SIM_SendCommand("+CPIN?");

    // TODO test response READY or NOT READY ? (+CPIN: READY)
    if ( SIM_IsReady( (char*)RxLine) ){
        UART_printf( "SIM Ready\r\n" );
    }
    else{
        UART_printf( "Setting PIN code\r\n" );
        SIM_SendCommand("+CPIN=\"1234\"");
    }

    HAL_Delay(500);

    r = SIM_SendCommand(""); // ACK command

    LED_SIM_OK_off();

    if ( r == SIM_OK ){
        return true;
    }
    return false;
}



bool SIM_CheckSimStatus(void){
    HAL_Delay(500);
    SIM_SendCommand("+CCID");   //Read SIM information to confirm whether the SIM is plugged
    UART_printf( "SIM ID: %s\r\n", RxLine );
    HAL_Delay(100);

    SIM_SendCommand("+CREG=?");  //Check whether it has registered in the network
    HAL_Delay(100);

    SIM_SendCommand("+COPS ?");
    UART_printf( "SIM OPS: %s\r\n", RxLine );
    HAL_Delay(100);

    // TODO test quality, operator list, ...

    return true;
}



void SIM_ClearAll( void ){
    //delete all sms
    SIM_SendCommand("+CMGD=1,4");
    HAL_Delay(1000);
    SIM_SendCommand("+CMGDA= \"DEL ALL\"");
    HAL_Delay(1000);
}


bool SIM_ConfigureForText(void){

    // GSM 7 bit default alphabet
    SIM_SendCommand("+CSCS=\"GSM\"");
    HAL_Delay(100);
    // Message in format text
    SIM_SendCommand("+CMGF=1");
    HAL_Delay(100);

    // Route SMS to serial
    //SIM_SendCommand("+CNMI=1,2,0,0,0");
    // Do not route SMS to serial, (No SMS-DELIVER notification)
    SIM_SendCommand("+CNMI=0,0,0,0,0");
    HAL_Delay(100);

    //// Save configuration
    //SIM_SendCommand("&W");
    //HAL_Delay(100);

    return true;
}


/*
AT+CMGL= "REC UNREAD"       +CMGL : , "REC UNREAD","<n° de tél>,"",     Affiche tous les SMS non lus
AT+CMGL= "REC READ"         +CMGL : , "REC READ","<n° de tél>,"",       Affiche tous les SMS lus
AT+CMGL= "STO UNSENT READ"                                              Enregistre les SMS non envoyés
AT+CMGL= "STO SENT READ"                                                Enregistre les SMS envoyés
AT+CMGL="ALL"               +CMGL : , "ALL","<n° de tél>,"",            Affiche tous les messages
AT+CMGD=1                                                               Supprime les SMS déjà lus
AT+CMGD=2                                                               Supprime les SMS lus et non envoyés
AT+CMGD=3                                                               Supprime les SMS lus, envoyés et non envoyés
AT+CMGD=4                                                               Supprime tous les SMS (lus, non lus, envoyés et non envoyés)

By default, the modem keeps all received SMS messages saved. This will lead to the SMS buffer eventually filling up,
at which point no further SMS can be received.
This buffer can simply be cleared by executing the command at+cmgda="DEL ALL"
******************************************************************************************************************/
bool SIM_CheckSMS(void){
    if ( SIM_OK_WITH_DATA == SIM_SendCommand("+CMGL=\"ALL\"") ){
        UART_printf( "Message: %s\r\n", RxBuffer );
        // Process message

        // Delete message
        //SIM_ClearAll();
        return true;
    }
    else{
        return false;
    }
}
