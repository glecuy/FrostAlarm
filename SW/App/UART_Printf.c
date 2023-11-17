
#include "main.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_tx;

#include <stdarg.h>
#include <stdio.h>

void UART_printf( const char *fmt, ...);

#define UART3_TX_BUFF_LEN (80)
// Send text up to 80 chars at once
uint8_t uart3TxBuffer[UART3_TX_BUFF_LEN];

volatile uint8_t uart3TxDone;


void PrintfEndOfTx(void){
    uart3TxDone = 1;
}

void UART_printf( const char *fmt, ...)
{
    va_list ap;

    if ( uart3TxDone == 0 ){
        // Wait a while, current message to be sent !
        HAL_Delay (10);
    }
    va_start(ap, fmt);
    vsnprintf( (char*)uart3TxBuffer, UART3_TX_BUFF_LEN, fmt, ap);
    va_end(ap);

    uart3TxDone = 0;
    HAL_UART_Transmit_IT(&huart3, uart3TxBuffer, strlen((char*)uart3TxBuffer));
    //HAL_UART_Transmit(&huart3, uart3TxBuffer, strlen((char*)uart3TxBuffer), 0xFFFF); uart3TxDone = 1;

}




