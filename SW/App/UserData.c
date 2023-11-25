/* UserData.c

   Permanent user data stored in flash

   Relies on HAL Flash primitives

   Use the very last page of flash memory for user storage (4x256 bytes) 1Kb page

 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32f1xx_hal.h"

#include "UserData.h"

#include "UART_Printf.h"

#define ADDR_FLASH_PAGE_63    ((uint32_t)(0x08000000 + (63*1024)))
#define ADDR_FLASH_PAGE_127   ((uint32_t)(0x08000000 + (127*1024)))

#define ADDR_FLASH_PAGE         ADDR_FLASH_PAGE_127   // Blue pill module with STM32F103C8T6

//#define USER_DATA_Debug UART_printf
//#define USER_DATA_Debug(fmt, ...) void(0)

//#define DEBUG 1

#ifdef DEBUG
  #define USER_DATA_Debug(...) UART_printf(__VA_ARGS__)
#else
  #define USER_DATA_Debug(...) (void)0
#endif


static int PositionInPage;


/* Erase the user Flash area (1 page)*/
static bool UserData_Erase(void){
    uint32_t error;
    bool ok=true;
    FLASH_EraseInitTypeDef flashErase;

    UART_printf( "Erasing %X page\r\n", ADDR_FLASH_PAGE);
    /* Fill EraseInit structure*/
    flashErase.TypeErase   = FLASH_TYPEERASE_PAGES;
    flashErase.PageAddress = ADDR_FLASH_PAGE;
    flashErase.NbPages     = 1;

    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&flashErase, &error) != HAL_OK)
    {
        UART_printf( "HAL_FLASHEx_Erase failed: %X\r\n", error);
        /*Error occurred while page erase.*/
        ok = false;
    }
    HAL_FLASH_Lock();

    return ok;
}

void UserDataDump(UserData_t *pData){
    if ( pData->User1[0] != 0xFF ){
        UART_printf( "User1=%s\r\n", (char*)pData->User1 );
    }
    if ( pData->User2[0] != 0xFF ){
        UART_printf( "User2=%s\r\n", (char*)pData->User2 );
    }
    if ( pData->User3[0] != 0xFF ){
        UART_printf( "User3=%s\r\n", (char*)pData->User3 );
    }
    if ( pData->User4[0] != 0xFF ){
        UART_printf( "User4=%s\r\n", (char*)pData->User4 );
    }

    UART_printf( "H_Thresholds=%+d ",    (int)pData->H_Thresholds );
    UART_printf( "L_Thresholds=%+d\r\n", (int)pData->L_Thresholds );
}

/*
 * Try to set PositionInPage to the right value (latest valid position)
 * and dump current content.
*/
bool UserDataInit(UserData_t *pData){
    bool rc;
    uint16_t w;

    while( true ){
        w = *((__IO uint16_t*)(ADDR_FLASH_PAGE + PositionInPage));
        if ( w == 0xFFFF ){
            break;
        }
        if ( w == 0x5555 ){
            PositionInPage += 256;
            if ( PositionInPage >= 1024 ){
                UserData_Erase();
                PositionInPage = 0;
                break;
            }
        }
        else {
            UserData_Erase();
            PositionInPage = 0;
        }
    }
    USER_DATA_Debug( "PositionInPage is %d\r\n", PositionInPage);

    rc = UserData_get( pData );
    if ( rc ){
        UserDataDump( pData );
    }

    return rc;
}


/*
 * Retrieve struture content at current position
 * Check CRC to return true or false
*/
bool UserData_get( UserData_t *pData ){
    uint16_t * pFlashData = (uint16_t *)pData;
    uint32_t fl_add = (ADDR_FLASH_PAGE + PositionInPage);
    int Len;

    Len = sizeof(UserData_t);

    for (uint32_t i = 0; i < Len ; i+=2)
    {
        *pFlashData = *((__IO uint16_t*)(i + fl_add));
        USER_DATA_Debug( "%04X", *pFlashData );
        pFlashData++;
    }
    USER_DATA_Debug( "\r\n");

    uint16_t crc = ModBusCRC16( (uint8_t *)pData, Len-2 );
    USER_DATA_Debug( "crc = %04X/%04X\r\n", crc, (uint16_t)pData->crc16);
    USER_DATA_Debug( "Magic = %04X User %c %c \r\n", (uint16_t)pData->magic, pData->User1[0], pData->User1[1]);

    return (crc == (uint16_t)pData->crc16);
}



bool UserData_set( UserData_t *pData ){
    HAL_StatusTypeDef rc;
    uint8_t * pFlashData = (uint8_t *)pData;
    uint32_t fl_add = (ADDR_FLASH_PAGE + PositionInPage);
    int Len;
    bool content_ok;
    uint16_t d_w, d_r;

    Len = sizeof(UserData_t);

    uint16_t crc = ModBusCRC16( pFlashData, Len-2 );
    pData->crc16 = crc;

    USER_DATA_Debug( "crc = %04X/%04X\r\n", crc, (uint16_t)pData->crc16);

    // Verify current content
    content_ok = true;
    for (uint32_t i = 0; i < Len ; i+=2)
    {
        d_w = (uint16_t)(pFlashData[i] | (pFlashData[i+1] << 8));
        d_r = *((__IO uint16_t*)(i + fl_add));
        if ( d_r != d_w ){
            content_ok = false;
            break;
        }
    }
    if ( content_ok  ){
        UART_printf( "Content Ok\r\n" );
        return true;
    }
    UART_printf( "Updating data at %d\r\n", PositionInPage);
    if ( d_r != 0xFFFF ){
        // Make current position invalid
        HAL_FLASH_Unlock();
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, fl_add, (uint64_t)0x5555);  //Write magic
        HAL_FLASH_Lock();
        PositionInPage += 256;
        if ( PositionInPage >= 1024 ){
            PositionInPage = 0;
            UserData_Erase();
        }
        fl_add = (ADDR_FLASH_PAGE + PositionInPage);
    }

    HAL_FLASH_Unlock();
    for (uint32_t i = 0; i < Len ; i+=2)
    {
        d_w = (uint16_t)(pFlashData[i] | (pFlashData[i+1] << 8));
        d_r = *((__IO uint16_t*)(i + fl_add));
        rc = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, i + fl_add, (uint64_t)d_w);
        d_r = *((__IO uint16_t*)(i + fl_add));
        USER_DATA_Debug( "%04X/%04X ", d_w, d_r );
    }
    HAL_FLASH_Lock();

    USER_DATA_Debug( "fl_write : %d\r\n", rc );

    return (rc == HAL_OK);
}




