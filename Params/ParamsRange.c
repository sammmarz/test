
/**
 \file
Файл содержит функции проверки вводимых данных для инициализации параметров.
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Params.h"
#include "../mdLIS200/mdLis200.h"

/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция проверяет входит ли значение в диапазон
*/
int checkRange(int value, int min, int max)
{
    if((value >= min) && (value <= max))
    {
        return 0;
    }

    return EC_WrongValueRange;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция проверяет входит ли значение double в диапазон
*/
int checkRangeFloat(double value, double min, double max)
{
    if((value >= min) && (value <= max))
    {
        return 0;
    }

    return EC_WrongValueRange;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция проверяет является ли значение IP (проверка наличия 3-х точек)
*/
int checkRangeIP(char *IP)
{
    int countPoint = 0;

    for(int i = 0; i<15; ++i)
    {
        if(IP[i] == '.')
        {
            countPoint++;
        }
    }

    if(countPoint == 3)
    {
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
 \brief Функция проверяет входит ли значение в указанный диапазон
*/
int checkValueRange(uint8_t	AC_ValueRange, void *value)
{
    int error = EC_WrongValueRange;

    switch(AC_ValueRange)
    {
        case VLR_0:
            error = 0;
            break;

        case VLR_0_6:
            error = checkRange(*((int *)value), 0, 6);
            break;

        case VLR_0_23:
            error = checkRange(*((int *)value), 0, 23);
            break;

        case VLR_20_60:
            error = checkRange(*((int *)value), 20, 60);
            break;

        case VLR_0_900:
            error = checkRange(*((int *)value), 0, 900);
            break;

        case VLR_20_120:
            error = checkRange(*((int *)value), 20, 120);
            break;

        case VLR_5_3600_5:
            error = checkRange(*((int *)value), 2, 3600); //2 - для режима калибровки

            // значение должно быть кратно 5
            if(*((int *)value) % 5)
            {
                if(*((int *)value) != 2) //для режима калибровки
                    error = EC_WrongValueRange;
            }

            break;

        case VLR_20_300:
            error = checkRange(*((int *)value), 20, 300);
            break;
            
        case VLR_20000_600000:
            error = checkRange(*((int *)value), 20000, 600000);
            break;
            
        case VLR_Cels:
            error = checkRangeFloat(*((double *)value), -40, 80);
            break;

        case VLR_Kelv:
            error = checkRangeFloat(*((double *)value), 233.15, 353.15);
            break;

        case VLR_P:
            error = checkRange(*((int *)value), 50000, 600000);
            break;

        case VLR_K:
            error = checkRangeFloat(*((double *)value), 0.5, 0.15);
            break;

        case VLR_IP:
            error = checkRangeIP((char *)value);
            break;
    }

    return error;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция записывает в буфер диапазон для указанного параметра
*/
void readRangeParam(uint16_t index, char *OutBuf)
{
    switch(DataTable[index].AC_ValueRange)
    {
        case VLR_0:
            sprintf(OutBuf, "(0)\x03");
            return;
            break;

        case VLR_0_6:
            sprintf(OutBuf, "(0-6");
            return;
            break;

        case VLR_0_23:
            OutBuf+= sprintf(OutBuf, "(0-23");
            break;

        case VLR_20_60:
            OutBuf+= sprintf(OutBuf, "(20-60");
            break;

        case VLR_0_900:
            OutBuf+= sprintf(OutBuf, "(0-900");
            break;

        case VLR_20_120:
            OutBuf+= sprintf(OutBuf, "(20-120");
            break;

        case VLR_5_3600_5:
            OutBuf+= sprintf(OutBuf, "(5-3600 %5");
            break;

        case VLR_20_300:
            OutBuf+= sprintf(OutBuf, "(20-300");
            break;

        case VLR_Cels:
            OutBuf+= sprintf(OutBuf, "(-40...+60");
            break;

        case VLR_Kelv:
            OutBuf+= sprintf(OutBuf, "(233.15...353.15");
            break;

        case VLR_IP:
            OutBuf+= sprintf(OutBuf, "(XXX.XXX.XXX.XXX");
            break;
    }

    if (DataTable[index].AC_DimStr)
    {
        OutBuf+= sprintf(OutBuf, "*%s", DataTable[index].AC_DimStr);
    }

    sprintf((char *)OutBuf, ")\x03");
}
