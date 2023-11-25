/* App/UserData.h

  Permanent user data stored in flash via ee (eeprom emulation)

 */

#ifndef _USER_DATA_H_
#define _USER_DATA_H_

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USER_DATA_PHONE_NUMBER_LEN 32
#define USER_DATA_PHONE_NUMBER_Nb  4

typedef struct UserData_s{
    int16_t  magic;
    char User1[USER_DATA_PHONE_NUMBER_LEN];
    char User2[USER_DATA_PHONE_NUMBER_LEN];
    char User3[USER_DATA_PHONE_NUMBER_LEN];
    char User4[USER_DATA_PHONE_NUMBER_LEN];
    int16_t  H_Thresholds;
    int16_t  L_Thresholds;
    int16_t  crc16;
} UserData_t;


// Re-use modbus crc implemetation
unsigned short ModBusCRC16(unsigned char *buf, int len);


bool UserDataInit( UserData_t *pData );
bool UserData_get( UserData_t *pData );
bool UserData_set( UserData_t *pData );

void UserDataDump(UserData_t *pData);

#ifdef __cplusplus
}
#endif
#endif //_USER_DATA_H_
