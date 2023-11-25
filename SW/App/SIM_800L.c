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

#include "SIM_800L.h"
#include "UART_Printf.h"

extern UART_HandleTypeDef huart1;
volatile uint8_t uart1TxDone;

//uint8_t RxBuffer[32];
uint8_t RxBuffer[512];

uint8_t SignalQuality;

void SIM_EndOfRx(void){
}

void SIM_EndOfTx(void){
    uart1TxDone = 1;
}


bool SIM_WriteText( char * text ){
    HAL_StatusTypeDef st;

    for ( int i=0 ; i<strlen(text) ; i++ ){
        UART_printf( "%02X ", text[i] );
    }
    UART_printf( "\r\n");

    if ( uart1TxDone != 0 ){
        HAL_Delay (10);
    }

    st = HAL_UART_Transmit_IT(&huart1, (uint8_t*)text, strlen(text) );

    return ( st == HAL_OK );
}


bool SIM_StartMessage( char * phoneNumber ){
    char ligne[32];

    snprintf(ligne, 32, "AT+CMGS=\"%s\"\r", phoneNumber);

    UART_printf( "\"  %s  \"\r\n", ligne );
    return SIM_WriteText(ligne);
}



bool SIM_WriteEndOfMessage( void ){
    HAL_StatusTypeDef st;

    uint8_t eot = 0x1A;

    if ( uart1TxDone != 0 ){
        HAL_Delay (10);
    }

    st = HAL_UART_Transmit_IT(&huart1, &eot, 1 );

    return ( st == HAL_OK );
}


void SIM_FlushRxComm( char * tag ){
    HAL_UART_Receive(&huart1, RxBuffer, 32, 200);
    UART_printf( "%s: %s\r", tag, RxBuffer );
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

    SIM_WriteText("AT+CSQ\n");    //Signal quality
    RxBuffer[0] = '\0';
    HAL_UART_Receive(&huart1, RxBuffer, 32, 100);

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


bool SIM_Ack(void){
    HAL_StatusTypeDef st;

    LED_SIM_OK_on();

    // HW reset ??

    SIM_WriteText("ATE0\n");        //Disable echo
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);  // Wait 50 mSec
    SIM_WriteText("ATI\n");          //Identification
    RxBuffer[0] = '\0';
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    RxBuffer[14] = '\0';
    UART_printf( "SIM800L release %s\r\n", RxBuffer );

    HAL_Delay(500);
    SIM_WriteText("AT+CPIN?\n");
    RxBuffer[0] = '\0';
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    UART_printf( "CPIN?: %s\r", RxBuffer );

    // TODO test response READY or NOT READY ? (+CPIN: READY)
    if ( SIM_IsReady( (char*)RxBuffer ) ){
        UART_printf( "SIM Ready\r\n" );
    }
    else{
        UART_printf( "Setting PIN code\r\n" );
        SIM_WriteText("AT+CPIN=\"1234\"\n");
        RxBuffer[0] = '\0';
        HAL_UART_Receive(&huart1, RxBuffer, 32, 200);
        UART_printf( "CPIN: %s\r", RxBuffer );
    }

    HAL_Delay(500);
    // Try to flush RX
    HAL_UART_Receive(&huart1, RxBuffer, 32, 100);

    SIM_WriteText("AT\r\n");          //ACK command
    // Check response ("OK")
    st = HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    //UART_printf( "Resp=%02X %02X %02X %02X\r\n", RxBuffer[0], RxBuffer[1], RxBuffer[2], RxBuffer[3] );

    LED_SIM_OK_off();

    if ( RxBuffer[2]=='O' && RxBuffer[3]=='K' ){
        return true;
    }
    // else
    UART_printf( "SIM800L not OK: %s (%d)\r\n", RxBuffer, st );
    return false;
}


bool SIM_ConfigureForText(void){
    SIM_WriteText("AT+CSCS=\"GSM\"\n"); // GSM 7 bit default alphabet
    HAL_Delay(100);
    SIM_WriteText("AT+CMGF=1\n");       // Message in format text
    HAL_Delay(500);

#if 0
    UART_printf( "Sending message ! \r\n" );

    SIM_WriteText("AT+CMGS=\"+33687908087\"\r"); // phone number
    HAL_Delay(200);
    SIM_WriteText("Hello how are you?\r"); // message
    HAL_Delay(200);
    SIM_WriteEndOfMessage();  //send a Ctrl+Z (end of the message)

    HAL_UART_Receive(&huart1, RxBuffer, 32, 200);
    UART_printf( "GMGS: %s\r", RxBuffer );
#endif

    return true;
}


bool SIM_CheckSimStatus(void){
    HAL_Delay(500);
    SIM_WriteText("AT+CCID\n");   //Read SIM information to confirm whether the SIM is plugged
    RxBuffer[0] = '\0';
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    UART_printf( "CCID: %s\r", RxBuffer );
    HAL_Delay(100);

    SIM_WriteText("AT+CREG=?\n");  //Check whether it has registered in the network
    RxBuffer[0] = '\0';
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    UART_printf( "CREG=?: %s\r", RxBuffer );
    HAL_Delay(100);

    SIM_WriteText("AT+CSQ\n");    //Signal quality test, value range is 0-31 , 31 is the best
    RxBuffer[0] = '\0';
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    UART_printf( "CSQ: %s\r", RxBuffer );
    HAL_Delay(100);

    SIM_WriteText("AT+COPS ?\n");
    RxBuffer[0] = '\0';
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    UART_printf( "COPS: %s\r", RxBuffer );
    HAL_Delay(100);

    // TODO test quality, operator list, ...

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
    uint16_t RxLen;
    SIM_WriteText("AT+CMGL=\"ALL\"\n");   //Read SIM information to confirm whether the SIM is plugged
    RxBuffer[0] = '\0';
    HAL_UARTEx_ReceiveToIdle(&huart1, RxBuffer, 512, &RxLen, 100);
    UART_printf( "RxLen=%d\r\n", RxLen );
    UART_printf( "CMGL1: %s\r\n", RxBuffer );
    HAL_UARTEx_ReceiveToIdle(&huart1, RxBuffer, 512, &RxLen, 100);
    UART_printf( "RxLen=%d\r\n", RxLen );
    UART_printf( "CMGL2: %s\r\n", RxBuffer );
    HAL_UARTEx_ReceiveToIdle(&huart1, RxBuffer, 512, &RxLen, 100);
    UART_printf( "RxLen=%d\r\n", RxLen );
    UART_printf( "CMGL3: %s\r\n", RxBuffer );

    return true;
}
