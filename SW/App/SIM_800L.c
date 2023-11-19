/* SIM_800L.c
 * Reference:


    SIM800Lconnected to UART1

 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "stm32f1xx_hal.h"

#include "SIM_800L.h"
#include "UART_Printf.h"

extern UART_HandleTypeDef huart1;
volatile uint8_t uart1TxDone;

uint8_t RxBuffer[32];

void SIM_EndOfRx(void){
}

void SIM_EndOfTx(void){
    uart1TxDone = 1;
}


bool SIM_WriteText( char * text ){
    HAL_StatusTypeDef st;

    if ( uart1TxDone != 0 ){
        HAL_Delay (10);
    }

    st = HAL_UART_Transmit_IT(&huart1, (uint8_t*)text, strlen(text) );

    return ( st == HAL_OK );
}


bool SIM_SendSms( char * TextMess ){


    return true;
}

bool SIM_Ack(void){
    HAL_StatusTypeDef st;

    LED3_on();

    SIM_WriteText("ATE0\r\n");        //Disable echo
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);  // Wait 50 mSec
    SIM_WriteText("ATI\r\n");          //Idetification
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    RxBuffer[14] = '\0';
    UART_printf( "SIM800L release %s\r\n", RxBuffer );

    memset(RxBuffer, 0, 32 );
    SIM_WriteText("AT\r\n");          //ACK command
    // Check response ("OK")
    st = HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    //UART_printf( "Resp=%02X %02X %02X %02X\r\n", RxBuffer[0], RxBuffer[1], RxBuffer[2], RxBuffer[3] );

    LED3_off();

    if ( RxBuffer[2]=='O' && RxBuffer[3]=='K' ){
        return true;
    }
    // else
    UART_printf( "SIM800L not OK: %s (%d)\r\n", RxBuffer, st );
    return false;
}


bool SIM_Configure(void){
    SIM_WriteText("AT+CSCS=\"GSM\"\r\n"); // GSM 7 bit default alphabet
    HAL_Delay(100);
    SIM_WriteText("AT+CMGF=1\r\n");       // Message in format text
    HAL_Delay(500);

    return true;
}

bool SIM_CheckSimStatus(void){
    SIM_WriteText("AT+CCID");   //Read SIM information to confirm whether the SIM is plugged
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    UART_printf( "CCID: %s\r\n", RxBuffer );

    SIM_WriteText("AT+CREG=?");  //Check whether it has registered in the network
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    UART_printf( "CREG=?: %s\r\n", RxBuffer );

    SIM_WriteText("AT+CSQ");    //Signal quality test, value range is 0-31 , 31 is the best
    HAL_UART_Receive(&huart1, RxBuffer, 32, 50);
    UART_printf( "CSQ: %s\r\n", RxBuffer );

    return true;
}


/************************************************
    SIM_WriteText("AT\r\n");          //ACK command
    HAL_Delay(500);
    SIM_WriteText("ATE0\r\n");        //disable echo
    HAL_Delay(500);
    SIM_WriteText("AT&W\r\n");        // save this command
    HAL_Delay(500);
    SIM_WriteText("AT+CSCS=\"GSM\"\r\n");
    HAL_Delay(500);
    SIM_WriteText("AT+CMGF=1\r\n");   //format text
    HAL_Delay(500);
    SIM_WriteText("AT+CNMI=1,2,0,0,0\r\n");
    HAL_Delay(500);
    SIM_WriteText("AT+CSAS\r\n");
    HAL_Delay(500);
****************************************************/
