/**
 \file
Файл содержит функции для централизованного управления данными.
*/

/** \defgroup mdDataManagerGroup mdDataManager
 Специализация модуля - централизованное управление данными (формирование,
запись на флеш, отправка на сервер).
*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "mdDataManager/mdDataManager.h"
#include "mdModes/mdModes.h"
#include "mdPrintToUART/printToUART.h"
#include <string.h>
#include "mdRTC/mdRTC.h"
// otlad
#include "mdTemperSensor/mdTemperSensor.h"
#include "mdTemperSensor/HardwareTemperSensor.h"
#include "mdHardware/mdHardware.h"
#include "mdFlashMemory/mdFlashMemory.h"
#include "mdDisplay/mdDisplay.h"
#include "../Params/Params.h"
#include "../mdPower/mdPower.h"
extern double temperature;
// otlad end

QueueHandle_t xDataManagerQueue;
TaskHandle_t xTaskDataManager;

extern SemaphoreHandle_t xMutexParams;
extern void FormingCycleMeasureData(void);
extern void FormingEventsArchive(uint16_t event);
extern void FormingChangesArchive(uint16_t event);
extern void FormingDayArchive(uint16_t event);
extern void FormingMonthArchive(uint16_t event);
extern void FormingIntervalArchive(uint16_t event);
extern void FormingIntervalArchiveTC(uint16_t event);
extern void checkEvents(uint32_t paramsAlertsOld);
void analyseFlagsForWriteToFlash(void);
void checkFlashError(void);
/**
 \ingroup mdDataManagerGroup
 \author Shuvalov Sergey
 \brief Функция в бесконечным цикле ожидает сообщений\команд для формирования,
  отправки данных, сохранения данных на флеш.
*/
void startConversionFromTemperTask(void)
{   
        //помещаем информацию о режиме в очередь
    StrEventMode mode;
    mode.status = 1;
    mode.modeCode = METERS_M;
    xQueueSendToBack(xModesQueue, &mode, NULL);
    
    uint8_t eventTemper = TEMPER_START_CONVERSION;
    xQueueSendToBack(xTemperSensorQueue, &eventTemper, NULL);    
}
/**
 \ingroup mdDataManagerGroup
 \author Shuvalov Sergey
 \brief Функция в получает значение температуры окруж. среды с датчика температуры MCU
и устанавлевает\сбрасывает предупреждения по значениям температуры.
*/
void getTenvironment(void)
{
    // Tenvironment
    static uint8_t countForTe = 0;
    //15 - 15*20сек. == 5 мин.
    if(!(countForTe % 15))
    {
        params.Tenvironment = getMCUTemperSensorValue();
        if((params.Tenvironment > params.TenvWarnU) ||
           (params.Tenvironment < params.TenvWarnD))
        {
            setEvent(EV_BORDER_WARN_T_ENV);
        }
        else
        {
            resetEvent(EV_BORDER_WARN_T_ENV);
        }
    }

    countForTe++;
}
/**
 \ingroup mdDataManagerGroup
 \author Shuvalov Sergey
 \brief Функция в бесконечным цикле ожидает сообщений\команд для формирования,
  отправки данных, сохранения данных на флеш.
*/
void dataControlThread(void *arg)
{
    StrDataManager dataManager;
    StrEventForDisplay cmdUpdate;
    uint32_t paramsAlertsOld;
    time_t delta = 0;
    while(1)
    {
        xQueueReceive(xDataManagerQueue, &dataManager, (TickType_t) portMAX_DELAY);
    
        switch(dataManager.cmd)
        {
            //обработка события цикла измерений
            case DM_CYCLE_MEASURE:
                xSemaphoreTake(xMutexParams, portMAX_DELAY);
                getTenvironment();
                //сохраняем состояние флагов тревог
                paramsAlertsOld = params.alerts;
                
                //анализ времени  и текущей даты
                memset(&params.curDateTime, 0, sizeof(struct tm));
                getRtcDateTime(&params.curDateTime);
                params.curDateTime.tm_year += 1900;
                if(params.oldDateTime.tm_year)
                {
                    delta = mktime(&params.curDateTime) - mktime(&params.oldDateTime);
                }
                FormingCycleMeasureData();    
                analyseFlagsForWriteToFlash();                
                // проверяем возможно возникла\снялась тревога
                checkEvents(paramsAlertsOld);                    
                xSemaphoreGive(xMutexParams);
                //
                // получаем значение напряжения с батарей                     
                if(delta >= 864000) //60*60*24*10 - прошло 10 дней
                {
                    initRequestBat();
                    setRequestBat();
                    float vBatA = getVoltageBat(BAT_A);
                    float vBatB = getVoltageBat(BAT_B);
                    resetRequestBat();
                    if(vBatA < 3.1)
                        setEvent(EV_BAT1_LOW);
                    if(vBatB < 3.1)
                        setEvent(EV_BAT2_LOW);
                }
                // 
                startConversionFromTemperTask();
                //
                xSemaphoreTake(xMutexParams, portMAX_DELAY);
                //controlBoxOpen();
                //controlBatBoxOpen();
                //controlMagnitActivity(2);
                xSemaphoreGive(xMutexParams);

                if(displayActive)
                {
                    cmdUpdate.status = 1;
                    cmdUpdate.codeEventForDisplay = UPDATE_DISPLAY;
                    xQueueSendToBack(xBtnsQueue, &cmdUpdate, NULL);
                }
                break;

            //обработка события формирования интервального архива
            case DM_INTERVAL_ALARM:
                xSemaphoreTake(xMutexParams, portMAX_DELAY);
                FormingIntervalArchive(dataManager.event);
                xSemaphoreGive(xMutexParams);
                writeDataToFlashQueue((uint8_t *)&intervalArchive, sizeof(StrIntervalArchive), &intervalArchiveInfo);
                break;

            //обработка события формирования дневного архива
            case DM_DAY_ALARM:
                xSemaphoreTake(xMutexParams, portMAX_DELAY);
                FormingDayArchive(dataManager.event);
                xSemaphoreGive(xMutexParams);
                writeDataToFlashQueue((uint8_t *)&dayArchive, sizeof(StrDayArchive), &dayArchiveInfo);
                break;

            //обработка события формирования месячного архива
            case DM_MONTH_ALARM:
                xSemaphoreTake(xMutexParams, portMAX_DELAY);
                FormingMonthArchive(dataManager.event);
                xSemaphoreGive(xMutexParams);
                writeDataToFlashQueue((uint8_t *)&monthArchive, sizeof(StrMonthArchive), &monthArchiveInfo);
                break;

            //обработка события
            case DM_EVENT:
                xSemaphoreTake(xMutexParams, portMAX_DELAY);
                FormingEventsArchive(dataManager.event);
                xSemaphoreGive(xMutexParams);
                writeDataToFlashQueue((uint8_t *)&eventsArchive, sizeof(StrEventsArchive), &eventsArchiveInfo);
                break;

            //обработка изменеий
            case DM_CHANGE:
                xSemaphoreTake(xMutexParams, portMAX_DELAY);
                FormingChangesArchive(dataManager.event);
                xSemaphoreGive(xMutexParams);
                writeDataToFlashQueue((uint8_t *)&changesArchive, sizeof(StrChangesArchive), &changesArchiveInfo);
                break;

            //обработка изменеий
            case DM_SAVE_PARAMS_IN_FLASH:
                writeParamsToFlash();
                break;
                
            // проверка прошивки
            case DM_CHECK_FLASH_CRC_ERROR:
                checkFlashError();
                break;                
        }

        //уведомляем поток режимов об окончании работы
        if(uxQueueSpacesAvailable(xDataManagerQueue) == QUEUE_DT_MANAGER_SIZE)
        {
            StrEventMode mode;
            mode.status = 0;
            mode.modeCode = DATA_M;
            xQueueSendToBack(xModesQueue, &mode, 0);
        }
    }
}