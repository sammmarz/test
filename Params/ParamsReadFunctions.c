/**
 \file
Файл содержит функции для чтения параметров устройства.
*/

#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "Params.h"
#include "Events.h"
#include "../mdRTC/mdRTC.h"

/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция интерпретирует значение как DT_Uchar | DT_Ushort | DT_Ulong
*/
void RdUint (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;

    switch (DataTable[indx].AC_DataType)
    {
        case DT_Ulong :
            k = sprintf((char *) p_Buff, "(%lu", *((unsigned long *) value));
            break;

        case DT_Ushort :
            k = sprintf((char *) p_Buff, "(%u", *((unsigned short *) value));
            break;

        default : // DT_Uchar
            k = sprintf((char *) p_Buff, "(%u", *((unsigned char *) value));
    }

    p_Buff+= k;

    if (DataTable[indx].AC_DimStr)
    {
        k = sprintf((char *) p_Buff, "*%s", (char *) (DataTable[indx].AC_DimStr));
        p_Buff+= k;
    }

    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция интерпретирует значение как DT_Sshort | DT_Slong
*/
void RdInt (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;

    switch (DataTable[indx].AC_DataType)
    {
        case DT_Slong :
            k = sprintf((char *) p_Buff, "(%ld", *((long *) value));
            break;

        case DT_Sshort :
            k = sprintf((char *) p_Buff, "(%d", *((short *) value));
            break;
    }

    p_Buff+= k;

    if (DataTable[indx].AC_DimStr)
    {
        k = sprintf((char *) p_Buff, "*%s", (char *) (DataTable[indx].AC_DimStr));
        p_Buff+= k;
    }

    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция интерпретирует значение как DT_Uchar | DT_Ushort | DT_Ulong в HEX
*/
void RdUintHex(unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;

    switch (DataTable[indx].AC_DataType)
    {
        case DT_Ulong :
            k = sprintf((char *) p_Buff, "(%#lx", *((unsigned long *) value));
            break;

        case DT_Ushort :
            k = sprintf((char *) p_Buff, "(%#x", *((unsigned short *) value));
            break;

        default : // DT_Uchar
            k = sprintf((char *) p_Buff, "(%#x", *((unsigned char *) value));
    }

    p_Buff+= k;

    if (DataTable[indx].AC_DimStr)
    {
        k = sprintf((char *) p_Buff, "*%s", (char *) (DataTable[indx].AC_DimStr));
        p_Buff+= k;
    }

    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция интерпретирует значение как string
*/
void RdString (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;
    k = sprintf((char *) p_Buff, "(%s", (char *) value);
    p_Buff+= k;

    if (DataTable[indx].AC_DimStr)
    {
        k = sprintf((char *) p_Buff, "*%s", (char *) (DataTable[indx].AC_DimStr));
        p_Buff+= k;
    }

    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает текущее время контроллера
*/
void RdDateTime (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;
    struct tm dateTime = {0};
    getRtcDateTime(&dateTime);
    k = strftime((char *)p_Buff, 32, "(%H:%M,%d.%m.%Y", &dateTime);
    p_Buff+= k;
    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает время начала данных для передачи данных на сервер
*/
void RdFTPdateTime (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;
    struct tm *dateTime = localtime(&params.beginTimeDataFTP);
    k = strftime((char *)p_Buff, 32, "(%H:%M,%d.%m.%Y", dateTime);
    p_Buff+= k;
    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает UTC
*/
void RdUTC (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;
    k = sprintf((char *) p_Buff, "(UTC%+d", *((short *) value));
    p_Buff+= k;
    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция интерпретирует значение как double
*/
void RdFloat (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;

    switch (DataTable[indx].AC_Format)
    {
        case F_D4 :
            k = sprintf((char *) p_Buff, "(%.4f", *((double *) value));
            break;

        case F_D2 :
            k = sprintf((char *) p_Buff, "(%.2f", *((double *) value));
            break;

        default : // DT_Uchar
            k = sprintf((char *) p_Buff, "(%g", *((double *) value));
    }

    p_Buff+= k;

    if (DataTable[indx].AC_DimStr)
    {
        k = sprintf((char *) p_Buff, "*%s", (char *) (DataTable[indx].AC_DimStr));
        p_Buff+= k;
    }

    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает значение температуры. Температура хранится в цельсиях
*/
void RdTemperature (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;
    double kelv = 0;

    if(DataTable[indx].Spec == 0)
    {
        //преобразуем к Кельвинам
        kelv += 273.15;
    }

    k = sprintf((char *) p_Buff, "(%.2f", *((double *) value) + kelv);
    p_Buff+= k;

    if (DataTable[indx].AC_DimStr)
    {
        k = sprintf((char *) p_Buff, "*%s", (char *) (DataTable[indx].AC_DimStr));
        p_Buff+= k;
    }

    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает значение эталонной температуры для точки калибровки. Температура хранится в цельсиях
*/
void RdTemperCalibPoint (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;

    k = sprintf((char *) p_Buff, "(%.3f", params.calibPoints[instansLis200-1]);
    p_Buff+= k;

    if (DataTable[indx].AC_DimStr)
    {
        k = sprintf((char *) p_Buff, "*%s", (char *) (DataTable[indx].AC_DimStr));
        p_Buff+= k;
    }

    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает значение коэффициента датчика температуры
*/
void RdKoefFromTS (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;
    if(adressLis200 == 0x280)
    {
        *((double *) value) = params.koefForTS[instansLis200-1][0];
    }
    else // 0x281
    {
        *((double *) value) = params.koefForTS[instansLis200-1][1];
    }
    
    k = sprintf((char *) p_Buff, "(%.4f", *((double *) value));    

    p_Buff+= k;

    if (DataTable[indx].AC_DimStr)
    {
        k = sprintf((char *) p_Buff, "*%s", (char *) (DataTable[indx].AC_DimStr));
        p_Buff+= k;
    }

    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает значение измеренной температуры для точки калибровки. Температура хранится в цельсиях
*/
void RdTemperCalibPointTS (unsigned indx, unsigned char *p_Buff, void *value)
{
    unsigned k;

    k = sprintf((char *) p_Buff, "(%.3f", params.calibPointsSignalTS[instansLis200-1]);
    p_Buff+= k;

    if (DataTable[indx].AC_DimStr)
    {
        k = sprintf((char *) p_Buff, "*%s", (char *) (DataTable[indx].AC_DimStr));
        p_Buff+= k;
    }

    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает значения флагов состояний.
Позиция бита-флага в слове соответствует Instans
*/
void RdStatus (unsigned indx, unsigned char *p_Buff, void *value)
{
    bool status = *((uint32_t *) value) & (1 << (DataTable[indx].Instans-1));
    unsigned k;
    k = sprintf((char *) p_Buff, "(%d)\x03", (int)status);
    p_Buff+= k;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает значения кода отслеживаемых событий в триггере
*/
void RdTrigger (unsigned indx, unsigned char *p_Buff, void *value)
{
    // определяем для какого архива
    StrEvents *arcPtr = 0;

    switch(instansLis200)
    {
        case 1:
            arcPtr = params.arcMonthSt;
            break;

        case 3:
            arcPtr = params.arcIntervSt;
            break;

        case 4:
            arcPtr = params.arcEventsSt;
            break;

        case 5:
            arcPtr = params.arcChangeSt;
            break;

        case 7:
            arcPtr = params.arcDaySt;
            break;
    }

    uint8_t trigNum = adressLis200 & 0xf;
    unsigned k;
    k = sprintf((char *) p_Buff, "(%#x", arcPtr[trigNum].trigger);
    p_Buff+= k;
    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает значение запрашиваемого статуса
*/
void RdStatuses (unsigned indx, unsigned char *p_Buff, void *value)
{
    uint16_t status = 0;

    if(adressLis200 == 0x111)
    {
        status = params.regEvents.events[instansLis200];
    }

    if(adressLis200 == 0x110)
    {
        status = params.curEvents.events[instansLis200];
    }

    if((adressLis200 == 0x100) && (instansLis200 == 2))
    {
        status = params.curEvents.events[0];
    }

    if((adressLis200 == 0x101) && (instansLis200 == 2))
    {
        status = params.regEvents.events[0];
    }

    unsigned k;
    k = sprintf((char *) p_Buff, "(%#x", status);
    p_Buff+= k;
    sprintf((char *) p_Buff, ")\x03");
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает список доступных массивов (для совместимости со старыми приборами)
*/
void RdArchivesString (unsigned indx, unsigned char *p_Buff, void *value)
{
    sprintf((char *) p_Buff, "((1)(3)(4)(5)(7))\x03");
}
