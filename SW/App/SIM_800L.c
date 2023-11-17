/* SIM_800L.h
 * Reference:


    SIM800Lconnected to UART1

 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "stm32f1xx_hal.h"

#include "SIM_800L.h"

extern UART_HandleTypeDef huart1;
volatile uint8_t uart1TxDone;

void SIM_EndOfRx(void){
}

void SIM_EndOfTx(void){
    uart1TxDone = 1;
}

bool SIM_WriteText( char * text ){
    HAL_StatusTypeDef st;

    while ( uart1TxDone == 0 ){
        HAL_Delay (10);
    }

    st = HAL_UART_Transmit_IT(&huart1, (uint8_t*)text, strlen(text) );
    return ( st == HAL_OK );
}


bool SIM_SendSms( char * TextMess ){


    return true;
}

void SIM_Init(void){
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
    SIM_SendSms("Khoi dong xong roi!!");
    HAL_Delay(500);
}
