#include <arm_itm.h>
#include <stdio.h>
#include "stm32l1xx.h"

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "mdFlashMemory/mdFlashMemory.h"
#include "mdFlashMemory/DataForFlash.h"
#include "mdFlashMemory/HardwareFlashMemory.h"

unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);
void writeDataInFlash(StrQueueInfoData *bufInfo);
extern const unsigned short CRCInit;
void fillFlashTestData(void)
{
    StrQueueInfoData dataForFlash = {0};
    dataForFlash.dataInfo = &intervalArchiveInfo;
    dataForFlash.length = sizeof(StrIntervalArchive);
    struct tm strTm;
    strTm.tm_year = 2018 - 1900; //Количество лет с 1900 года.  //stm32 rtc 0-159
    strTm.tm_mon = 0;         //0-11                         //stm32 rtc: 1-12
    strTm.tm_mday = 1;          //1-31
    strTm.tm_hour = 10;
    strTm.tm_min = 0;
    strTm.tm_sec = 0;
    strTm.tm_wday = 0;
    intervalArchive.timeDate = mktime(&strTm);
    struct tm *st = localtime(&intervalArchive.timeDate);
    struct tm st2 = *st;

    for(int block = 0; block < 2; block++)
    for(int i = 0; i < 16; ++i)
    {
        // формируем данные
        intervalArchive.count++;
        intervalArchive.timeDate += 3600;
        st = localtime(&intervalArchive.timeDate);
        st2 = *st;
        intervalArchive.Vc = st2.tm_year;
        intervalArchive.VcO = st2.tm_mon;
        intervalArchive.Vp = st2.tm_mday;
        intervalArchive.VpO = st2.tm_hour;
        intervalArchive.averageTemper = i + 0.5;
        intervalArchive.PpodAvI = i + 0.5;
        intervalArchive.averageKkor = i + 0.7;
        int size = (char *)&intervalArchive.cs - (char *)&intervalArchive;
        intervalArchive.cs = GetCRC16((unsigned char *)&intervalArchive, size, CRCInit);
        memcpy(dataForFlash.data, (uint8_t *)&intervalArchive, sizeof(StrIntervalArchive));
        writeDataInFlash(&dataForFlash);
    }
}

bool testFlash(void)
{
    uint8_t rdata[256] = {0};
    StrQueueInfoData dataForFlash = {0};
    
    dataForFlash.dataInfo = &intervalArchiveInfo;
    dataForFlash.length = sizeof(StrIntervalArchive);
    
    for(int i=0; i<20; ++i)
        dataForFlash.data[i] = i+1;

    writeDataInFlash(&dataForFlash);
        
    readDataFromFlash(rdata, 256, 0);
    for(int i = 0; i<20; ++i)
    {
        if(dataForFlash.data[i] != rdata[i])
            return false;
    }
    return true;
}