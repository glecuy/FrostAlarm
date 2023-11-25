/* TextMessage.c

   SMS Text messages parser


 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32f1xx_hal.h"

#include "UserData.h"
#include "SIM_800L.h"

#include "UART_Printf.h"

#define SECRET ":Secret"
#define LINE_MAX_LEN 32

//#define DEBUG 1

#ifdef DEBUG
  #define TEXT_Debug(...) UART_printf(__VA_ARGS__)
#else
  #define TEXT_Debug(...) (void)0
#endif

extern UserData_t PermanentData;

char * BuffGetLine( char * line, char * buff ){
    char * ptSrc, * ptDst;

    ptSrc = buff;
    ptDst = line;
    while( *ptSrc != '\n' ){
        *ptDst = *ptSrc;
        if ( *ptSrc == '\0' ){
            return NULL;
        }
        ptDst++;
        ptSrc++;
        if ( ptDst >= (line+LINE_MAX_LEN-1) )
            break;
    }
    *ptDst = '\0';

    return ptSrc+1;
}


bool GetLineData( char * pData, char * userData ){

        if ( *pData != '+' ){
            memset( userData, 0xFF, USER_DATA_PHONE_NUMBER_LEN );
        }
        else{
            TEXT_Debug("Un:%s\n", pData );
            strncpy(userData, pData, USER_DATA_PHONE_NUMBER_LEN);
        }

    return true;
}

bool ReadLineParam( char * line, UserData_t * userData ){
    int16_t w;
    if ( line[0] == 'U' ){
        if ( line[1] == '1' ){
            GetLineData( line+3, userData->User1 );
        }
        else if ( line[1] == '2' ){
            GetLineData( line+3, userData->User2 );
        }
        else if ( line[1] == '3' ){
           GetLineData( line+3, userData->User3 );
        }
        else if ( line[1] == '4' ){
            GetLineData( line+3, userData->User4 );
        }
    }
    if ( line[0] == 'S' ){
        if ( line[1] == 'H' ){
            TEXT_Debug("SH:%s\r\n", line+3 );
            w = (int16_t)(10*atof(line+3));
            userData->H_Thresholds = w;
        }
        else if ( line[1] == 'B' ){
            TEXT_Debug("SB:%s\r\n", line+3 );
            w = (int16_t)(10*atof(line+3));
            userData->L_Thresholds = w;
        }
    }
    return true;
}


bool TextMessageParse( char * text, UserData_t * userData ){
    char line[LINE_MAX_LEN];
    char * ptText = text;
    ptText = BuffGetLine( line, ptText );
    TEXT_Debug("%s\r\n", line );

    if ( strcmp (line, SECRET) != 0 ){
        UART_printf( "Wrong secret. text rejected.\r\n");
    }

    while (ptText != NULL ){
        ptText = BuffGetLine( line, ptText );
        //TEXT_Debug("%s\n", line );
        ReadLineParam( line, userData );
    }

    return true;
}

bool TextDefaultConfig(void){
    extern unsigned default_cfg_txt_size(void);
    extern const unsigned char default_cfg_txt[];
    TEXT_Debug( "Size of config: %d\r\n", default_cfg_txt_size() );
    memset( &PermanentData, 0xFF, sizeof(UserData_t) );
    return TextMessageParse( (char*)default_cfg_txt, &PermanentData );
}


bool TextSendStatusMessage(void){
    extern int16_t T1, T2;
#if 0
    UART_printf( "Sending message ! \r\n" );

    //SIM_WriteText("AT+CMGS=\"+33687908087\"\r"); // phone number
    SIM_StartMessage2( (char*)PermanentData.User1 );
    //SIM_StartMessage2( "AT+CMGS=\"+33687908087\"\r" ); // User1=+33687908087
    HAL_Delay(200);
    SIM_WriteText("Second: hello how are you?\r"); // message
    HAL_Delay(200);
    SIM_WriteEndOfMessage();  //send a Ctrl+Z (end of the message)

#else

    char ligne[40];
    printf( "TextSendStatusMessage\n" );
    //SIM_StartMessage( (char*)PermanentData.User1 );
    //SIM_StartMessage( "AT+CMGS=\"+33687908087\"\r" );
    SIM_WriteText("AT+CMGS=\"+33687908087\"\r"); // phone number
    HAL_Delay(200);

    snprintf(ligne, 40, "T1:%d T2:%d\n", T1, T2 );
    SIM_WriteText(ligne);
    HAL_Delay(200);

    SIM_WriteText(":xx\n");
    HAL_Delay(200);

    if ( PermanentData.User1[0] == '+' ){
        snprintf(ligne, 40, "U1:%s\n", PermanentData.User1 );
        SIM_WriteText(ligne);
        HAL_Delay(200);
    }
    if ( PermanentData.User2[0] == '+' ){
        snprintf(ligne, 40, "U2:%s\n", PermanentData.User2 );
        SIM_WriteText(ligne);
        HAL_Delay(200);
    }
    if ( PermanentData.User3[0] == '+' ){
        snprintf(ligne, 40, "U3:%s\n", PermanentData.User3 );
        SIM_WriteText(ligne);
        HAL_Delay(200);
    }
    if ( PermanentData.User4[0] == '+' ){
        snprintf(ligne, 40, "U4:%s\n", PermanentData.User4 );
        SIM_WriteText(ligne);
        HAL_Delay(200);
    }

    //
    snprintf(ligne, 40, "SH:%+d.%d\n", PermanentData.H_Thresholds/10, abs(PermanentData.H_Thresholds%10) );
    SIM_WriteText(ligne);
    HAL_Delay(200);

    snprintf(ligne, 40, "SB:%+d.%d\n", PermanentData.L_Thresholds/10, abs(PermanentData.L_Thresholds%10) );
    SIM_WriteText(ligne);
    HAL_Delay(200);

    snprintf(ligne, 40, "Q:%+d\n", SIM_GetSignalQuality() );
    SIM_WriteText(ligne);
    HAL_Delay(200);

    SIM_WriteEndOfMessage();
#endif

    SIM_FlushRxComm("GMGS");

    return true;
}


