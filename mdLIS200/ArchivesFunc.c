/**
 \file
Файл содержит функции для чтения архивов по протоколу Lis200.
*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdLis200.h"
#include "../mdFlashMemory/mdFlashMemory.h"
#include "../mdFlashMemory/DataForFlash.h"
#include "../mdOpticalInterface/mdOpticalInterface.h"
#include "../Params/Params.h"

extern int exchangeRegim;
extern StrDataUART optData;
extern const unsigned short CRCInit;
unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);
unsigned char countBcc (unsigned char *p_msg);
void sendDataUsart1(uint8_t *buf, int len);

StrArchive archive;
static uint8_t bufArc[256];
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция определяет количество записей в одном блоке.
  В случае ошибки возвращает 0
*/
int getSumPagesInBlock(char *str, uint16_t *sumPagesBlock)
{
    char *ptrBeg = (char *) strrchr((const char *)str, ';');
    char *ptrEnd = (char *) strrchr((const char *)str, ')');

    if(!(ptrBeg && ptrEnd))
    {
        return 0;
    }

    if((ptrEnd - ptrBeg) == 1)
    {
        return 0;
    }

    *ptrEnd = 0;
    *sumPagesBlock = (int)strtol((const char *)ptrBeg+1, 0, 0);
    *ptrEnd = ')';

    if ((*sumPagesBlock > 999) || (*sumPagesBlock < 1))
    {
        return 0;
    }

    /*// otlad
    if(*sumPagesBlock == 10)
    {
      *sumPagesBlock = 1;
    }
    // otlad end*/
    return 1;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция определяет тип выборки.
  В случае ошибки возвращает 0
*/
int getTypeSelection(char *str, uint8_t *typeSelection)
{
    char *ptrBeg = (char *) strchr((const char *)str, '(');
    char *ptrEnd = (char *) strchr((const char *)str, ';');

    if(!(ptrBeg && ptrEnd))
    {
        return 0;
    }

    if((ptrEnd - ptrBeg) == 1)
    {
        *typeSelection = 1;
        return 1;
    }

    *ptrEnd = 0;
    *typeSelection = (int)strtol((const char *)ptrBeg+1, 0, 0);
    *ptrEnd = ';';

    if ((*typeSelection > 3) || (*typeSelection < 1))
    {
        return 0;
    }

    return 1;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция формирует строку разбирая каждый бит статуса
*/
char *FmtStatus(unsigned pSt, char *p_Buff, int *count)
{
    unsigned char j;
    unsigned lSt;
    short k;
    lSt = pSt;

    if (lSt)
    {
        *p_Buff++ = '(';
        (*count)++;

        for (j = 1; j<=16; j++)                      // old value j<= 8 ;
        {
            if (lSt & 0x0001)
            {
                if (*(p_Buff-1) != '(')
                {
                    *p_Buff++ = ';';
                    (*count)++;
                }

                k = sprintf((char *)p_Buff, "%u", j);
                p_Buff+= k;
                (*count) += k;
            }

            lSt>>= 1;
        }//for (j = 1;8;j++)

        *p_Buff++ = ')';
        (*count)++;
        *p_Buff = 0x00;
    }
    else
    {
        k = sprintf((char *)p_Buff, "(%u)", 0);
        (*count) += k;
        p_Buff = strchr(p_Buff, 0x00);
    }

    return p_Buff;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция определяет атрибут запроса: 0 - значение; 2 - заголовки;
3 - единицы измерения; 10 - количество записей.
  В случае ошибки возвращает 0
*/
int getAttribute(char *str, uint8_t *attribute)
{
    char *ptrBeg = (char *) strchr((const char *)str, '.');
    char *ptrEnd = (char *) strchr((const char *)str, '(');

    if(!(ptrBeg && ptrEnd) || ((ptrEnd - ptrBeg) == 1))
    {
        return 0;
    }

    *ptrEnd = 0;
    ptrBeg++;

    if((*ptrBeg == 'A') || (*ptrBeg == 'a'))
    {
        *attribute = 10;
    }
    else
    {
        *attribute = (int)strtol((const char *)ptrBeg, 0, 0);
    }

    *ptrEnd = '(';
    return 1;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция формирует строку записи интервального архива на выдачу.
*/
void createIntervalArchiveString(char *bufTX, StrIntervalArchive *archive, int *count)
{
    if(archive->count == 0xffffffff)
    {
        memset(archive, 0, sizeof(StrIntervalArchive));
    }

    struct tm *t = localtime(&archive->timeDate);

    // формируем строку
    *count += sprintf(bufTX + *count,
                      "(%u)(%u)(%.2u-%.2u-%.2u,%.2u:%.2u:%.2u)(%0.4f)(%0.4f)(%0.4f)(%0.4f)(%0.2f)(%0.4f)(%0.4f)(%0.4f)(%0.4f)",
                      archive->count, archive->globCount, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,
                      archive->Vc, archive->VcO, archive->Vp, archive->VpO, archive->averageTemper, archive->PpodAvI, archive->KaverI, archive->averageKkor, archive->QcAver
                     );

    bufTX = bufTX + *count;
    bufTX = FmtStatus(archive->sysSt, bufTX, count);
    bufTX = FmtStatus(archive->st1, bufTX, count);
    bufTX = FmtStatus(archive->st2, bufTX, count);
    bufTX = FmtStatus(archive->st3, bufTX, count);
    bufTX = FmtStatus(archive->st4, bufTX, count);
    bufTX = FmtStatus(archive->st5, bufTX, count);
    bufTX = FmtStatus(archive->st6, bufTX, count);
    bufTX = FmtStatus(archive->st7, bufTX, count);
    bufTX = FmtStatus(archive->st8, bufTX, count);
    bufTX = FmtStatus(archive->st9, bufTX, count);    

    *count += sprintf(bufTX, "(0x%.4X)", archive->eventCode);
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция формирует строку записи суточного архива на выдачу.
*/
void createDayArchiveString(char *bufTX, StrDayArchive *archive, int *count)
{
    if(archive->count == 0xffffffff)
    {
        memset(archive, 0, sizeof(StrDayArchive));
    }

    struct tm *t = localtime(&archive->timeDate);

    // формируем строку
    *count += sprintf(bufTX + *count,
                      "(%u)(%u)(%.2u-%.2u-%.2u,%.2u:%.2u:%.2u)(%0.4f)(%0.4f)(%0.4f)(%0.4f)(%0.2f)(%0.4f)(%0.4f)(%0.4f)(%0.4f)",
                      archive->count, archive->globCount, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,
                      archive->Vc, archive->VcO, archive->Vp, archive->VpO, archive->averageTemper, archive->PpodAvI, archive->KaverI, archive->averageKkor, archive->QcAver
                     );

    bufTX = bufTX + *count;
    bufTX = FmtStatus(archive->sysSt, bufTX, count);
    bufTX = FmtStatus(archive->st1, bufTX, count);
    bufTX = FmtStatus(archive->st2, bufTX, count);
    bufTX = FmtStatus(archive->st3, bufTX, count);
    bufTX = FmtStatus(archive->st4, bufTX, count);
    bufTX = FmtStatus(archive->st5, bufTX, count);
    bufTX = FmtStatus(archive->st6, bufTX, count);
    bufTX = FmtStatus(archive->st7, bufTX, count);
    bufTX = FmtStatus(archive->st8, bufTX, count);
    bufTX = FmtStatus(archive->st9, bufTX, count);    

    *count += sprintf(bufTX, "(0x%.4X)", archive->eventCode);
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция формирует строку записи месячного архива на выдачу.
*/
void createMonthArchiveString(char *bufTX, StrMonthArchive *archive, int *count)
{
    if(archive->count == 0xffffffff)
    {
        memset(archive, 0, sizeof(StrMonthArchive));
    }

    struct tm *t = localtime(&archive->timeDate);

    // формируем строку
    *count += sprintf(bufTX + *count,
                      "(%u)(%u)(%.2u-%.2u-%.2u,%.2u:%.2u:%.2u)(%0.4f)(%0.4f)(%0.4f)(%0.4f)(%0.2f)(%0.4f)(%0.4f)(%0.4f)(%0.4f)",
                      archive->count, archive->globCount, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,
                      archive->Vc, archive->VcO, archive->Vp, archive->VpO, archive->averageTemper, archive->PpodAvI, archive->KaverI, archive->averageKkor, archive->QcAver
                     );

    bufTX = bufTX + *count;
    bufTX = FmtStatus(archive->sysSt, bufTX, count);
    bufTX = FmtStatus(archive->st1, bufTX, count);
    bufTX = FmtStatus(archive->st2, bufTX, count);
    bufTX = FmtStatus(archive->st3, bufTX, count);
    bufTX = FmtStatus(archive->st4, bufTX, count);
    bufTX = FmtStatus(archive->st5, bufTX, count);
    bufTX = FmtStatus(archive->st6, bufTX, count);
    bufTX = FmtStatus(archive->st7, bufTX, count);
    bufTX = FmtStatus(archive->st8, bufTX, count);
    bufTX = FmtStatus(archive->st9, bufTX, count);    

    *count += sprintf(bufTX, "(0x%.4X)", archive->eventCode);
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция формирует архив изменений на выдачу.
*/
void createChangesArchiveString(char *bufTX, StrChangesArchive *archive, int *count)
{
    if(archive->count == 0xffffffff)
    {
        memset(archive, 0, sizeof(StrChangesArchive));
    }

    struct tm *t = localtime(&archive->timeDate);

    // формируем строку
    *count += sprintf(bufTX + *count,
                      "(%u)(%u)(%.2u-%.2u-%.2u,%.2u:%.2u:%.2u)",
                      archive->count, archive->globCount, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec
                     );
    *count += sprintf(bufTX + *count,
                      "(%s)(%s)(%s)(%u)(%u)(%u)",
                      (char*)archive->paramCode, (char*)archive->oldParamValue, (char*)archive->newParamValue,
                       archive->stCalibLock, archive->stSupplierLock, archive->stConsumerLock   
                     );
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция формирует архив событий на выдачу.
*/
void createEventsArchiveString(char *bufTX, StrEventsArchive *archive, int *count)
{
    if(archive->count == 0xffffffff)
    {
        memset(archive, 0, sizeof(StrEventsArchive));
    }

    struct tm *t = localtime(&archive->timeDate);

    // формируем строку
    *count += sprintf(bufTX + *count,
                      "(%u)(%u)(%.2u-%.2u-%.2u,%.2u:%.2u:%.2u)",
                      archive->count, archive->globCount, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec
                     );
    *count += sprintf(bufTX + *count, "(0x%.4X)", archive->eventCode);
    *count += sprintf(bufTX + *count,
                      "(%0.4f)(%0.4f)(%0.4f)(%0.4f)(%0.4f)(%0.4f)(%0.4f)(%0.2f)(%0.4f)",
                      archive->Vc, archive->VcO, archive->Vp, archive->VpO, archive->PpodAvI, archive->KaverI, 
                      archive->averageKkor, archive->T, archive->Q
                     );
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция формирует интервальный архив.
*/
void ReadArchive(unsigned char *bufTX)
{
    uint32_t adress = 0;
    int oldPages = 0; //уже выданные записи - используется при переполнении
    int count = 0;
    //vTaskDelay(100);
    while(archive.curPage < archive.sumPages)
    {
        count = 0;
        adress = archive.adrBegin + archive.curPage*256;

        //если переполнение новые данные в начале адресного пространства флеш
        if(adress > archive.adressFlashEnd)
        {
            oldPages = (archive.adressFlashEnd + 1 - archive.adrBegin) >> 8;
            adress = archive.adressFlashBegin + (archive.curPage - oldPages) *256;
        }

        readDataFromFlash(bufArc, archive.size, adress);

        archive.curRecInBlock++;

        if(archive.curPage % archive.sumPagesBlock == 0)
        {
            bufTX[count] = STX;
            count++;
        }

        archive.curPage++;

        switch(archive.num)
        {
            //интервальный архив
            case 3:
            createIntervalArchiveString((char *)bufTX, (StrIntervalArchive *)bufArc, &count);
            break;    
            
            //суточный архив
            case 7:
            createDayArchiveString((char *)bufTX, (StrDayArchive *)bufArc, &count);
            break;
            
            //месячный архив
            case 1:
            createMonthArchiveString((char *)bufTX, (StrMonthArchive *)bufArc, &count);
            break;
            
            //архив событий
            case 4:
            createEventsArchiveString((char *)bufTX, (StrEventsArchive *)bufArc, &count);
            break;
            
            //архив изменений
            case 5:
            createChangesArchiveString((char *)bufTX, (StrChangesArchive *)bufArc, &count);
            break;
        }

        // проверка CRC записи
        uint16_t crc = 0;
        crc = GetCRC16(bufArc, archive.sizeCRC, CRCInit);

        if(crc == *((uint16_t *)(bufArc + archive.sizeCRC)))
        {
            count += sprintf((char *)bufTX + count, "(CRC Ok)\x00");
        }
        else
        {
            if(params.sendErrLis200)
            {
                count += sprintf((char *)bufTX + count, "(CRC Error)\x00");
            }
            else 
            {
                if (archive.sumPages > 0)                                       // Decrement whole number of pages
                {
                    archive.sumPages--;
                }
                
                if (archive.curPage > 0)                                        // Decrement number os sent pages, as this page was not snt
                {
                    archive.curPage--;
                }                
                continue;
            }
        }

        // последняя запись из диапазона (вставить ETX)
        if(archive.curPage == archive.sumPages)
        {
            bufTX[count] = ETX;
            count++;
            archive.bccBlock ^= countBcc (bufTX);
            bufTX[count] = archive.bccBlock;
            optData.BCCisZERO = !(bool)bufTX[count];
        }
        // последняя страница в блоке (вставить EOT)
        else if(archive.curPage % archive.sumPagesBlock == 0)
        {
            bufTX[count] = EOT;
            count++;
            archive.bccBlock ^= countBcc (bufTX);
            bufTX[count] = archive.bccBlock;
            optData.BCCisZERO = !(bool)bufTX[count];
        }
        else
        {
            bufTX[count] = 0x0D;
            bufTX[count+1] = 0x0A;
            count += 2;
        }

        archive.bccBlock ^= countBcc (bufTX);
        //sendDataUsart1((uint8_t *)bufTX, strlen((char const *)bufTX));
        sendDataWithOpticalInterface();
        memset(bufTX, 0, BUF_UART_MAX);

        //выдали блок - ждём АСК
        if(archive.curPage % archive.sumPagesBlock == 0)
        {
            break;
        }
    }
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция находит адреса мин. и макс. значений из диапазона в запросе.
Алгоритм с использованием shift и size только для little endian систем.
*/
int GetRecSum(StrArchive *archive, time_t beg, time_t end)
{
    /*
        у всех архивов первые три поля структуры одинаковые
        struct {
        uint32_t count;
        uint32_t globCount;
        time_t timeDate;
    */
    int shift;
    int size;
    const uint32_t IMPOSSIBLE_ADRESS = 0xffffffff; // flash 0x7ffffff - 0

    switch(archive->typeSelection)
    {
        // выборка по uint32_t count;
        case 1:
            shift = 0;
            size = sizeof(uint32_t);
            break;

        // выборка по uint32_t globCount;
        case 2:
            shift = sizeof(uint32_t);
            size = sizeof(uint32_t);
            break;

        // выборка по time_t timeDate;
        case 3:
            shift = sizeof(uint32_t) * 2;
            size = sizeof(time_t);
            break;

        default:
            return 0;
    }

    time_t value = 0xffffffffffffffff;
    uint8_t buf[16];
    uint32_t countMin = IMPOSSIBLE_ADRESS;
    uint32_t countMax = 0;
    uint32_t adrBlockMin = IMPOSSIBLE_ADRESS; //адрес блока в котором нужно искать минимальное значение
    uint32_t adrBlockMax = IMPOSSIBLE_ADRESS; //адрес блока в котором нужно искать максимальное значение
    archive->curPage = 0;

    // обходим флеш блоками по 4 кБ
    for(uint32_t adressB = archive->adressFlashBegin; adressB < archive->adressFlashEnd; adressB += 0x1000)
    {
        readDataFromFlash(buf, 16, adressB);
        // инициализируем значение выборки это время, или номер записи (лок.\глоб.)
        memcpy(&value, &buf[shift], size);
        uint32_t count = *((uint32_t*)&buf);
        
        // пустой блок
        if(value == 0xffffffffffffffff)
        {
            continue;
        }

        // проверка, что значение в диапазоне
        if((value >= beg) && ((value <= end) || (!end)))
        {
            // в этом блоке значение ещё ниже
            if(count < countMin)
            {
                countMin = count;
                adrBlockMin = adressB;
            }
            // в этом блоке значение ещё больше
            if(count > countMax)
            {
                countMax = count;
                adrBlockMax = adressB;
            }
        }
    }

    //находим адрес с наименьшим значением  в найденом блоке (поиск по страницам)
    archive->adrBegin = IMPOSSIBLE_ADRESS;

    if(adrBlockMin != IMPOSSIBLE_ADRESS)
    {
        // проверим предыдущий блок - может в конце того блока значение было меньше
        if(adrBlockMin)
        {
            adrBlockMin -= 0x1000;
        }

        // возможно было переполнение и найдено наименьшее значение в первом блоке
        // поэтому будем работать с последним блоком из диапазона соответствующего архива
        if((adrBlockMin < archive->adressFlashBegin) || (!adrBlockMin))
        {
            adrBlockMin = archive->adressFlashEnd - 0xFFF;
        }

        for(uint32_t adressP = adrBlockMin; adressP < adrBlockMin + 0x1000; adressP += 0x100)
        {
            
            readDataFromFlash(buf, 16, adressP);
            // инициализируем значение выборки это время, или номер записи (лок.\глоб.)
            memcpy(&value, &buf[shift], size);
            uint32_t count = *((uint32_t*)&buf);
            
            // проверка, что значение в диапазоне
            if((value >= beg) && ((value <= end) || (!end)))
            {
                // в этой записи значение ещё ниже
                if(count <= countMin)
                {
                    countMin = count;
                    archive->adrBegin = adressP;
                }
            }
            
            //если анализировали последний блок тогда пройдем и по первому (переход)
            if(adressP == (archive->adressFlashEnd & 0xffffff00))
            {
                adressP = archive->adressFlashBegin;
                adrBlockMin = archive->adressFlashBegin;
                adressP -= 0x100;  //иначе след. чтение будет не с 0x000, а с 0x0100
            }
        }
    }

    //находим адрес с наибольшим значением  в найденом блоке (поиск по страницам)
    archive->adrEnd = IMPOSSIBLE_ADRESS;

    if(adrBlockMax != IMPOSSIBLE_ADRESS)
    {
        for(uint32_t adressP = adrBlockMax; adressP < adrBlockMax + 0x1000; adressP += 0x100)
        {
            readDataFromFlash(buf, 16, adressP);
            // инициализируем значение выборки это время, или номер записи (лок.\глоб.)
            memcpy(&value, &buf[shift], size);
            uint32_t count = *((uint32_t*)&buf);
            
            // проверка, что значение в диапазоне
            if((value >= beg) && ((value <= end) || (!end)))
            {
                // в этой записи значение ещё больше
                if(count >= countMax)
                {
                    countMax = count;
                    archive->adrEnd = adressP;
                }
            }
        }
    }

    // нет параметров в запрашиваемом диапазоне
    if((archive->adrEnd==IMPOSSIBLE_ADRESS) || (archive->adrBegin == IMPOSSIBLE_ADRESS))
    {
        return 0;
    }

    int sumRec = 0;

    if(archive->adrBegin == archive->adrEnd)
    {
        sumRec = 1;
    }
    else if(archive->adrBegin > archive->adrEnd) // переполнение
    {
        sumRec = ((archive->adressFlashEnd + 1) - archive->adrBegin) >> 8;
        sumRec += ((archive->adrEnd - (archive->adressFlashBegin)) >> 8) + 1;
    }
    else
    {
        sumRec = ((archive->adrEnd - archive->adrBegin) >> 8) + 1;
    }   
    archive->sumPages = sumRec;
    return 1;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция преобразует данные запроса к дате\времени возвращает 1 если нет ошибок
<S0H>R3<STX>[k]:V.[a]([p];[v];[b];[t])<ETX><BCC> str-> [v];[b];[t]
*/
int getTimeFromStringRequest(unsigned char *str, time_t *timeDate)
{
    struct tm temp = {0};
    int k = 0;

    // если параметр отсутствует берем самую старую(для начала)/последнюю(для конца) запись
    if(*str == ';')
    {
        *timeDate = 0;
        return 1;
    }
    else
    {
        k = sscanf((char *) str, "%d-%d-%d,%d:%d:%d", &temp.tm_year, &temp.tm_mon, &temp.tm_mday, &temp.tm_hour, &temp.tm_min, &temp.tm_sec);
        temp.tm_year -= 1900;

        if(k != 6)
        {
            return 0;
        }
        else
        {
            temp.tm_mon -= 1;
            *timeDate = mktime(&temp);

            if(*timeDate == -1)
            {
                return 0;
            }
        }
    }

    return 1;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция преобразует данные запроса к счетчикам возвращает 1 если нет ошибок
<S0H>R3<STX>[k]:V.[a]([p];[v];[b];[t])<ETX><BCC> str-> [v];[b];[t]
*/
int getCountFromStringRequest(unsigned char *str, uint32_t *count)
{
    char *ptr = 0;

    // если параметр отсутствует берем самую старую(для начала)/последнюю(для конца) запись
    if (*str == ';')
    {
        *count = 0;
        return 1;
    }

    ptr = strchr((char const *)str, ';');

    if(!ptr)
    {
        return 0;
    }

    *ptr = 0;
    *count = (uint32_t)strtol((const char *)str, 0, 0);
    *ptr = ';';
    return 1;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция исходя из запроса (номер архива и тип выборки) определяет диапазон адресов
для выборки.
<S0H>R3<STX>[k]:V.[a]([p];[v];[b];[t])<ETX><BCC>    p_Order-> [p];[v];[b];[t]
*/
int GetArchiveRecSum(char *bufTX, unsigned char *p_Order)
{
    time_t beg = 0, end = 0;
    p_Order = (unsigned char *)strchr((char const *)p_Order, ';');

    if (p_Order == NULL)
    {
        return 13;
    }

    // за первым символом ';' начало диапазона
    p_Order++;

    //определяем начало и конец записи
    switch(archive.typeSelection)
    {
        // номер (1- локальный 2- глобальный) записи в архиве
        case 1:
        case 2:

            // определяем начало диапазона
            if(!getCountFromStringRequest(p_Order, (uint32_t *)&beg))
            {
                return 103;
            }

            p_Order = (unsigned char *)strchr((char const *)p_Order, ';');

            if (p_Order == NULL)
            {
                return 13;
            }

            p_Order++;

            // определяем конец диапазона
            if(!getCountFromStringRequest(p_Order, (uint32_t *)&end))
            {
                return 103;
            }

            break;

        // дата время
        case 3:

            // определяем начало диапазона
            if(!getTimeFromStringRequest(p_Order, &beg))
            {
                return 103;
            }

            p_Order = (unsigned char *)strchr((char const *)p_Order, ';');

            if (p_Order == NULL)
            {
                return 13;
            }

            p_Order++;

            // определяем конец диапазона
            if(!getTimeFromStringRequest(p_Order, &end))
            {
                return 103;
            }

            break;
    }

    switch(archive.num)
    {
        //интервальный архив
        case 3:
        archive.adressFlashBegin = intervalArchiveInfo.adressFlashBegin;
        archive.adressFlashEnd = intervalArchiveInfo.adressFlashEnd;
        archive.size = sizeof(StrIntervalArchive);
        archive.sizeCRC = (char *)&intervalArchive.cs - (char *)&intervalArchive;
        break;
        
        //суточный архив
        case 7:
        archive.adressFlashBegin = dayArchiveInfo.adressFlashBegin;
        archive.adressFlashEnd = dayArchiveInfo.adressFlashEnd;
        archive.size = sizeof(StrDayArchive);
        archive.sizeCRC = (char *)&dayArchive.cs - (char *)&dayArchive;
        break;
        
        //месячный архив
        case 1:
        archive.adressFlashBegin = monthArchiveInfo.adressFlashBegin;
        archive.adressFlashEnd = monthArchiveInfo.adressFlashEnd;
        archive.size = sizeof(StrMonthArchive);
        archive.sizeCRC = (char *)&monthArchive.cs - (char *)&monthArchive;
        break;
        
        //архив событий
        case 4:        
        archive.adressFlashBegin = eventsArchiveInfo.adressFlashBegin;
        archive.adressFlashEnd = eventsArchiveInfo.adressFlashEnd;
        archive.size = sizeof(StrEventsArchive);
        archive.sizeCRC = (char *)&eventsArchive.cs - (char *)&eventsArchive;
        break;
        
        //архив изменений
        case 5:
        archive.adressFlashBegin = changesArchiveInfo.adressFlashBegin;
        archive.adressFlashEnd = changesArchiveInfo.adressFlashEnd;
        archive.size = sizeof(StrChangesArchive);
        archive.sizeCRC = (char *)&changesArchive.cs - (char *)&changesArchive;
        break;
    }

    if(!GetRecSum(&archive, beg, end))
    {
        return 103;
    }
    
    sprintf((char *)bufTX, "\x02(%d)\x03\x00", archive.sumPages);

    return 0;
}
/**
 \ingroup mdLis200Group
 \author Shuvalov Sergey
 \brief Функция исходя из запроса (номер архива) формирует строку заголовка.
*/
int GetArchiveHead(char *bufTX, unsigned char Attribute)
{
    // Description
    if(Attribute == 2)
    {
        switch(archive.num)
        {
            //интервальный архив
            case 3:
            sprintf((char *)bufTX, "\x02(AONo)(GONo)(Time)(Vc)(VcO)(Vp)(VpO)(TaverI)(PpodAvI)(KAvI)(KkorAvI)(QcAvI)(SysSt)(St1)(St2)(St3)(St4)(St5)(St6)(St7)(St8)(St9)(EventCode)\x03");                
            break;          
            //суточный архив
            case 7:
            sprintf((char *)bufTX, "\x02(AONo)(GONo)(Time)(Vc)(VcO)(Vp)(VpO)(TaverD)(PpodAvI)(KAvI)(KkorAvD)(QcAvD)(SysSt)(St1)(St2)(St3)(St4)(St5)(St6)(St7)(St8)(St9)(EventCode)\x03");                
            break;
            //месячный архив
            case 1:
            sprintf((char *)bufTX, "\x02(AONo)(GONo)(Time)(Vc)(VcO)(Vp)(VpO)(TaverM)(PpodAvI)(KAvI)(KkorAvM)(QcAvM)(SysSt)(St1)(St2)(St3)(St4)(St5)(St6)(St7)(St8)(St9)(EventCode)\x03");                
            break;
            //архив событий
            case 4:
            sprintf((char *)bufTX, "\x02(AONo)(GONo)(Time)(Vc)(VcO)(Vp)(VpO)(PpodAvI)(KAvI)(KkorAvI)(T)(Q)\x03");                
            break;
            //архив изменений
            case 5:
            sprintf((char *)bufTX, "\x02(AONo)(GONo)(Time)(parCode)(oldVal)(newVal)(CalLock)(SupLock)(ConsLock)\x03");
            break;
        }
    }
    // Source
    else if(Attribute == 4)
    {
        switch(archive.num)
        {
            //интервальный архив
            case 3:
            sprintf((char *)bufTX, "\x02(0003:0A20)(0001:01F8)(0001:0400)(0002:0300)(0002:0302)(0004:0300)(0004:0302)(0008:0313)(0007:0313)(0008:0313)(0005:0313)(0002:0316)(0002:0100)(0001:0110)(0002:0110)(0003:0110)(0004:0110)(0005:0110)(0006:0110)(0007:0110)(0008:0110)(0009:0110)(0007:0A21)\x03");                
            break;          
            //суточный архив
            case 7:
            sprintf((char *)bufTX, "\x02(0007:0A20)(0001:01F8)(0001:0400)(0002:0300)(0002:0302)(0004:0300)(0004:0302)(0008:0314)(0007:0314)(0008:0314)(0005:0314)(0002:0317)(0002:0100)(0001:0110)(0002:0110)(0003:0110)(0004:0110)(0005:0110)(0006:0110)(0007:0110)(0008:0110)(0009:0110)(0007:0A21)\x03");                
            break;
            //месячный архив
            case 1:
            sprintf((char *)bufTX, "\x02(0001:0A20)(0001:01F8)(0001:0400)(0002:0300)(0002:0302)(0004:0300)(0004:0302)(0008:0315)(0007:0315)(0008:0315)(0005:0315)(0002:0318)(0002:0100)(0001:0110)(0002:0110)(0003:0110)(0004:0110)(0005:0110)(0006:0110)(0007:0110)(0008:0110)(0009:0110)(0007:0A21)\x03");                
            break;
            //архив событий
            case 4:
            sprintf((char *)bufTX, "\x02(0004:0A20)(0001:01F8)(0001:0400)(0004:0A22)(0002:0300)(0002:0302)(0004:0300)(0004:0302)(0007:0313)(0008:0313)(0005:0313)(0006:0310)(0002:0310)\x03");                
            break;
            //архив изменений
            case 5:
            sprintf((char *)bufTX, "\x02(0005:0A20)(0001:01F8)(0001:0400)(0005:0810)(0005:0811)(0005:0812)(0001:0170)(0003:0170)(0004:0170)\x03");
            break;
        }
    }
    // единицы измерений
    else if(Attribute == 3)
    {
        switch(archive.num)
        {
            //интервальный архив
            case 3:
            sprintf((char *)bufTX, "\x02()()()(\xEC\x33)(\xEC\x33)(\xEC\x33)(\xEC\x33)(C)(\xCF\xE0)()()(\xEC\x33\\\xF7)()()()()()()()()()()()\x03");
            break;
            //суточный архив
            case 7:
            sprintf((char *)bufTX, "\x02()()()(\xEC\x33)(\xEC\x33)(\xEC\x33)(\xEC\x33)(C)(\xCF\xE0)()()(\xEC\x33\\\xF7)()()()()()()()()()()()\x03");
            break;
            //месячный архив
            case 1:
            sprintf((char *)bufTX, "\x02()()()(\xEC\x33)(\xEC\x33)(\xEC\x33)(\xEC\x33)(C)(\xCF\xE0)()()(\xEC\x33\\\xF7)()()()()()()()()()()()\x03");               
            break;
            //архив событий
            case 4:
            sprintf((char *)bufTX, "\x02()()()(\xEC\x33)(\xEC\x33)(\xEC\x33)(\xEC\x33)(\xCF\xE0)()()(C)(\xEC\x33\\\xF7)\x03");                
            break;
            //архив изменений
            case 5:
            sprintf((char *)bufTX, "\x02()()()()()()()()()\x03");
            break;
        }
    }

    return 0;
}
