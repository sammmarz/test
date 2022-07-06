/**
 \file
В файле организованны алгоритмы формирования архивов
*/

#include <time.h>
#include <string.h>
#include "mdFlashMemory/DataForFlash.h"
#include "mdTemperSensor/mdTemperSensor.h"
#include "mdPrintToUART/printToUART.h"
#include "mdRTC/mdRTC.h"
#include "Params/Events.h"


unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);
extern const unsigned short CRCInit;
/**
 \ingroup mdDataManagerGroup
 \author Shuvalov Sergey
 \brief Функция заполняет структуру данных архива событий.
*/
void FormingEventsArchive(uint16_t event)
{
    eventsArchive.count = ++params.countEventArc;
    eventsArchive.globCount = ++globCountFlash;
    struct tm st;
    memset(&st, 0, sizeof(struct tm));
    getRtcDateTime(&st);
    eventsArchive.timeDate = mktime(&st);
    eventsArchive.eventCode = event;
    eventsArchive.Vc = params.Vc;
    eventsArchive.VcO = params.VcO;
    eventsArchive.Vp = params.Vp;
    eventsArchive.VpO = params.VpO;
    eventsArchive.PpodAvI = params.PpodAvD;
    eventsArchive.KaverI = params.KaverD;
    eventsArchive.averageKkor = params.KkorAvD;
    eventsArchive.T = params.T;
    eventsArchive.Q = params.Qc;
    // исключаем cs из подсчета
    int size = (char *)&eventsArchive.cs - (char *)&eventsArchive;
    eventsArchive.cs = GetCRC16((unsigned char *)&eventsArchive, size, CRCInit);
}
/**
 \ingroup mdDataManagerGroup
 \author Shuvalov Sergey
 \brief Функция заполняет структуру данных архива изменений.
*/
void FormingChangesArchive(uint16_t event)
{
    changesArchive.count = ++params.countChangesArc;
    changesArchive.globCount = ++globCountFlash;
    struct tm st;
    memset(&st, 0, sizeof(struct tm));
    getRtcDateTime(&st);
    changesArchive.timeDate = mktime(&st);
    // исключаем cs из подсчета
    int size = (char *)&changesArchive.cs - (char *)&changesArchive;
    changesArchive.cs = GetCRC16((unsigned char *)&changesArchive, size, CRCInit);
}
/**
 \ingroup mdDataManagerGroup
 \author Shuvalov Sergey
 \brief Функция заполняет структуру данных дневного архива.
*/
void FormingDayArchive(uint16_t event)
{
    dayArchive.count = ++params.countDayArc;
    dayArchive.globCount = ++globCountFlash;
    struct tm st;
    memset(&st, 0, sizeof(struct tm));
    getRtcDateTime(&st);
    dayArchive.timeDate = mktime(&st);
    dayArchive.Vc = params.Vc;
    dayArchive.VcO = params.VcO;
    dayArchive.Vp = params.Vp;
    dayArchive.VpO = params.VpO;
    dayArchive.averageTemper = params.TaverD;
    dayArchive.PpodAvI = params.PpodAvD;
    dayArchive.KaverI = params.KaverD;
    dayArchive.averageKkor = params.KkorAvD;
    dayArchive.QcAver = params.QcAvD;
    dayArchive.st1 = params.curEvents.events[ST_1];
    dayArchive.st2 = params.curEvents.events[ST_2];
    dayArchive.st3 = params.curEvents.events[ST_3];
    dayArchive.st4 = params.curEvents.events[ST_4];
    dayArchive.st5 = params.curEvents.events[ST_5];
    dayArchive.st6 = params.curEvents.events[ST_6];
    dayArchive.st7 = params.curEvents.events[ST_7];
    dayArchive.st8 = params.curEvents.events[ST_8];
    dayArchive.st9 = params.curEvents.events[ST_9];
    dayArchive.sysSt = params.curEvents.events[SYS_ST];
    dayArchive.eventCode = event;
    // исключаем cs из подсчета
    int size = (char *)&dayArchive.cs - (char *)&dayArchive;
    dayArchive.cs = GetCRC16((unsigned char *)&dayArchive, size, CRCInit);
}
/**
 \ingroup mdDataManagerGroup
 \author Shuvalov Sergey
 \brief Функция заполняет структуру данных месячного архива.
*/
void FormingMonthArchive(uint16_t event)
{
    monthArchive.count = ++params.countMonthArc;
    monthArchive.globCount = ++globCountFlash;
    struct tm st;
    memset(&st, 0, sizeof(struct tm));
    getRtcDateTime(&st);
    monthArchive.timeDate = mktime(&st);
    monthArchive.Vc = params.Vc;
    monthArchive.VcO = params.VcO;
    monthArchive.Vp = params.Vp;
    monthArchive.VpO = params.VpO;
    monthArchive.averageTemper = params.TaverM;
    monthArchive.PpodAvI = params.PpodAvM;
    monthArchive.KaverI = params.KaverM;
    monthArchive.averageKkor = params.KkorAvM;
    monthArchive.QcAver = params.QcAvM;
    monthArchive.st1 = params.curEvents.events[ST_1];
    monthArchive.st2 = params.curEvents.events[ST_2];
    monthArchive.st3 = params.curEvents.events[ST_3];
    monthArchive.st4 = params.curEvents.events[ST_4];
    monthArchive.st5 = params.curEvents.events[ST_5];
    monthArchive.st6 = params.curEvents.events[ST_6];
    monthArchive.st7 = params.curEvents.events[ST_7];
    monthArchive.st8 = params.curEvents.events[ST_8];
    monthArchive.st9 = params.curEvents.events[ST_9];
    monthArchive.sysSt = params.curEvents.events[SYS_ST];
    monthArchive.eventCode = event;
    // исключаем cs из подсчета
    int size = (char *)&monthArchive.cs - (char *)&monthArchive;
    monthArchive.cs = GetCRC16((unsigned char *)&monthArchive, size, CRCInit);
}
/**
 \ingroup mdDataManagerGroup
 \author Shuvalov Sergey
 \brief Функция заполняет структуру данных интервального архива.
*/
void FormingIntervalArchive(uint16_t event)
{
    intervalArchive.count = ++params.countIntervArc;
    intervalArchive.globCount = ++globCountFlash;
    struct tm st;
    memset(&st, 0, sizeof(struct tm));
    getRtcDateTime(&st);
    intervalArchive.timeDate = mktime(&st);
    intervalArchive.Vc = params.Vc;
    intervalArchive.VcO = params.VcO;
    intervalArchive.Vp = params.Vp;
    intervalArchive.VpO = params.VpO;
    intervalArchive.averageTemper = params.TaverI;
    intervalArchive.PpodAvI = params.PpodAvI;
    intervalArchive.KaverI = params.KaverI;
    intervalArchive.averageKkor = params.KkorAvI;
    intervalArchive.QcAver = params.QcAvI;
    intervalArchive.st1 = params.curEvents.events[ST_1];
    intervalArchive.st2 = params.curEvents.events[ST_2];
    intervalArchive.st3 = params.curEvents.events[ST_3];
    intervalArchive.st4 = params.curEvents.events[ST_4];
    intervalArchive.st5 = params.curEvents.events[ST_5];
    intervalArchive.st6 = params.curEvents.events[ST_6];
    intervalArchive.st7 = params.curEvents.events[ST_7];
    intervalArchive.st8 = params.curEvents.events[ST_8];
    intervalArchive.st9 = params.curEvents.events[ST_9];
    intervalArchive.sysSt = params.curEvents.events[SYS_ST];
    intervalArchive.eventCode = event;
    // исключаем cs из подсчета
    int size = (char *)&intervalArchive.cs - (char *)&intervalArchive;
    intervalArchive.cs = GetCRC16((unsigned char *)&intervalArchive, size, CRCInit);
}
/**
 \ingroup mdDataManagerGroup
 \author Shuvalov Sergey
 \brief Функция заполняет структуру данных интервального архива. За основу взят
    интервальный архив ТС.
*/
void FormingIntervalArchiveTC(uint16_t event)
{
    intervalArchive.count = ++params.countIntervArc;
    struct tm st;
    memset(&st, 0, sizeof(struct tm));
    getRtcDateTime(&st);
    intervalArchive.timeDate = mktime(&st);
    intervalArchive.Vc = params.Vc;
    intervalArchive.VcO = params.VcO;
    intervalArchive.Vp = params.Vp;
    intervalArchive.VpO = params.VpO;
    /*
    Параметры averageTemper, averageKkor, PpodAvI формируются в FormingCycleMeasureData()
    */
    intervalArchive.st5 = params.curEvents.events[ST_5];
    intervalArchive.st6 = params.curEvents.events[ST_6];
    intervalArchive.sysSt = params.curEvents.events[SYS_ST];
    intervalArchive.eventCode = event;
    // исключаем cs из подсчета
    int size = (char *)&intervalArchive.cs - (char *)&intervalArchive;
    intervalArchive.cs = GetCRC16((unsigned char *)&intervalArchive, size, CRCInit);
}