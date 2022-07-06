/**
 \file
Файл содержит функции для записи параметров устройства.
*/
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Params.h"
#include "Events.h"
#include "mdFlashMemory/mdFlashMemory.h"
#include "mdDataManager/mdDataManager.h"
#include "mdFlashMemory/DataForFlash.h"
#include "mdModes/mdModes.h"
#include "../mdLIS200/mdLis200.h"
#include "../mdRTC/mdRTC.h"
#include "reprogram.h"
#include "mdOpticalInterface/mdOpticalInterface.h"

extern uint8_t accessStatus;
extern SemaphoreHandle_t xMutexParams;
extern uint32_t counterHall_A;
extern uint32_t counterHall_B;
extern StrDataUART optData;
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция интерпретирует значение как DT_Uchar | DT_Ushort | DT_Ulong
*/
unsigned int WrtUint (unsigned indx, unsigned char *p_Buff)
{
    errno = 0;
    uint32_t value = (uint32_t)strtol((const char *)p_Buff, 0, 0);
    int error = checkValueRange(DataTable[indx].AC_ValueRange, &value);

    if(error)
    {
        return error;
    }

    if(!errno)
    {
        switch (DataTable[indx].AC_DataType)
        {
            case DT_Ulong :
                memcpy(DataTable[indx].AC_Value, &value, sizeof(long));
                break;

            case DT_Ushort :
                memcpy(DataTable[indx].AC_Value, &value, sizeof(short));
                break;

            default : // DT_Uchar
                memcpy(DataTable[indx].AC_Value, &value, sizeof(char));
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
 \brief Функция интерпретирует значение как string
*/
unsigned int WrtString (unsigned indx, unsigned char *p_Buff)
{
    int size = 1; //1 для '\0'
    int error = checkValueRange(DataTable[indx].AC_ValueRange, p_Buff);

    if(error)
    {
        return error;
    }

    switch(DataTable[indx].AC_DataType)
    {
        case DT_Array6:
            size += 6;
            break;
            
        case DT_Array8:
            size += 8;
            break;            

        case DT_Array12:
            size += 12;
            break;

        case DT_Array16:
            size += 16;
            break;

        case DT_Array24:
            size += 24;
            break;            

        case DT_Array32:
            size += 32;
            break;
    }

    snprintf(DataTable[indx].AC_Value, size, (char const *)p_Buff);
    return 0;
}

/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция устанавливает текущее время контроллера
*/
unsigned int WrtDateTime(unsigned indx, unsigned char *p_Buff)
{
    unsigned int k, Year, Mon, Day, Hour, Min;
    k = sscanf((char *) p_Buff, "%d:%d,%d.%d.%d", &Hour, &Min, &Day, &Mon, &Year);

    if (k == 5) // Six values were converted
    {
        if (Year > 2010)
            if ((Mon < 13) && (Mon > 0))
                if ((Day > 0) && (Day <= 31))
                    if (Hour < 24)
                        if (Min < 60)
                        {
                            k = 0;
                        }

        if (k) //incorrect input
        {
            return EC_WrongChr;
        }
    }
    else
    {
        return EC_WrongChr;
    }

    struct tm dt;

    memset(&dt, 0, sizeof(struct tm));

    dt.tm_hour = Hour;

    dt.tm_min = Min;

    dt.tm_mday = Day;

    dt.tm_mon = Mon-1;

    dt.tm_year = Year - 1900;

    int error = 0;

    if(!setRtcDateTime(&dt))
    {
        error = EC_WrongValueFromSet;
    }
    else
    {
        params.timeData = mktime(&dt);
        resetEvent(EV_DATA_UPDATE);
        resetEvent(EV_RESET);
        resetEvent(EV_TIMER_NOT_SET);
    }

    return error;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция устанавливает время начала данных для передачи данных на сервер
*/
unsigned int WrtFTPdataTime(unsigned indx, unsigned char *p_Buff)
{
    unsigned int k, Year, Mon, Day, Hour, Min;
    k = sscanf((char *) p_Buff, "%d:%d,%d.%d.%d", &Hour, &Min, &Day, &Mon, &Year);

    if (k == 5) // Six values were converted
    {
        if (Year > 2010)
            if ((Mon < 13) && (Mon > 0))
                if ((Day > 0) && (Day <= 31))
                    if (Hour < 24)
                        if (Min < 60)
                        {
                            k = 0;
                        }

        if (k) //incorrect input
        {
            return EC_WrongChr;
        }
    }
    else
    {
        return EC_WrongChr;
    }

    struct tm dt;

    memset(&dt, 0, sizeof(struct tm));

    dt.tm_hour = Hour;

    dt.tm_min = Min;

    dt.tm_mday = Day;

    dt.tm_mon = Mon-1;

    dt.tm_year = Year - 1900;

    int error = 0;

    params.beginTimeDataFTP = mktime(&dt);

    return error;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция устанавливает UTC
*/
unsigned int WrtUTC(unsigned indx, unsigned char *p_Buff)
{
    int Min = 0;
    char *ch = strchr((char *)p_Buff, 'C');

    if(ch)
    {
        ch++;
        sscanf(ch, "%d", &Min);
    }
    else
    {
        return EC_WrongChr;
    }

    if((Min > 1440) || (Min < -1440))
    {
        return EC_WrongChr;
    }

    *((short *)DataTable[indx].AC_Value) = Min;
    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция принудительно сохранение данных в энергонезависимую память
*/
unsigned int saveInFlash(unsigned indx, unsigned char *p_Buff)
{
    // инициируем режим работы с данными
    StrEventMode mode;
    mode.status = 1;
    mode.modeCode = DATA_M;
    xQueueSendToBack(xModesQueue, &mode, 0);   
    //
    StrDataManager forDataManager;
    //
    forDataManager.cmd = DM_CYCLE_MEASURE;
    xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
    //
    forDataManager.cmd = DM_SAVE_PARAMS_IN_FLASH;
    xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
    //
    forDataManager.cmd = DM_INTERVAL_ALARM;
    xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
    //
    forDataManager.cmd = DM_DAY_ALARM;
    xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
    //
    forDataManager.cmd = DM_MONTH_ALARM;
    xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
    //
    forDataManager.cmd = DM_EVENT;
    xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
    //
    forDataManager.cmd = DM_CHANGE;
    xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
   
    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция осуществляет cброс счётчиков и архивов
*/
unsigned int resetArchCount(unsigned indx, unsigned char *p_Buff)
{
    // стираем счётчики
    params.counterHall_A = 0;
    params.counterHall_A_old = 0;
    params.Vc = 0;
    params.Vp = 0;
    params.VcO = 0;
    params.VpO = 0;
    params.VpB = 0;
    vTaskSuspend( xTaskDataManager );
    vTaskSuspend(xTaskWriteBufferToFlash);
    // стираем интервальный архив
    xQueueReset(xFlashWriteQueue);
    //    
    eraseFlashMemory(monthArchiveInfo.adressFlashBegin, monthArchiveInfo.adressFlashEnd);
    monthArchiveInfo.adress = monthArchiveInfo.adressFlashBegin;
    params.countMonthArc = 0;
    params.adrFlashMonthArc = 0;
    //
    eraseFlashMemory(intervalArchiveInfo.adressFlashBegin, intervalArchiveInfo.adressFlashEnd);
    intervalArchiveInfo.adress = intervalArchiveInfo.adressFlashBegin;
    params.countIntervArc = 0;
    params.adrFlashIntervArc = 0;
    //
    eraseFlashMemory(eventsArchiveInfo.adressFlashBegin, eventsArchiveInfo.adressFlashEnd);
    eventsArchiveInfo.adress = eventsArchiveInfo.adressFlashBegin;
    params.countEventArc = 0;
    params.adrFlashEventArc = 0;
    //
    eraseFlashMemory(changesArchiveInfo.adressFlashBegin, changesArchiveInfo.adressFlashEnd);
    changesArchiveInfo.adress = changesArchiveInfo.adressFlashBegin;
    params.countChangesArc = 0;
    params.adrFlashChangesArc = 0;
    //
    eraseFlashMemory(dayArchiveInfo.adressFlashBegin, dayArchiveInfo.adressFlashEnd);
    dayArchiveInfo.adress = dayArchiveInfo.adressFlashBegin;
    params.countDayArc = 0;
    params.adrFlashDayArc = 0;    
    //
    vTaskResume( xTaskDataManager );
    vTaskResume(xTaskWriteBufferToFlash);
    
    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция сбрасывает архив.
*/
unsigned int CmdResetArchive (unsigned indx, unsigned char *p_Buff)
{
    unsigned int error = 0;
    //уведомляем поток режимов об окончании работы с флеш если очередь пуста
    while(uxQueueSpacesAvailable(xFlashWriteQueue) != QUEUE_FLASH_SIZE)
    {
        vTaskDelay(1);
    }
    vTaskSuspend(xTaskDataManager);
    vTaskSuspend(xTaskWriteBufferToFlash);
    switch(instansLis200)
    {
        case 1:
            eraseFlashMemory(monthArchiveInfo.adressFlashBegin, monthArchiveInfo.adressFlashEnd);
            monthArchiveInfo.adress = monthArchiveInfo.adressFlashBegin;
            params.countMonthArc = 0;
            params.adrFlashMonthArc = 0;
        break;
        case 3:
            eraseFlashMemory(intervalArchiveInfo.adressFlashBegin, intervalArchiveInfo.adressFlashEnd);
            intervalArchiveInfo.adress = intervalArchiveInfo.adressFlashBegin;
            params.countIntervArc = 0;
            params.adrFlashIntervArc = 0;
        break;
        case 4:
            eraseFlashMemory(eventsArchiveInfo.adressFlashBegin, eventsArchiveInfo.adressFlashEnd);
            eventsArchiveInfo.adress = eventsArchiveInfo.adressFlashBegin;
            params.countEventArc = 0;
            params.adrFlashEventArc = 0;
        break;    
        case 5:
            eraseFlashMemory(changesArchiveInfo.adressFlashBegin, changesArchiveInfo.adressFlashEnd);
            changesArchiveInfo.adress = changesArchiveInfo.adressFlashBegin;
            params.countChangesArc = 0;
            params.adrFlashChangesArc = 0;
        break;
        case 7:
            eraseFlashMemory(dayArchiveInfo.adressFlashBegin, dayArchiveInfo.adressFlashEnd);
            dayArchiveInfo.adress = dayArchiveInfo.adressFlashBegin;
            params.countDayArc = 0;
            params.adrFlashDayArc = 0;
        break;   
        default:
            error = 1;
        break;        
    }
    vTaskResume(xTaskDataManager);
    vTaskResume(xTaskWriteBufferToFlash);
    
    return error;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция инициализации устройства (сброс до начальных настроек)
*/
unsigned int resetDevice(unsigned indx, unsigned char *p_Buff)
{
    vTaskSuspend(xTaskDataManager);
    vTaskSuspend(xTaskWriteBufferToFlash);

    eraseFlashMemory(monthArchiveInfo.adressFlashBegin, monthArchiveInfo.adressFlashEnd);
    eraseFlashMemory(intervalArchiveInfo.adressFlashBegin, intervalArchiveInfo.adressFlashEnd);
    eraseFlashMemory(eventsArchiveInfo.adressFlashBegin, eventsArchiveInfo.adressFlashEnd);
    eraseFlashMemory(changesArchiveInfo.adressFlashBegin, changesArchiveInfo.adressFlashEnd);
    eraseFlashMemory(dayArchiveInfo.adressFlashBegin, dayArchiveInfo.adressFlashEnd);    
    eraseFlashMemory(paramsInfo.adressFlashBegin, paramsInfo.adressFlashEnd);
    
    initParamsDefault();
    NVIC_SystemReset();
    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция сбрасывает флаги тревог
*/
unsigned int resetAlerts(unsigned indx, unsigned char *p_Buff)
{
    memset(&params.regEvents, 0, sizeof(StrEvents));
    memcpy(&params.regEvents, &params.curEvents, sizeof(StrEvents));
    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция инициирует работу с клапаном
 \todo реализовать GateControl
*/
unsigned int GateControl(unsigned indx, unsigned char *p_Buff)
{
    printf("\nGateControl");
    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция настраивает время связи с сервером
 \todo реализовать WrtPeriodTime и настройку будильника для связи с сервером
*/
unsigned int WrtPeriodTime(unsigned indx, unsigned char *p_Buff)
{
    /*  unsigned int mode, dayOfXX, hh, mm;                                         // mode
      //      0 - FTP transmission is off
      //      1 - every hour
      //      2 - every day
      //      3 - every week
      //      4 - every month
      // dayOfXX
      //      0 for modes 0,1,2
      //      1-7 for mode 3
      //      1-27, 31 for mode 4
      // hh
      //      00 for modes 0,1
      //      00-23 for modes 2,3,4
      // mm
      //      00-59
      sscanf((char const *)p_Buff, "%u-%u_%u:%u", &mode, &dayOfXX, &hh,
             &mm);                 // 4 values should be read
      time_t nextFTPConnectioTime;
      time_t timeUnix = time(NULL);                                               // Get time
      time_t cycle;
      time_t delta;
      time_t tmp;

      if (mode == 0)                                                              // OFF
      {
          nextFTPConnectioTime = 2147483647;
      }
      else if (mode == 1)                                                         // every hour
      {
          cycle = 60*60;                                                          // sec, in 1 hour
          delta = mm*60;                                                          // sec, mm(min)* 60(sec)
          tmp = (timeUnix/cycle)*cycle+delta;

          if (tmp >= timeUnix)
          {
              nextFTPConnectioTime = tmp;
          }
          else
          {
              nextFTPConnectioTime = tmp + cycle;
          }
      }
      else if (mode == 2)                                                         // every day
      {
          cycle = 24*60*60;                                                       // sec, in 1 day
          delta = (hh*60 + mm)
                  *60;                                                // sec, (hh(hours)* 60 (min) + mm(min))*60(sec)
          tmp = (timeUnix/cycle)*cycle+delta;

          if (tmp >= timeUnix)
          {
              nextFTPConnectioTime = tmp;
          }
          else
          {
              nextFTPConnectioTime = tmp + cycle;
          }
      }
      else if (mode == 3)                                                         // every week
      {
          cycle = 7*24*60*60;                                                     // sec in 7 days
          delta = ( ((dayOfXX-1)*24 + hh)*60 + mm)
                  *60;                            // sec, (full days*24 + hh(hours)* 60 (min) + mm(min))*60(sec)
          time_t shift =
              4*24*60*60;                                              // As Timestamp = 0 is Thursday, shift = thursday, friday, saturday, sunday = 4 days
          tmp = ((timeUnix-shift)/cycle)*cycle+shift+delta;

          if (tmp >= timeUnix)
          {
              nextFTPConnectioTime = tmp;
          }
          else
          {
              nextFTPConnectioTime = tmp + cycle;
          }
      }
      else if (mode == 4)                                                         // every month
      {
          struct tm timeStruct;
          struct tm *timeStructPtr = &timeStruct;
          timeStructPtr = localtime(
                              &timeUnix);                                   // fill the structure with current date info

          if (dayOfXX == 31)                                                      // on the last day of month
          {
              // find the last day of current month:
              // a) find the first day of last month
              if (timeStructPtr->tm_mon == 11)                                    // it is December now
              {
                  timeStructPtr->tm_mon = 0;                                      // set January
                  timeStructPtr->tm_year++;                                       // increment year
              }
              else
              {
                  timeStructPtr->tm_mon++;                                        // increment month
              }

              timeStructPtr->tm_mday = 1;                                         // Set the first day of month
              timeStructPtr->tm_hour = hh;                                        // Set the time of transmission
              timeStructPtr->tm_min = mm;                                         // Set the time of transmission
              timeStructPtr->tm_sec = 0;                                          // Set the time of transmission
              tmp = mktime(timeStructPtr);
              // b) substract 24 hours to get the last day of curent month
              tmp -= 86400;

              if (tmp >=
                      timeUnix)                                                // Next connection will be this month, later
              {
                  nextFTPConnectioTime = tmp;
              }
              else                                                                // Next connection will be next month
              {
                  timeStructPtr = localtime(
                                      &tmp);                                // fill the structure with next connection date

                  if (timeStructPtr->tm_mon == 11)                                // it is December now
                  {
                      timeStructPtr->tm_mon = 1;                                  // set February
                      timeStructPtr->tm_year++;                                   // increment year
                  }
                  else if (timeStructPtr->tm_mon == 10)                           // it is November now
                  {
                      timeStructPtr->tm_mon = 0;                                  // set January
                      timeStructPtr->tm_year++;                                   // increment year
                  }
                  else
                  {
                      timeStructPtr->tm_mon = timeStructPtr->tm_mon + 2;          // increment month
                  }

                  timeStructPtr->tm_mday = 1;                                     // Set the first day of month
                  tmp = mktime(timeStructPtr);
                  // c) substract 24 hours to get the last day of curent month
                  tmp -= 86400;
                  nextFTPConnectioTime = tmp;
              }
          }
          else
          {
              timeStructPtr->tm_mday =
                  dayOfXX;                                   // Set the day from command FTPcycle
              timeStructPtr->tm_hour = hh;                                        // Set the time of transmission
              timeStructPtr->tm_min = mm;                                         // Set the time of transmission
              timeStructPtr->tm_sec = 0;                                          // Set the time of transmission
              tmp = mktime(timeStructPtr);

              if (tmp >=
                      timeUnix)                                                // Next connection will be this month, later
              {
                  nextFTPConnectioTime = tmp;
              }
              else                                                                // Next connection will be next month
              {
                  if (timeStructPtr->tm_mon == 11)                                // it is December now
                  {
                      timeStructPtr->tm_mon = 0;                                  // set January
                      timeStructPtr->tm_year++;                                   // increment year
                  }
                  else
                  {
                      timeStructPtr->tm_mon++;                                    // increment month
                  }

                  tmp = mktime(timeStructPtr);
                  nextFTPConnectioTime = tmp;
              }
          }
      }
      else if (mode == 5)                                                         // every X minutes
      {
          cycle = mm*60;                                                          // sec in X minutes
          delta = mm*60;                                                          // sec, mm(min))*60(sec)
          tmp = (timeUnix/cycle)*cycle+delta;

          if (tmp >= timeUnix)
          {
              nextFTPConnectioTime = tmp;
          }
          else
          {
              nextFTPConnectioTime = tmp + cycle;
          }
      }
    */
    // return nextFTPConnectioTime;
    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция интерпретирует значение как double float
*/
unsigned int WrtFloat(unsigned indx, unsigned char *p_Buff)
{
    errno = 0;
    char *ch = strchr((char *)p_Buff, ',');

    if(ch)
    {
        *ch = '.';
    }

    double value = atof((const char *)p_Buff);
    int error = checkValueRange(DataTable[indx].AC_ValueRange, &value);

    if(error)
    {
        return error;
    }

    if(!errno)
    {
        memcpy(DataTable[indx].AC_Value, &value, sizeof(double));
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
 \brief Функция прописывает значение температуры. Температура хранится в цельсиях
*/
unsigned int WrtTemperature(unsigned indx, unsigned char *p_Buff)
{
    errno = 0;
    char *ch = strchr((char *)p_Buff, ',');

    if(ch)
    {
        *ch = '.';
    }

    double value = atof((const char *)p_Buff);
    int error = checkValueRange(DataTable[indx].AC_ValueRange, &value);

    if(error)
    {
        return error;
    }

    if(!errno)
    {
        if(DataTable[indx].Spec == 0)
        {
            //преобразуем к Цельсиям
            value -= 273.15;
        }

        memcpy(DataTable[indx].AC_Value, &value, sizeof(double));
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
 \brief Функция прописывает в триггеры событий архивов отслеживаемые события
*/
unsigned int WrtValueForTrigger(unsigned indx, unsigned char *p_Buff)
{
    errno = 0;
    uint16_t value = (uint16_t)strtol((const char *)p_Buff, 0, 0);
    int error = checkValueRange(DataTable[indx].AC_ValueRange, &value);

    if(error)
    {
        return error;
    }

    if(errno)
    {
        return EC_WrongValueRange;
    }

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

    uint8_t row = 0;
    uint8_t col = 0;
    uint8_t numEvent = (value & 0xf00) >> 8;
    uint8_t numSt = value & 0xff;
    uint8_t trigNum = adressLis200 & 0xf;

    // нет признака работы сразу со всеми статусами
    if(!(value & 0x1000))
    {
        col = numSt;
    }

    // нет признака работы сразу с несколькими событиями в статусе
    if(!(value & 0x4000))
    {
        row = numEvent;
    }

    // отмечаем какие события отслеживать
    for(int c = col; c <= numSt; ++c)
    {
        arcPtr[trigNum].events[c] = 0;

        for(int r = row; r <= numEvent; ++r)
        {
            arcPtr[trigNum].events[c] |= (1<<r);
        }
    }

    arcPtr[trigNum].trigger = value;
    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция инициирует процесс перепрограммирования
*/
unsigned int StartProgUpdate(unsigned i, unsigned char *p_Buff)
{
       optData.bufTX[0] = ACK;
       optData.bufTX[1] = 0;
       sendDataWithOpticalInterface();
       reprogramming();
     
  return 1; // возврат ошибки
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция прописывает эталонное значение температуры в точках калибровки
*/
unsigned int WrtTemperForCalibPoint(unsigned indx, unsigned char *p_Buff)
{
    errno = 0;
    char *ch = strchr((char *)p_Buff, ',');

    if(ch)
    {
        *ch = '.';
    }

    float value = atof((const char *)p_Buff);
    int error = checkValueRange(DataTable[indx].AC_ValueRange, &value);

    if((error) || (errno))
    {
        return EC_WrongValueRange;
    }
    else
    {
        // номер instansLis200 соответствует индексу массива минус 1
        params.calibPoints[instansLis200-1] = value;
    }        
    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция прописывает коэффициенты для подсчёта температуры
*/
unsigned int WrtKoefForTS(unsigned indx, unsigned char *p_Buff)
{
    errno = 0;
    char *ch = strchr((char *)p_Buff, ',');

    if(ch)
    {
        *ch = '.';
    }

    double value = atof((const char *)p_Buff);
    int error = checkValueRange(DataTable[indx].AC_ValueRange, &value);

    if((error) || (errno))
    {
        return EC_WrongValueRange;
    }
    else
    {
        if(adressLis200 == 0x280)
        {
            params.koefForTS[instansLis200-1][0] = value;  
        }
        else // 0x281
        {
            params.koefForTS[instansLis200-1][1] = value;
        }
    }

    return 0;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция прописывает значение температуры измеренное прибором в точке калибровки
*/
unsigned int SaveTemperAtCalibPoint(unsigned indx, unsigned char *p_Buff)
{
    if((-45 < params.Ttek) && (params.Ttek < 60))
    {
        params.calibPointsSignalTS[instansLis200-1] = params.Ttek;
        return 0;
    } 
    else
    {
        return 1;
    }
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция вычисляет коэффициенты исходя из эталонного и измеренного значений в точке калибровки
*/
unsigned int CountKoefForRange(unsigned indx, unsigned char *p_Buff)
{
    int i = instansLis200-1;
    if(((int)params.calibPoints[i] == DEFAULT_VAL_CALIB_POINT_T) || 
       ((int)params.calibPoints[i+1] == DEFAULT_VAL_CALIB_POINT_T) ||
       ((int)params.calibPointsSignalTS[i] == DEFAULT_VAL_CALIB_POINT_T) || 
       ((int)params.calibPointsSignalTS[i+1] == DEFAULT_VAL_CALIB_POINT_T))
    {
        return EC_WrongValueFromSet;
    }
    /*
    measVal_1 = etalVal_1*k + b
    measVal_2 = etalVal_2*k + b
    вычитаем и получаем
    k = (measVal_1 - measVal_2) /(etalVal_1 - etalVal_2)
    */
    if((params.calibPoints[i] - params.calibPoints[i+1]) != 0)
    {
        params.koefForTS[i][0] = (params.calibPointsSignalTS[i] - params.calibPointsSignalTS[i+1]) / 
            (params.calibPoints[i] - params.calibPoints[i+1]);
    }
    else
    {
        params.koefForTS[i][0] = (params.calibPointsSignalTS[i] - params.calibPointsSignalTS[i+1]);
    }
    // b = measVal_1 - etalVal_1*k
    params.koefForTS[i][1] = params.calibPointsSignalTS[i] - params.calibPoints[i] * params.koefForTS[i][0];
    
    /* 
    применение коэффициентов при получении значения температуры
    if(measVal_1 <= T <= measVal_2)
    {
        T -= params.koefForTS[i][1];
        T /= params.koefForTS[i][0];        
    }
    */
    return 0;
}
