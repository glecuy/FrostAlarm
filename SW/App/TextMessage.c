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
#include "Temperature.h"

#include "UART_Printf.h"

#define KEY_SECRET ":Secret"

#define KEY_STATUS ":Statut"
#define KEY_CONFIG ":Config"

#define LINE_MAX_LEN 40  // Large enough to get phone number

//#define DEBUG 1

#ifdef DEBUG
  #define TEXT_Debug(...) UART_printf(__VA_ARGS__)
#else
  #define TEXT_Debug(...) (void)0
#endif

typedef enum{
    TEXT_CONFIG,
    TEXT_STATUS,
    TEXT_ACK,
    TEXT_RESTART,
    TEXT_OK,
    TEXT_INVALID,
    TEXT_ERROR,
}  TEXT_ID_e;


extern UserData_t PermanentData;

char SenderAddr[16];

// Last message time stamp
uint32_t LastMessageTS;
#define MIN_MESSAGE_INTERVAL (5*60)    // 5 minutes


char * BuffGetLine( char * line, char * buff ){
    char * ptSrc, * ptDst;

    ptSrc = buff;
    ptDst = line;
    while( *ptSrc != '\n' ){
        if ( *ptSrc == '\r' ){
            ptSrc++;
            continue;
        }
        if ( ptDst < (line+LINE_MAX_LEN-1) ){
            *ptDst = *ptSrc;
            ptDst++;
        }
        if ( *ptSrc == '\0' ){
            return NULL;
        }
        ptSrc++;
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


TEXT_ID_e TextMessageParse( char * text, UserData_t * userData ){
    char line[LINE_MAX_LEN];
    char * ptText = text;
    ptText = BuffGetLine( line, ptText );
    TEXT_Debug("(%d)%s\n", (int)strlen(line), line );

    if ( strcmp (line, KEY_SECRET) != 0 ){
        UART_printf( "Wrong secret(%s). text rejected.\r\n", line);
        return TEXT_INVALID;
    }

    ptText = BuffGetLine( line, ptText );
    TEXT_Debug("%s\r\n", line );

    if ( strcasecmp (line, KEY_CONFIG) == 0 ){
        // Config requested, parse parameters
        while (ptText != NULL ){
            ptText = BuffGetLine( line, ptText );
            TEXT_Debug("(%d)%s\n", (int)strlen(line), line );
            ReadLineParam( line, userData );
        }
        return TEXT_CONFIG;
    }

    return TEXT_OK;
}


char * TextMessageHeaderParse( char * text ){
    char line[LINE_MAX_LEN];
    char * ptText = text;
    ptText = BuffGetLine( line, ptText );
    TEXT_Debug("Header:(%d)%s\n", (int)strlen(line), line );

    // Must contain at least 4 '"' characters
    char c, *pdst, *ptr = line;
    int token=0;
    while( (c = *ptr) != '\0' ){
        if ( c== '"' ){
            token++;
        }
        ptr++;
    }
    if ( token >= 4 ){
        ptr = line;
        token=0;

        while( token<3 ){
            if ( *ptr== '"' ){
                token++;
            }
            ptr++;
        }
        pdst = SenderAddr;
        while ( *ptr!= '"' ){
            *pdst++ = *ptr++;
            if ( (pdst - SenderAddr) >= 16-1 ){
                break;
            }
        }
        *pdst = '\0';
    }

    TEXT_Debug("SenderAddr = %s\n", SenderAddr );

    return ptText;
}


bool TextDefaultConfig(void){
    extern unsigned default_cfg_txt_size(void);
    extern const unsigned char default_cfg_txt[];
    TEXT_Debug( "Size of config: %d\r\n", default_cfg_txt_size() );
    memset( &PermanentData, 0xFF, sizeof(UserData_t) );
    return (TEXT_ERROR != TextMessageParse( (char*)default_cfg_txt, &PermanentData ) );
}

bool IsValidUser( char * user ){

    if ( strncmp (PermanentData.User1, user, USER_DATA_PHONE_NUMBER_LEN ) == 0 ){
        return true;
    }
    if ( strncmp (PermanentData.User2, user, USER_DATA_PHONE_NUMBER_LEN ) == 0 ){
        return true;
    }
    if ( strncmp (PermanentData.User3, user, USER_DATA_PHONE_NUMBER_LEN ) == 0 ){
        return true;
    }
    if ( strncmp (PermanentData.User4, user, USER_DATA_PHONE_NUMBER_LEN ) == 0 ){
        return true;
    }

    return false;
}


bool TextSendStatusMessage(char *pDest, bool isConfig, char * title){
    char sA[8], sB[8];

    uint32_t ts = HAL_GetTick()/1000;

    //if ( ts < (LastMessageTS + MIN_MESSAGE_INTERVAL) ){
    //    UART_printf( "Message not sent (%u/%u)!\r\n", ts, LastMessageTS );
    //    return false;
    //}

    UART_printf( "Sending message \"%s\" to %s\r\n", title, pDest );

    SIM_StartMessage( pDest );
    HAL_Delay(200);

    SIM_WriteText_f(":%s\n", title);
    HAL_Delay(200);

    if ( ! isConfig ){
        //SIM_WriteText_f("T1:%d T2:%d\n", T1, T2 );
        SIM_WriteText_f("T1=%s T2=%s\n", formatTemp(Temp_GetT1(), sA), formatTemp(Temp_GetT2(), sB) );
        HAL_Delay(100);

        SIM_WriteText_f("Min=%s Max=%s\n", formatTemp(Temp_HistoryGetMin(), sA), formatTemp(Temp_HistoryGetMax(), sB) );
        HAL_Delay(100);
    }

    if ( PermanentData.User1[0] == '+' ){
        SIM_WriteText_f("U1:%s\n", PermanentData.User1 );
        HAL_Delay(200);
    }
    if ( PermanentData.User2[0] == '+' ){
        SIM_WriteText_f("U2:%s\n", PermanentData.User2 );
        HAL_Delay(200);
    }
    if ( PermanentData.User3[0] == '+' ){
        SIM_WriteText_f("U3:%s\n", PermanentData.User3 );
        HAL_Delay(200);
    }
    if ( PermanentData.User4[0] == '+' ){
        SIM_WriteText_f("U4:%s\n", PermanentData.User4 );
        HAL_Delay(200);
    }

    //
    SIM_WriteText_f("SH:%s\n", formatTemp(PermanentData.H_Thresholds, sA) );
    HAL_Delay(200);

    SIM_WriteText_f("SB:%s\n", formatTemp(PermanentData.L_Thresholds, sA) );
    HAL_Delay(200);

    if ( ! isConfig ){
        SIM_WriteText_f("SQ:%d\n", SIM_GetSignalQuality() );
        HAL_Delay(200);
    }

    // Send Case internal temperature
    SIM_WriteText_f("TCase=%s\n", formatTemp(Temp_GetT_Case(), sA) );
    HAL_Delay(100);

    // Current time stamp
    SIM_WriteText_f("TS:%07u\n", ts );
    HAL_Delay(200);

    SIM_WriteEndOfMessage();

    LastMessageTS = ts;
    return true;
}



/*
 * Process message and return true if message is considered
 * as an acknowlegment.
 *
 ***************************************************/
bool TextIncomingMessageProcess(char * pMess){
    char * ptText = pMess;
    bool ack=false;
    TEXT_ID_e rc;

    ptText = TextMessageHeaderParse( ptText );

    TEXT_Debug( "\n\nMessage from %s :\r\n", SenderAddr );

    rc = TextMessageParse( ptText, &PermanentData );

    if ( rc == TEXT_INVALID ){
        return false;
    }

    if ( IsValidUser(SenderAddr) ){
        ack = true;
    }

    if ( ack && rc == TEXT_CONFIG ){
        TEXT_Debug( "New config required\r\n" );
        // Store new Config
        UserData_set( &PermanentData );

        TextSendStatusMessage( SenderAddr, true, "Config");
        return ack;
    }

    if ( rc == TEXT_ERROR ){
        TEXT_Debug( "Text parsing error !\r\n" );
        return ack;
    }

    // Send status by default
    TEXT_Debug( "Status requested\r\n" );
    TextSendStatusMessage( SenderAddr, false, "Temperature");

    return ack;
}

// SMS sent at boot time
bool TextSendInitialMessage(void){
    return TextSendStatusMessage( PermanentData.User1, true, "Initial");
}

/*
 * Send Alarm to user
 * Text + Call
 *
 *******************************************************/
bool TextSendAlarm(TEMP_TH_e status, int userNo ){
    char * pTitle;
    char * user =  NULL;

    if ( status == TEMP_HIGH ){
        pTitle = "Alarme Haute";
    }
    else if ( status == TEMP_LOW ){
        pTitle = "Alarme Basse";
    }
    else if ( status == TEMP_ERROR ){
        pTitle = "Systeme en PANNE";
    }
    else{
        return false;
    }

    if ( userNo==0 && PermanentData.User1[0] == '+' ){
        user = PermanentData.User1;
    }
    if ( userNo==1 && PermanentData.User2[0] == '+' ){
        user = PermanentData.User2;
    }
    if ( userNo==2 && PermanentData.User3[0] == '+' ){
        user = PermanentData.User3;
    }
    if ( userNo==3 && PermanentData.User4[0] == '+' ){
        user = PermanentData.User4;
    }

    if ( user != NULL ){
        TextSendStatusMessage( user, false, pTitle);
        // Wait a while
        HAL_Delay(5000);
        MakeAnAlarmCall(user);
    }

    return ( user != NULL );
}
