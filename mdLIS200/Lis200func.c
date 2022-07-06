/**
 \file
Файл содержит функции поддержки реализации протокола Lis200.
*/

#include <string.h>
#include "mdLis200.h"

#define Err_Telegramma 1
#define EC_AbsentValue 4
#define Err_BCC 13

/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция очищает приёмный буфер.
*/
void clearRXBuf(StrDataUART *data)
{
    memset(data->bufRX, 0, BUF_UART_MAX);
    data->indx = 0;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция очищает буфер передачи данных.
*/
void clearTXBuf(StrDataUART *data)
{
    memset(data->bufTX, 0, BUF_UART_MAX);
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция добавляет принятый байт в приёмный буфер.
*/
void addByteInBufRX(char ch, StrDataUART *data)
{  
    if((data->indx+1) == BUF_UART_MAX)
    {
        clearRXBuf(data);
    }
    //прописываем пришедший символ в строку
    data->bufRX[data->indx] = ch;
    data->indx++;    
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция подсчитывает BCC.
*/
unsigned char countBcc (unsigned char *p_msg)
{
    unsigned char Bcc = 0;

    while (*p_msg)
    {
        Bcc = Bcc^*p_msg;
        p_msg++;
    }

    return Bcc;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция подсчитывает BCC и вставляет в конец строки.
*/
int GetBcc (unsigned char *p_msg)
{
    unsigned char Bcc = 0;
    p_msg++; // пропускаем первый симовл STX или SOH

    while (*p_msg)
    {
        Bcc = Bcc^*p_msg;
        p_msg++;
    }

    *p_msg=Bcc;
    p_msg++;
    *p_msg = 0x00;
    if(Bcc == 0)
        return 1;
    return 0;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция проверяет BCC.
*/
int CheckBcc (unsigned char *p_msg)
{
    unsigned char Bcc = 0;
    int DoBcc = 1;

    while ((*p_msg) && (DoBcc))
    {
        Bcc = Bcc^*p_msg;

        if ((*p_msg == ETX) || (*p_msg == EOT))
        {
            DoBcc = 0;
        }

        p_msg++;
    }

    if (*p_msg == Bcc)
    {
        return 0;
    }
    else
    {
        return Err_BCC;
    }
}
