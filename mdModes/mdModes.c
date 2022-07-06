/**
 \file
Файл содержит функции для контроля активности режимов
*/

/** \defgroup mdModesGroup mdModes
 Модуль отслеживает активные действующие потоки. В случае простоя - инициирует уход в режим "Stop".
*/
#include "FreeRTOS.h"
#include "timers.h"
#include "mdModes.h"
#include "mdPower/mdPower.h"
#include "mdPrintToUART/printToUART.h"
#include "Params/Params.h"

#define KOEF_TIME ((double)108000.) /* 30(частота) * 60(секунды) * 60(минуты) */
QueueHandle_t xModesQueue;
TaskHandle_t xTaskCheckModes;
TimerHandle_t xTimerModes;
static int modes; //содержит флаги об активности режимов
static int tikDisplayM;
static int tikOpticalM;
static int tikFTPM;
static int tikFlashM;
extern unsigned int tikTimer;
extern volatile int timeOUTprogramming;
void controlTimeOut(int mlSec);
//==============================================================================
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция-обработчик таймера для анализа времени выполнения режима.
*/
void timerForMode(int mode, int *tik)
{
    if(modes & mode)
    {
        (*tik)++;
    }
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief режим окончен - вычитаем из оставшегося времени батареи.
*/
void countRemainBatTime(int mode)
{
    double *batPowerVolume = 0;
    uint8_t *batLiveTime = 0;
    if(activeBat == 1)
    {
        batPowerVolume = &params.volBat1;
        batLiveTime = &params.lifeBat1;
    }
    else
    {
        batPowerVolume = &params.volBat2;
        batLiveTime = &params.lifeBat2;
    }
    
    switch(mode)
    {
        case DISPLAY_M:
            *batPowerVolume -= tikDisplayM / KOEF_TIME * 1000; // 1000 mkrA for Display mode
            tikDisplayM = 0;
        break;
        case OPTICAL_M:
            *batPowerVolume -= tikOpticalM / KOEF_TIME * 1000; // 
            tikOpticalM = 0;
        break;
        case FTP_M:
            *batPowerVolume -= tikFTPM / KOEF_TIME * 70000; // 
            tikFTPM = 0;
        break;
        case FLASH_M:
            *batPowerVolume -= tikFlashM / KOEF_TIME * 10000; // 
            tikFlashM = 0;
        break;
        case WKUP_M:
            *batPowerVolume -= 1 / KOEF_TIME * 130; // 130 - теоретический средний штатный расход
        break;
        default:
        break;
    }
    if(*batPowerVolume < (ALL_BAT_VOL / TOUR_BAT))
    {
        *batPowerVolume = ALL_BAT_VOL / TOUR_BAT;
    }
    *batLiveTime = (uint8_t)(*batPowerVolume / (ALL_BAT_VOL / TOUR_BAT)); 
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция-обработчик таймера для анализа времени выполнения каждого режима.
*/
void TimerModesFunction(TimerHandle_t xTimer)
{
    // контроль таймаутов при перепрограммировании
    if(timeOUTprogramming)
    {
        controlTimeOut(100);
    }
}
/**
 \ingroup mdModesGroup
 \author Shuvalov Sergey
 \brief Основная функция обратного вызова модуля.

 Функция в бесконечном цикле ожидает информацию об активности режимов от других модулей.
 В случае если все модули отработали (не активны) функция инициирует переход
 в режим пониженного энергопотребления.
*/
void CheckModes(void *arg)
{
    StrEventMode modeStr;   

    if(!xTimerModes)
    {
        xTimerModes = xTimerCreate("TimerModes", 100, pdTRUE, NULL, TimerModesFunction);
        xTimerStart( xTimerModes, 0 );
    }

    while(1)
    {
        xQueueReceive(xModesQueue, &modeStr, (TickType_t) portMAX_DELAY);

        if(!modeStr.status)
        {
            //работа по данному режиму окончена
            modes &= ~modeStr.modeCode;
            // инкрементируем счётчики времени для активных режимов по WKUP таймеру
            if(WKUP_M == modeStr.modeCode)
            {
                timerForMode(DISPLAY_M, &tikDisplayM);
                timerForMode(OPTICAL_M, &tikOpticalM);
                timerForMode(FTP_M, &tikFTPM);
                timerForMode(FLASH_M, &tikFlashM);
            }
            countRemainBatTime(modeStr.modeCode);            
        }
        else
        {
            //отмечаем активный режим
            modes |= modeStr.modeCode;
            //printf("m1: %x\r\n", modes);
        }

        //если нет активных режимов переходим к пониженному энергопотреблению
        if(uxQueueSpacesAvailable(xModesQueue) == QUEUE_MODES_SIZE)
        if(!modes)
        {
            LowPowerON();
            //========================================================================
            //================== здесь режим STOP, ждём прерываний ===================
            //========================================================================
            //произошло прерывание переходим к нормальному режиму работы
            LowPowerOFF();
            tikTimer = 0;            
        }
    }
}
//==============================================================================

