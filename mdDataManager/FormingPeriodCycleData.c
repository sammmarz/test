/**
\file
В файле организованны алгоритмы формирования данных цикла измерений
*/

#include <time.h>
#include <string.h>
#include "mdFlashMemory/DataForFlash.h"
#include "mdTemperSensor/mdTemperSensor.h"
#include "mdPrintToUART/printToUART.h"
#include "mdDataManager/mdDataManager.h"
#include "mdDisplay/mdDisplay.h"
#include "mdRTC/mdRTC.h"
#include "mdHall/mdHall.h"
#include "crc32.h"

#define BORDER_ALERT_QP (1 << 0)
#define BORDER_WARN_QP (1 << 1)
#define BORDER_ALERT_T (1 << 2)
#define BORDER_WARN_T_GAS (1 << 3)
#define ERROR_KKOR (1 << 4)

#define NEW_HOUR (1 << 0)
#define NEW_DAY (1 << 1)
#define NEW_MONTH (1 << 2)

/**
\ingroup mdDataManagerGroup
\author Shuvalov Sergey
\brief Функция заполняет структуру данных по истечении цикла измерений.
Минимальное значение периода цикла измерений 20 сек.
*/
void FormingCycleMeasureData(void) @ "METER_REGION"
{    
    //вводим локальные счетчики т.к. среди вычислений может придти новый сигнал
    //и мы его потеряем
    uint32_t counterHall_A_local = params.counterHall_A;
    uint32_t curCounterHalls = (counterHall_A_local - params.counterHall_A_old);
    params.counterHall_A_old = counterHall_A_local;
    //
    //заполняем массив суммой импульсов для расчета расхода
    params.countHallImpulse[params.periodIndex % params.maxPeriodsBetweenSignals] = curCounterHalls;
    params.periodIndex++;
       
    // T
    {
    // применяем коэффициенты
    int sumPeriods;
    for(sumPeriods = SUM_CALIB_PERIODS; sumPeriods >= 0; --sumPeriods)
    {
        if((int)params.calibPointsSignalTS[sumPeriods] != DEFAULT_VAL_CALIB_POINT_T)
            break;
    }
    for(int i=0; i<sumPeriods; ++i)
    {
        if(params.calibPointsSignalTS[i] <= params.Ttek <= params.calibPointsSignalTS[i+1])
        {
            params.Ttek -= params.koefForTS[i][1];
            if(params.koefForTS[i][0] != 0)
                params.Ttek /= params.koefForTS[i][0];
            break;
        }
    }
    //
    params.T = params.Ttek;    
    if((params.Ttek > params.TwarnD + params.ThystD) && (params.Ttek < params.TwarnU - params.ThystU))
    {
        params.alerts &= ~BORDER_WARN_T_GAS;
    }
    if((params.Ttek > params.TwarnU) ||
       (params.Ttek < params.TwarnD))
    {
        params.alerts |= BORDER_WARN_T_GAS;
    }
    
    if((params.Ttek > params.TalertD + params.ThystD) && (params.Ttek < params.TalertU - params.ThystU))
    {
        params.alerts &= ~BORDER_ALERT_T;
    }
    if(params.Ttek > params.TalertU)
    {
        params.T = params.Tpod;
        params.alerts |= BORDER_ALERT_T;
    }
    
    if(params.Ttek < params.TalertD)
    {
        params.T = params.Tpod;
        params.alerts |= BORDER_ALERT_T;
    }
    
    params.aTi.sum += params.T;
    params.aTi.count++;
    params.TaverI = params.aTi.sum / params.aTi.count;
    //
    params.aTTC.sum += params.T;
    params.aTTC.count++;
    params.TaverD = params.aTTC.sum / params.aTTC.count;
    //
    params.aTTM.sum += params.T;
    params.aTTM.count++;
    params.TaverM = params.aTTM.sum / params.aTTM.count;
    
    if(params.TmaxM < params.T)
    {
        params.TmaxM = params.T;
    }
    
    if(params.TminM > params.T)
    {
        params.TminM = params.T;
    }
    
    if(params.TmaxD < params.T)
    {
        params.TmaxD = params.T;
    }
    
    if(params.TminD > params.T)
    {
        params.TminD = params.T;
    }
    }
    //
    // Kkor коэффициент коррекции (
    {
    params.alerts &= ~ERROR_KKOR;
    if(params.K *params.Pstand * params.Ppod)
    {
        params.Kkor = (params.Ppod * (params.Tstand + 273.15)) /
            (params.K *params.Pstand * (params.T + 273.15));
    }
    else
    {
        params.Kkor = 1;
        params.alerts |= ERROR_KKOR;
    }
    
    //для интервального архива - средний коэф. коррекции
    params.aKkorAvI.sum += params.Kkor;
    params.aKkorAvI.count++;
    params.KkorAvI = params.aKkorAvI.sum / params.aKkorAvI.count;
    //
    params.aKkorAvD.sum += params.Kkor;
    params.aKkorAvD.count++;
    params.KkorAvD = params.aKkorAvD.sum / params.aKkorAvD.count;
    //
    params.aKkorAvM.sum += params.Kkor;
    params.aKkorAvM.count++;
    params.KkorAvM = params.aKkorAvM.sum / params.aKkorAvM.count;
    //
    //для интервального архива - средний подстановочное давление
    params.aPpodAvI.sum += params.Ppod;
    params.aPpodAvI.count++;
    params.PpodAvI = (uint32_t)params.aPpodAvI.sum / params.aPpodAvI.count;
    //
    params.aPpodAvD.sum += params.Ppod;
    params.aPpodAvD.count++;
    params.PpodAvD = (uint32_t)params.aPpodAvD.sum / params.aPpodAvD.count;
    //
    params.aPpodAvM.sum += params.Ppod;
    params.aPpodAvM.count++;
    params.PpodAvM = (uint32_t)params.aPpodAvM.sum / params.aPpodAvM.count;
    //
    //для интервального архива - средний коэф. сжимаемости
    params.aKavI.sum += params.K;
    params.aKavI.count++;
    params.KaverI = params.aKavI.sum / params.aKavI.count;
    //
    params.aKavD.sum += params.K;
    params.aKavD.count++;
    params.KaverD = params.aKavD.sum / params.aKavD.count;
    //
    params.aKavM.sum += params.K;
    params.aKavM.count++;
    params.KaverM = params.aKavM.sum / params.aKavM.count;
    //
    }
    // VpCM рабочий объём за период цикла измерений
    params.VpCM = curCounterHalls * params.koefImpulseHall;
    params.VpCMclear = curCounterHalls * params.koefImpulseHall;
    
    // если было два импульса - можно измерять расход
    taskENTER_CRITICAL();
    if(timeBetweenImp)
    {
        double timeCycleMeas = 0;
        timeCycleMeas = getTimeWithSubSecAfterPoint();
        if((timeCycleMeas - impHallTimeNew) > timeBetweenImp)
        {
            timeBetweenImp = timeCycleMeas - impHallTimeNew;
        }
        if(timeBetweenImp > 135)
        {
            timeBetweenImp = 0;
            impHallTimeNew = 0;
            impHallTimeOld = 0;
        }
    }
    if(!timeBetweenImp)
    {
        params.QTek = 0;
    }
    else
    {
        // 2 - два импульса; / 3600. - приводим к часам
        params.QTek = 2 * params.koefImpulseHall / (timeBetweenImp / 3600.);
    }
    taskEXIT_CRITICAL();
 
    //Qp
    params.Qp = params.QTek;
    params.alerts &= ~BORDER_WARN_QP;
    if( (params.QTek > params.QwarnU) || ((params.QTek < params.QwarnD) && (params.QTek != 0)) )
    {
        params.alerts |= BORDER_WARN_QP;
    }
    
    params.alerts &= ~BORDER_ALERT_QP;   
    if(params.QTek > params.QalertU)
    {
        params.Qp = params.QpodU;
        params.alerts |= BORDER_ALERT_QP;
    }    
    if(params.QTek < params.QalertD)
    {
        if(params.QTek !=0)
        {
            params.Qp = params.QpodD;
            params.alerts |= BORDER_ALERT_QP;
        }
    }
    
    //Vp рабочий объём VpB возмущенный рабочий объём
    if(!params.alerts)
    {
        params.Vp += params.VpCM;
        params.VPCP += params.VpCMclear;
    }
    else
    {
        params.VpB += params.VpCM;
        params.VPCP += params.VpCMclear;
    }
    
    params.VpTI += params.VpCM;
    params.VpTC += params.VpCM;
    params.VpTM += params.VpCM;
    params.VpO = params.Vp + params.VpB;
    //VcCM и VcBCM стандартный объем цикла измерений
    params.VcBCM = 0;
    params.VcCM = 0;
    
    if(params.alerts & BORDER_ALERT_QP)
    {
        params.VcBCM = params.Qp / (3600/params.measPeriod) * params.Kkor;
    }
    else if(params.alerts & BORDER_ALERT_T)
    {
        params.VcBCM = params.VpCM * params.Kkor;
    }
    else
    {
        params.VcCM = params.VpCM * params.Kkor;
    }
    
    // VcB Счетчик возмущенного стандартного объема	м3
    params.VcB += params.VcBCM;
    // Vc стандартного объема
    params.Vc += params.VcCM;
    // VcO стандартного объема
    params.VcO = params.Vc + params.VcB;
    // Qc стандартный расход газа
    params.Qc = params.Qp * params.Kkor;
    // средний расход за интервал
    params.aQcAvI.sum += params.Qc;
    params.aQcAvI.count++;
    params.QcAvI = params.aQcAvI.sum / params.aQcAvI.count;
    //
    // средний расход за сутки
    params.aQcAvD.sum += params.Qc;
    params.aQcAvD.count++;
    params.QcAvD = params.aQcAvD.sum / params.aQcAvD.count;
    //
    // средний расход за месяц
    params.aQcAvM.sum += params.Qc;
    params.aQcAvM.count++;
    params.QcAvM = params.aQcAvM.sum / params.aQcAvM.count;
    //
    params.VcTI += params.VcCM + params.VcBCM;
    params.VcTC += params.VcCM + params.VcBCM;
    params.VcTM += params.VcCM + params.VcBCM;
    
    params.dateTimeFlags = 0;
    
    //if(params.oldDateTime.tm_min > params.curDateTime.tm_min)
    if((params.oldDateTime.tm_min % 5) == 0)
    {
        params.dateTimeFlags |= NEW_HOUR;
        // T
        params.TaverI = params.T;        
        params.aTi.count = 1;
        params.aTi.sum = params.T;
        //Vp...
        params.VpOI = params.VpTI;
        params.VpTI = params.VpCM;
        //Vc...
        params.VcOI = params.VcTI;
        params.VcTI = params.VcCM + params.VcBCM;
        
        if((params.oldDateTime.tm_hour < params.dayBegin) && params.curDateTime.tm_hour >= params.dayBegin)
        {
            params.dateTimeFlags |= NEW_DAY;
            // T
            params.TaverD = params.T;
            params.aTTC.count = 1;
            params.aTTC.sum = params.T;
            params.TmaxD = params.T;
            params.TminD = params.T;
            //Vp...
            params.VpOC = params.VpTM;
            params.VpTC = params.VpCM;
            //Vc...
            params.VcOC = params.VcTM;
            params.VcTC = params.VcCM + params.VcBCM;
            
            if(params.curDateTime.tm_mday == 1)
            {
                params.dateTimeFlags |= NEW_MONTH;
                // T
                params.TaverM = params.T;
                params.aTTM.count = 1;
                params.aTTM.sum = params.T;
                params.TmaxM = params.T;
                params.TminM = params.T;
                //Vp...
                params.VpOM = params.VpTM;
                params.VpTM = params.VpCM;
                //Vc...
                params.VcOM = params.VcTM;
                params.VcTM = params.VcCM + params.VcBCM;
            }
        }
    }
    
    params.oldDateTime = params.curDateTime;
}
/**
\ingroup mdDataManagerGroup
\author Shuvalov Sergey
\brief Функция проверяет нужно ли записать архивы во флэш
*/
void analyseFlagsForWriteToFlash(void)
{
    StrDataManager forDataManager;
    
    if(params.dateTimeFlags & NEW_HOUR)
    {
        //сохраняем параметры во флеш каждый час
        forDataManager.cmd = DM_SAVE_PARAMS_IN_FLASH;
        xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
        forDataManager.cmd = DM_INTERVAL_ALARM;
        forDataManager.event = EV_NEW_INTERVAL_ARC;
        xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
        params.aKkorAvI.sum = 0;
        params.aKkorAvI.count = 0;
        params.aPpodAvI.sum = 0;
        params.aPpodAvI.count = 0;
        params.aKavI.sum = 0;
        params.aKavI.count = 0;
        params.aQcAvI.sum = 0;
        params.aQcAvI.count = 0;
    }
    
    if(params.dateTimeFlags & NEW_DAY)
    {
        forDataManager.cmd = DM_DAY_ALARM;
        forDataManager.event = 0;
        xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
        params.aKkorAvD.sum = 0;
        params.aKkorAvD.count = 0;
        params.aPpodAvD.sum = 0;
        params.aPpodAvD.count = 0;
        params.aKavD.sum = 0;
        params.aKavD.count = 0;
        params.aQcAvD.sum = 0;
        params.aQcAvD.count = 0;
    }
    
    if(params.dateTimeFlags & NEW_MONTH)
    {
        forDataManager.cmd = DM_MONTH_ALARM;
        forDataManager.event = 0;
        xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
        params.aKkorAvM.sum = 0;
        params.aKkorAvM.count = 0;
        params.aPpodAvM.sum = 0;
        params.aPpodAvM.count = 0;
        params.aKavM.sum = 0;
        params.aKavM.count = 0;
        params.aQcAvM.sum = 0;
        params.aQcAvM.count = 0;
    }   
    params.dateTimeFlags = 0;
}
/**
\ingroup mdDataManagerGroup
\author Shuvalov Sergey
\brief Функция проверяет флаг и устанавливает\снимает событие
*/
void analyseFlagsForSetOrResetEvent(uint32_t paramsAlertsOld, uint16_t codeEvent, uint16_t codeFlag)
{
    if(!(paramsAlertsOld & codeFlag) &&
       (params.alerts & codeFlag))
    {
        setEvent(codeEvent);
    }
    if((paramsAlertsOld & codeFlag) &&
       !(params.alerts & codeFlag))
    {
        resetEvent(codeEvent);
    }
}
/**
\ingroup mdDataManagerGroup
\author Shuvalov Sergey
\brief Функция проверяет флаги и устанавливает\снимает события
*/
void checkEvents(uint32_t paramsAlertsOld)
{
    analyseFlagsForSetOrResetEvent(paramsAlertsOld, EV_BORDER_ALERT_QP, BORDER_ALERT_QP);
    analyseFlagsForSetOrResetEvent(paramsAlertsOld, EV_BORDER_WARN_QP, BORDER_WARN_QP);
    analyseFlagsForSetOrResetEvent(paramsAlertsOld, EV_BORDER_ALERT_T, BORDER_ALERT_T);
    analyseFlagsForSetOrResetEvent(paramsAlertsOld, EV_BORDER_WARN_T_GAS, BORDER_WARN_T_GAS);
    analyseFlagsForSetOrResetEvent(paramsAlertsOld, EV_ERROR_KKOR, ERROR_KKOR);
}