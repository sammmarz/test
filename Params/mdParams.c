/**
 \file
Файл содержит функцию инициализации параметров значениями по умолчанию.
*/

/** \defgroup mdParamsGroup mdParams
 Специализация модуля - работа c параметрами устройства.
*/
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "Params.h"
#include "mdModes/mdModes.h"
#include "../mdFlashMemory/mdFlashMemory.h"
#include "../mdFlashMemory/DataForFlash.h"
#include "../Common.h"


// g4 1, 2 g6 3
int typeG = 3;
//
extern const unsigned short CRCInit;
unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);
void countChecksum(void);
extern uint16_t checksumMainCode;
extern uint16_t checksumMeterCode;
    

StrCorrectorParams def;
StrCorrectorParams params @0x20007AC0;
extern SemaphoreHandle_t xMutexParams;
/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция инициализирует параметры значениями по умолчанию.
*/
void initParamsDefault(void)
{
    params.dayBegin = def.dayBegin = 10;
    params.verBoard = def.verBoard = 2;

    if(typeG == 1)
    {
        strcpy(def.identDevice, "4221380001");
        strcpy(params.identDevice, def.identDevice);
        strcpy(def.typeDevice, "BK-G4_TC");//"TC210");//
        strcpy(params.typeDevice, def.typeDevice);
    }

    if(typeG == 2)
    {
        strcpy(def.identDevice, "4221380002");
        strcpy(params.identDevice, def.identDevice);
        strcpy(def.typeDevice, "BK-G4_TC");//"TC210");//
        strcpy(params.typeDevice, def.typeDevice);
    }

    if(typeG == 3)
    {
        strcpy(def.identDevice, "4221380003");
        strcpy(params.identDevice, def.identDevice);
        strcpy(def.typeDevice, "BK-G6_TC");//"TC210");//
        strcpy(params.typeDevice, def.typeDevice);
    }

    strcpy(def.Maker,
           "\xCE\xCE\xCE \"\xDD\xEB\xFC\xF1\xF2\xE5\xF0 \xC3\xE0\xE7\xFD\xEB\xE5\xEA\xF2\xF0\xEE\xED\xE8\xEa\xE0\"");
    strcpy(params.Maker, def.Maker);
    strcpy(def.versMain, VER_SOFT_MAIN);
    strcpy(params.versMain, VER_SOFT_MAIN);
    strcpy(def.versMeter, VER_SOFT_METR);
    strcpy(params.versMeter, VER_SOFT_METR);
    //
    countChecksum();
    params.crcMain = def.crcMain = checksumMainCode;
    params.crcMeter = def.crcMeter = checksumMeterCode;
    //
    params.volBat1 = def.volBat1 = ALL_BAT_VOL;
    params.volBat2 = def.volBat2 = ALL_BAT_VOL;
    params.lifeBat1 = def.lifeBat1 = TOUR_BAT;
    params.lifeBat2 = def.lifeBat2 = TOUR_BAT;
    //
    params.utc = def.utc = 180; //московское время
    strcpy(def.typeTempDevice, "tmp117");
    strcpy(params.typeTempDevice, def.typeTempDevice);
    params.measPeriod = def.measPeriod = 20;
    params.lcdTimeOff = def.lcdTimeOff = 30;
    params.lcdTimeToMain = def.lcdTimeToMain = 30;
    strcpy(def.envirTempRange, "-40...+60");
    strcpy(params.envirTempRange, def.envirTempRange);
    params.koefTimer = def.koefTimer = 1;
    params.koefImpulseHall = def.koefImpulseHall = 0.00015;
    params.maxPeriodsBetweenSignals = def.maxPeriodsBetweenSignals = 6;
    params.Vc = def.Vc = 0;
    params.Vp = def.Vp = 0;
    params.VcB = def.VcB = 0;
    params.VpB = def.VpB = 0;
    params.VcO = def.VcO = 0;
    params.VpO = def.VpO = 0;
    params.VcTI = def.VcTI = 0;
    params.VcOI = def.VcOI = 0;
    params.VcTC = def.VcTC = 0;
    params.VcOC = def.VcOC = 0;
    params.VcTM = def.VcTM = 0;
    params.VcOM = def.VcOM = 0;
    params.VpTI = def.VpTI = 0;
    params.VpOI = def.VpOI = 0;
    params.VpTC = def.VpTC = 0;
    params.VpOC = def.VpOC = 0;
    params.VpTM = def.VpTM = 0;
    params.VpOM = def.VpOM = 0;
    params.Qc = def.Qc = 0;
    params.QcAvI = def.QcAvI = 0;
    params.QcAvD = def.QcAvD = 0;
    params.QcAvM = def.QcAvM = 0;
    params.Qp = def.Qp = 0;
    params.QTek = def.QTek = 0;
    params.Kkor = def.Kkor = 1;
    params.KkorAvI = def.KkorAvI = 1;
    params.KkorAvD = def.KkorAvD = 1;
    params.KkorAvM = def.KkorAvM = 1;
    params.T = def.T = 20;
    params.Ttek = def.Ttek = 0;
    params.Tpod = def.Tpod = 20;
    params.Ppod = def.Ppod = 101325;
    params.PpodAvI = def.PpodAvI = 101325;
    params.PpodAvD = def.PpodAvM = 101325;
    params.PpodAvD = def.PpodAvM = 101325;
    params.QpodD = def.QpodD = 0.04;

    if(typeG == 3)
    {
        params.QpodD = def.QpodD = 0.06;
    }

    params.QpodU = def.QpodU = 7;

    if(typeG == 3)
    {
        params.QpodU = def.QpodU = 11;
    }

    params.K = def.K = 0.97;
    params.KaverI = def.KaverI = 1;
    params.KaverD = def.KaverD = 1;
    params.KaverM = def.KaverM = 1;
    params.Tstand = def.Tstand = 20;
    params.Pstand = def.Pstand = 101325;
    params.TalertU = def.TalertU = 60;
    params.TalertD = def.TalertD = -40;
    params.TwarnU = def.TwarnU = 60;
    params.TwarnD = def.TwarnD = -40;   
    params.TenvWarnU = def.TenvWarnU = 60;
    params.TenvWarnD = def.TenvWarnD = -40;    
    params.ThystU = def.ThystU = 1;
    params.ThystD = def.ThystD = 1;      
    params.QalertU = def.QalertU = 10000;
    params.QwarnU = def.QwarnU = 10000;
    params.QwarnD = def.QwarnD = 0.04;

    if(typeG == 3)
    {
        params.QalertU = def.QalertU = 10;
    }

    params.QalertD = def.QalertD = 0.04;

    if(typeG == 3)
    {
        params.QalertD = def.QalertD = 0.06;
    }
    // для калибровки
    for(int i = 0; i<10; ++i)
    {
        params.calibPoints[i] = DEFAULT_VAL_CALIB_POINT_T;
        params.calibPointsSignalTS[i] = DEFAULT_VAL_CALIB_POINT_T;
    }
    for(int i = 0; i<9; ++i)
    {
        params.koefForTS[i][0] = 1;
        params.koefForTS[i][1] = 0;
    }
    //
    params.TrangeD = def.TrangeD = -40;
    params.TrangeU = def.TrangeU = 60;
    params.TaverD = def.TaverD = 0;
    params.TaverM = def.TaverM = 0;
    params.TmaxD = def.TmaxD = 0;
    params.TmaxM = def.TmaxM = 0;
    params.TminD = def.TminD = 0;
    params.TminM = def.TminM = 0;
    params.Tenvironment = def.Tenvironment = 0;
    params.statusReg = def.statusReg = 0;
    params.passwS = def.passwS = 0;
    params.passwU = def.passwU = 0;
    params.Smodem = def.Smodem = 0;
    params.GSMsgnl = def.GSMsgnl = 0;
    params.Sopt = def.Sopt = 0;
    strcpy(def.Apn, "internet.mts.ru");
    strcpy(params.Apn, def.Apn);
    strcpy(def.LoginIn, "mts");
    strcpy(params.LoginIn, def.LoginIn);
    strcpy(def.PasswIn, "mts");
    strcpy(params.PasswIn, def.PasswIn);
    strcpy(def.IP, "0.0.0.0");
    strcpy(params.IP, def.IP);
    params.PortS = def.PortS = 0;
    strcpy(def.LoginS, "");
    strcpy(params.LoginS, def.LoginS);
    strcpy(def.PasswS, "");
    strcpy(params.PasswS, def.PasswS);
    params.Npin = def.Npin = 0;
    params.Spin = def.Spin = 0;
    params.TservC = def.TservC = -10;
    params.TclsGSM = def.TclsGSM = 20;
    params.TGSMmax = def.TGSMmax = 600;
    params.TclsOpt = def.TclsOpt = 30;
    params.TsyncA = def.TsyncA = 0;
    strcpy(def.TimSCon, "");
    strcpy(params.TimSCon, def.TimSCon);
    params.GatePas = def.GatePas = 0;
    params.arcType_3 = def.arcType_3 = 3;
    params.arcType1 = def.arcType1 = 461;
    params.arcType3 = def.arcType3 = 463;
    params.arcType4 = def.arcType4 = 464;
    params.arcType5 = def.arcType5 = 465;
    params.arcType7 = def.arcType7 = 467;
    params.speedUARTcode = def.speedUARTcode = 6; //19200
    params.sendErrLis200 = def.sendErrLis200 = 1;
    memset(&params.regEvents, 0, sizeof(StrEvents));
    memset(&params.curEvents, 0, sizeof(StrEvents));
    //program update
    params.LockPr = def.LockPr = 0;
    params.PassPr = def.PassPr = 0;
    params.RegPr = def.RegPr = 0;
    //
    params.timeOutForOpticIntOff = def.timeOutForOpticIntOff = 60000 * 120;
    params.timeOutForSwitch300bod = def.timeOutForSwitch300bod = 20000;
    //параметризация
    // ---> FTP
    sprintf(params.Apn, "internet.mts.ru\0");                                   // default APN
    sprintf(params.LoginIn, "mts.ru\0");                                        // default login
    sprintf(params.PasswIn, "mts.ru\0");                                        // default pass
    sprintf(params.IP, "82.208.88.82\0");                                       // FTP IP-address
    params.PortS = 48100;                                                       // FTP port
    sprintf(params.LoginS, "BK\0");                                             // default FTP login
    sprintf(params.PasswS, "BK\0");                                             // default FTP pass
    params.beginTimeDataFTP = 0;
    // ---< FTP
    params.measPeriod = 2;
    params.maxPeriodsBetweenSignals = 6;
    params.speedUARTcode = 5;

    // для 2-х литровых (2\1000\8) 2\1000 - приведение к кубам 8- количество сигналов
    // g1 - 4
    if(typeG == 1)
    {
        params.koefImpulseHall = 0.30674847 / 1000;//0.00025; 31162356
        params.koefImpulseHall -= params.koefImpulseHall / 100. * 1.5; //уменьшаем на 1.5 %
    }
    else if (typeG == 2)
    {
        params.koefImpulseHall = 0.31162356 / 1000;//0.00025; 31162356
        params.koefImpulseHall -= params.koefImpulseHall / 100. * 1.; //уменьшаем на 1.%
    }
    else if (typeG == 3)
    {
        params.koefImpulseHall = 0.49646643 / 1000;//0.00025; 31162356
        params.koefImpulseHall += params.koefImpulseHall / 100. * 4.4;
    }

    // в архив событий записываются все события
    for(int trig = 0; trig < SUM_TRIGGERS; trig++)
    {
        for(int st = 0; st < SUM_STATUS_EVENTS; st++)
        {
            params.arcEventsSt[trig].events[st] = 0xffff;
        }
    }

    //
}
/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция записывает параметры во флеш.
*/
void writeParamsToFlash(void)
{
    xSemaphoreTake (xMutexParams, portMAX_DELAY);
    params.count++;
    int size = (char *)&params.crc16 - (char *)&params;
    params.crc16 = GetCRC16((unsigned char *)&params, size, CRCInit);
    writeDataToFlashQueue((uint8_t *)&params, sizeof(StrCorrectorParams), &paramsInfo);
    xSemaphoreGive(xMutexParams);
}
/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция инициализирует параметры из флеш (размнер блоков равен 4096(0x1000)),
если есть корректные записи иначе возвращает 0
*/
int initParamsFromFlash(void)
{
    int sizeBlock = 0x1000;
    // определяем количество блоков
    int sumBlock = (paramsInfo.adressFlashEnd - paramsInfo.adressFlashBegin + 1) / sizeBlock;
    // находим самую последнюю запись
    uint32_t oldCount = 0;
    uint32_t crc16;
    bool crcError = false;
    int nBlock;

    for(nBlock = 0; nBlock<sumBlock; ++nBlock)
    {
        readDataFromFlash((uint8_t *)&params, sizeof(uint32_t),
                          paramsInfo.adressFlashBegin + nBlock * sizeBlock);

        if((params.count == 0xFFFFFFFF))
        {
            if(!nBlock)
            {
                //ещё не было ни одной записи
                return 0;
            }

            //будем работать с последней хорошей записью
            --nBlock;
            readDataFromFlash((uint8_t *)&params, sizeof(StrCorrectorParams),
                              paramsInfo.adressFlashBegin + nBlock * sizeBlock);
            int size = (char *)&params.crc16 - (char *)&params;
            crc16 = GetCRC16((unsigned char *)&params, size, CRCInit);

            if(crc16 != params.crc16)
            {
                crcError = true;
            }

            break;
        }

        // переполнение или пока в этот блок еще не записывали - надо анализировать предыдущую запись
        if(params.count < oldCount)
        {
            nBlock--;
        }

        //последний блок в памяти или переполнение
        if((nBlock == (sumBlock-1)) || (params.count < oldCount))
        {
            readDataFromFlash((uint8_t *)&params, sizeof(StrCorrectorParams),
                              paramsInfo.adressFlashBegin + nBlock * sizeBlock);
            int size = (char *)&params.crc16 - (char *)&params;
            crc16 = GetCRC16((unsigned char *)&params, size, CRCInit);

            if(crc16 != params.crc16)
            {
                crcError = true;
            }

            break;
        }

        //смещаемся дальше по памяти
        oldCount = params.count;
    }

    if(crcError)
    {
        //ошибка в первой записи
        if(nBlock == 0)
        {
            // может раньше сохраняли параметры - проанализируем последний блок во флеш
            // 17,2,3....16 (в первом блоке последняя запись)
            nBlock = sumBlock;
        }

        //пробуем взять предпоследнюю запись
        readDataFromFlash((uint8_t *)&params, sizeof(uint32_t),
                          paramsInfo.adressFlashBegin + (nBlock-1) * sizeBlock);
        int size = (char *)&params.crc16 - (char *)&params;
        crc16 = GetCRC16((unsigned char *)&params, size, CRCInit);

        // если и здесь ошибка, то прибор явно работает не корректно и нужно брать значения по умолчанию
        if(crc16 != params.crc16)
        {
            return 0;
        }
    }

    // параметры восстановлены успешно
    return 1;
}
