/**
 \file
Файл содержит функции для работы с уровнями доступа.
*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "Params.h"
#include "../mdRTC/mdRTC.h"
#include "../mdLIS200/mdLis200.h"


extern const unsigned short CRCInit;

uint8_t accessStatus = 0xff;
unsigned char l_Value[64];

unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);

/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция проверяет имеет ли текущий пользователь работать с данным параметром
*/
unsigned char CheckLock(unsigned char pRights, unsigned char pRW)
{
    unsigned char access;

    if(accessStatus & 0xf)
    {
        //для поверителя и производителя доступны все возможности
        return 0;
    }

    switch (pRW)
    {
        case Acc_Read:	//request for Read
            access = accessStatus & 0xAA; //Mask write access
            pRights &= 0xAA;
            break;

        case Acc_Write:	//request for Write
            access = accessStatus & 0x55; //Mask read access
            pRights &= 0x55;
            break;
            //		else
            //			return 10;		//TODO -> define error code for this case
    }

    if (access & pRights)
    {
        return 0;    //access accepted
    }
    else	// access denied
    {
        if (pRights & 0xC0)
        {
            return EC_18;    //EC_Lock3;	//customer lock closed
        }

        if (pRights & 0x30)
        {
            return EC_18;    //EC_Lock2;	//supplier lock closed
        }

        if (pRights & 0x0C)
        {
            return EC_18;    //EC_Lock1;	//manufacture lock closed
        }

        if (pRights & 0x03)
        {
            return EC_18;    //EC_Lock0;	//calibration lock closed
        }
    }

    //return 10; //TODO -> define error code for this case
    return EC_20;                                                           // Action is prohibited
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция запрещает доступ (закрывает замок) в зависимости от Instans
*/
unsigned int stopAccess(unsigned indx, unsigned char *p_Buff)
{
    switch (DataTable[indx].Instans)
    {
        //verifier
        case 1 :
            accessStatus &= ~0x3;
            resetEvent(EV_LOCK_CALIB_OPEN);
            break;

        //manufacture
        case 2 :
            accessStatus &= ~0xC;
            resetEvent(EV_LOCK_MANUF_OPEN);
            break;

        //supplier
        case 3 :
            accessStatus &= ~0x30;
            resetEvent(EV_LOCK_SUPL_OPEN);
            break;

        //user
        case 4 :
            accessStatus &= ~0xC0;
            resetEvent(EV_LOCK_CONS_OPEN);
            break;
    }

    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция открывает замок или меняет пароль, если замок уже открыт,
только для поставщика и потребителя
*/
unsigned int openAccessChangePass(unsigned indx, unsigned char *p_Buff)
{
    errno = 0;
    uint32_t value = (uint32_t)strtol((const char *)p_Buff, 0, 0);

    if(!errno)
    {
        switch (DataTable[indx].Instans)
        {
            //supplier
            case 3 :

                //открыт замок поставщика
                if(accessStatus & 0x30)
                {
                    *((uint32_t *)DataTable[indx].AC_Value) = value;
                }
                else
                {
                    accessStatus |= 0x30;
                    setEvent(EV_LOCK_SUPL_OPEN);
                }

                break;

            //user
            case 4 :

                //открыт замок потребителя
                if(accessStatus & 0xC0)
                {
                    *((uint32_t *)DataTable[indx].AC_Value) = value;
                }
                else
                {
                    accessStatus |= 0xC0;
                    setEvent(EV_LOCK_CONS_OPEN);
                }

                break;

            default:
                //сюда мы не должны заходить
                return EC_WrongValueRange;
                break;
        }

        return 0;
    }
    else
    {
        return EC_WrongValueRange;
    }
}
/**
\ingroup mdParams
\author Shuvalov Sergey
\brief Функция возвращает статус замка
*/
void RdAccessStatus (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;

    switch (DataTable[indx].Instans)
    {
        //verifier
        case 1 :
            k = sprintf((char *) p_Buff, "(%d)\x03", (bool)(accessStatus & 0x3));
            break;

        //manufacture
        case 2 :
            k = sprintf((char *) p_Buff, "(%d)\x03", (bool)(accessStatus & 0xC));
            break;

        //supplier
        case 3 :
            k = sprintf((char *) p_Buff, "(%d)\x03", (bool)(accessStatus & 0x30));
            break;

        //user
        case 4 :
            k = sprintf((char *) p_Buff, "(%d)\x03", (bool)(accessStatus & 0xC0));
            break;
    }

    p_Buff+= k;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает статус замка перепрограммирования
*/
void RdAccessProgUpdateStatus (unsigned indx, unsigned char *p_Buff, void *value)   
{
  
    unsigned int k = sprintf((char *) p_Buff, "(%d)\x03", (bool)(params.LockPr));
    p_Buff += k;
}
/**
\ingroup mdParams
\author Shuvalov Sergey
\brief Функция возвращает статус замка
*/
void RdPassw (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;
    bool access = false;

    switch (DataTable[indx].Instans)
    {
        //verifier
        case 1 :

        //manufacture
        case 2 :
            break;

        //supplier
        case 3 :

            //открыт замок поставщика или выше по допуску
            if(accessStatus & 0x3F)
            {
                access = true;
            }

            break;

        //user
        case 4 :

            //открыт любой замок
            if(accessStatus & 0xFF)
            {
                access = true;
            }
    }

    if(access)
    {
        k = sprintf((char *) p_Buff, "(%d)\x03", *((uint32_t *)value));
    }
    else
    {
        k = sprintf((char *) p_Buff, "(#%#04d)\x03", EC_18);
    }

    p_Buff+= k;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция открывает замок производителя динамическим паролем
(в зависимости от даты и номера счётчика)
*/
unsigned openAccessManuf (unsigned i, unsigned char *p_Buff)
{
    unsigned k = 0;
    unsigned int StringLength =
        10;                                               // Set the length of strings used in function
    unsigned long
    PasswEntered;                                                   // Entered password, unsigned short
    unsigned long Passw;                                                          // Calculated password
    unsigned long
    CRC_serial;                                                     // CRC16 of the Serial string
    unsigned long
    CRC_result;                                                     // CRC16 of the XOR'ed string
    k = sscanf((const char *)p_Buff, "%lu",
               &PasswEntered);                       // Get entered value of unsigned  long

    if ((k == 1)&&
            (PasswEntered <=
             99999999))                                     // Check that it is <= 8 symbols (99999999)
    {
        // Scanned as unsigned long and less than 100 000 000
        memset(l_Value, 0x00, sizeof(l_Value));                         // Clear external temp buffer
        // Get current date DD.MM.YYYY
        struct tm dateTime = {0};
        getRtcDateTime(&dateTime);
        sprintf((char *)l_Value, "%02u.%02u.%4u", dateTime.tm_mday, dateTime.tm_mon + 1, dateTime.tm_year + 1900);
        CRC_result = GetCRC16(l_Value, StringLength,
                              CRCInit);                  // Get CRC for date[10] string
        // Get the length of the serial, maximum 12 symbols
        unsigned int Serial_MaxLen;
        unsigned int Serial_CurrentLen;
        Serial_MaxLen = sizeof(
                            params.identDevice);                                    // Get max length of the R_DvcSerial
        Serial_CurrentLen = strlen((char *)
                                   params.identDevice);                        // Get current length of the R_DvcSerial,

        if (Serial_CurrentLen >
                Serial_MaxLen)                                  // If current length = max length, strlen get write wrong length (no 0x00 in the end)
        {
            StringLength = Serial_MaxLen;
        }
        else
        {
            StringLength = Serial_CurrentLen;
        }

        CRC_serial = GetCRC16((unsigned char *)params.identDevice, StringLength,
                              CRCInit); // Get CRC for serial string
        Passw = (unsigned long)CRC_serial * (unsigned long)
                CRC_result;          // Get unsigned long password
        Passw -= Passw/(unsigned long)100000000 * (unsigned long)
                 100000000;     // Get the last 8 symbols of the password

        if (Passw == PasswEntered)                                              // Password is correct
        {
            accessStatus |= 0x0C;                                                // Open Manufacturer lock
            setEvent(EV_LOCK_MANUF_OPEN);
            setEvent(EV_LOCK_CALIB_OPEN);
            return 0;                                                             // Result is OK
        }
        else                                                                    // Password is not correct
        {
            k = accessStatus &
                0x04;                                             // Check current state of the lock

            if (k)                                                                // Lock is opened
            {
                // Wanted to change the lock
                return EC_20;                                                       // Action is prohibited
            }
        }
    }

    return EC_17;                                                                 // Wrong value entered
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция проверяет введеный пользователем пароль для перепрограммирования
*/
unsigned int WritePassProgUpdate(unsigned i, unsigned char *p_Buff)
{
    unsigned k = 0;
    unsigned int StringLength = 10;                                               // Set the length of strings used in function
    unsigned long PasswEntered;                                                   // Entered password, unsigned short
    unsigned long Passw;                                                          // Calculated password
    unsigned long CRC_serial;                                                     // CRC16 of the Serial string
    unsigned long CRC_result;                                                     // CRC16 of the XOR'ed string
    unsigned short CRCInit = 0xABCD;
    
    k = sscanf((const char *)p_Buff, "%lu", &PasswEntered);                       // Get entered value of unsigned  long
    
    if ((k == 1)&& (PasswEntered <= 99999999))                                     // Check that it is <= 8 symbols (99999999)
    {
        // Scanned as unsigned long and less than 100 000 000
        memset(l_Value, 0x00, sizeof(l_Value));                         // Clear external temp buffer
        
        sprintf((char *)l_Value, "%02u.%02u.%4u", 2, 10, 1869);
        
        CRC_result = GetCRC16(l_Value, StringLength, CRCInit);                  // Get CRC for date[10] string
        // Get the length of the serial, maximum 12 symbols
        unsigned int Serial_MaxLen;
        unsigned int Serial_CurrentLen;
        Serial_MaxLen = sizeof(params.identDevice);                                    // Get max length of the R_DvcSerial
        Serial_CurrentLen = strlen((char *)params.identDevice);                        // Get current length of the R_DvcSerial,

        if (Serial_CurrentLen > Serial_MaxLen)                                  // If current length = max length, strlen get write wrong length (no 0x00 in the end)
        {
            StringLength = Serial_MaxLen;
        }
        else
        {
            StringLength = Serial_CurrentLen;
        }

        CRC_serial = GetCRC16((unsigned char *)params.identDevice, StringLength, CRCInit); // Get CRC for serial string
        Passw = (unsigned long)CRC_serial * (unsigned long) CRC_result;          // Get unsigned long password
        Passw -= Passw/(unsigned long)100000000 * (unsigned long) 100000000;     // Get the last 8 symbols of the password

        if (Passw == PasswEntered)                                              // Password is correct
        {
            params.LockPr = 1;
            return 0;                                                             // Result is OK
        }
        else                                                                    // Password is not correct
        {
            k = params.LockPr & 1;                                             // Check current state of the lock

            if (k)                                                                // Lock is opened
            {
                // Wanted to change the lock
                return EC_20;                                                       // Action is prohibited
            }
        }
    }

    return EC_17;                                                                 // Wrong value entered
}