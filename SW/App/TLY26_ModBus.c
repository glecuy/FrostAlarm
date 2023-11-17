/*
 Very poor implementation of MODBUS-RTU protocol

 One Master (This controller), one slave (TLY26)  connected via rs485 link.

 * Reference:
 TLY25/26/27/28/29/35 Communication protocol User’s manual (Tecnologic S.p.A.)

 */


/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "main.h"
#include "stm32f1xx_hal.h"
#include "TLY26_ModBus.h"
#include "UART_Printf.h"

extern UART_HandleTypeDef huart2;

volatile uint8_t uart2TxDone;

uint8_t ModBusSlaveAddress;
uint8_t ModBusFrame[MODBUS_MAX_LEN];


void DumpFrame( int len ){
    uint8_t * ptw = ModBusFrame;
    UART_printf( "Frame: %02X ", (int)*ptw++ );
    for ( int i=0 ; i< len -1 ; i++ )
        UART_printf( "%02X ", (int)*ptw++ );
    UART_printf( "\r\n" );
}

/* CRC16-MODBUS calculation
 * From https://community.st.com/ forum
 * Differs with bit order and init value !
 */
unsigned int ModBusCRC16(unsigned char *buf, int len)
{
    unsigned int crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++){
        crc ^= (unsigned int)buf[pos];  // XOR byte into least sig. byte of crc
        for (int i = 8; i != 0; i--) {  // Loop over each bit
            if ((crc & 0x0001) != 0) {  // If the LSB is set
                crc >>= 1;              // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else                        // Else LSB is not set
                crc >>= 1;              // Just shift right
        }
    }
    return crc;
}



bool TLY26_Init( uint8_t slaveAddress ){

    ModBusSlaveAddress = slaveAddress;
    uart2TxDone = 1;

    // Turn ModBus in RX
    HAL_GPIO_WritePin  (MODBUS_TX_ENABLE_Port, MODBUS_TX_ENABLE_Pin, GPIO_PIN_RESET);

    return true;
}


static bool TLY26_ReadFrame( uint16_t * pData, uint16_t n ){
    HAL_StatusTypeDef st;

    memset(ModBusFrame, 0, n );

    LED3_off();
    //st = HAL_UART_Receive(&huart2, ModBusFrame, n, 10);
    st = HAL_UART_Receive(&huart2, ModBusFrame, n, 20);
    LED3_on();

    DumpFrame(n);
    //UART_printf( "NB=%d (%02X %02X)\r\n", (int)ModBusFrame[2], (int)ModBusFrame[0], (int)ModBusFrame[1] );

    uint16_t w;
    uint8_t *ptr = &ModBusFrame[3];
    for ( int i=0 ; i<n ; i++ ){
        w = ptr[0]<<8 | ptr[1]<<0;
        *pData = w;
        pData +=1;
        ptr += 2;
    }

    // crc
    uint16_t crc = ModBusCRC16(ModBusFrame, n-2);
    w   = ModBusFrame[n-2]<<0 | ModBusFrame[n-1]<<8;
    //UART_printf( "CRC : %04X/%04X\r\n", w, crc);
    if ( w != crc ){
        UART_printf( "CRC error: %04X/%04X\r\n", w, crc);
        return false;
    }

    return ( st == HAL_OK );
}

/*

Communication protocol TLY25/27/28/29/35
 * 3.2 Function 6 - writing a word
*/
bool TLY26_writeWord( uint16_t address, uint16_t Data ){
    HAL_StatusTypeDef st;

    // Build ModBus frame
    ModBusFrame[0] = ModBusSlaveAddress;
    ModBusFrame[1] = 6;                         // Function Write
    ModBusFrame[2] = (uint8_t)(address>>8);     // MSB
    ModBusFrame[3] = (uint8_t)(address&0x00FF); // LSB

    ModBusFrame[4] = (uint8_t)(Data>>8);     // MSB
    ModBusFrame[5] = (uint8_t)(Data&0x00FF); // LSB

    // CRC 16
    unsigned int crc = ModBusCRC16(ModBusFrame, 6);

    /* ! Low order byte of the CRC comes first in the RTU message */
    ModBusFrame[6] = crc & 0x00FF;
    ModBusFrame[7] = crc >> 8;

    HAL_GPIO_WritePin (MODBUS_TX_ENABLE_Port, MODBUS_TX_ENABLE_Pin, GPIO_PIN_SET);

    st = HAL_UART_Transmit_IT(&huart2, ModBusFrame, 8);

    // TODO  Timing !!
    HAL_GPIO_WritePin  (MODBUS_TX_ENABLE_Port, MODBUS_TX_ENABLE_Pin, GPIO_PIN_RESET);

    // Wait for response



    return ( st == HAL_OK );
}

/*

Communication protocol TLY25/27/28/29/35
 * 3.2 Function 3 - reading of n words
*/
bool TLY26_ReadWords( uint16_t address, uint16_t * pData, uint16_t n ){
    HAL_StatusTypeDef st;
    bool ok = false;

    // Build ModBus frame
    ModBusFrame[0] = ModBusSlaveAddress;
    ModBusFrame[1] = 3;                         // Function 3 - reading of n words
    ModBusFrame[2] = (uint8_t)(address>>8);     // MSB
    ModBusFrame[3] = (uint8_t)(address&0x00FF); // LSB

    ModBusFrame[4] = (uint8_t)(n>>8);           // MSB
    ModBusFrame[5] = (uint8_t)(n&0x00FF);       // LSB

    // CRC 16
    unsigned int crc = ModBusCRC16(ModBusFrame, 6);

    /* ! Low order byte of the CRC comes first in the RTU message */
    ModBusFrame[6] = crc & 0x00FF;
    ModBusFrame[7] = crc >> 8;

    //DumpFrame(8);

    uart2TxDone = 0;
    HAL_GPIO_WritePin  (MODBUS_TX_ENABLE_Port, MODBUS_TX_ENABLE_Pin, GPIO_PIN_SET);

    st = HAL_UART_Transmit_IT(&huart2, ModBusFrame, 8);

    while( uart2TxDone == 0 ){
        HAL_Delay(10);
    }

    if ( st == HAL_OK ){
        // Wait for response
        ok = TLY26_ReadFrame( pData, n*2+5 );
    }

    return ok;
}

bool TLY26_WriteWords( uint16_t address, uint16_t * pData, uint16_t n ){
    return false;
}


bool TLY26_Terminate( uint8_t slaveAddress ){
    return true;
}


void TLY26_EndOfRx(void){
}


void TLY26_EndOfTx(void){
    HAL_GPIO_WritePin  (MODBUS_TX_ENABLE_Port, MODBUS_TX_ENABLE_Pin, GPIO_PIN_RESET);
    uart2TxDone = 1;
}

