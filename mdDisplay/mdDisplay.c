/**
 \file
Файл содержит функции для обработки нажатия кнопок пользователем.
*/

/** \defgroup mdDisplayGroup mdDisplay
 Специализация модуля - обработка нажатия кнопок пользователем, работа с навигацией и дисплеем.
*/

#include <stdio.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"
#include "../mdModes/mdModes.h"
#include "mdDisplay.h"
#include "DisplayFunctions.h"
#include "Navigate.h"
#include "mdHardware/mdHardware.h"
#include "mdDisplay/DisplayHardware.h"
// otlad
#include "mdTemperSensor/mdTemperSensor.h"
#include "mdTemperSensor/HardwareTemperSensor.h"
#include <string.h>
double temperature;
#include "../mdDisplay/Fonts5x8.h"
extern CHAR_TYPE optStr[148];
// otlad end

extern SemaphoreHandle_t xMutexParams;
#define TIMER_PERIOD 100

int32_t maxPushBtnWait = 60000;

//в случае периодического обновления экрана - не уйдем в режим LPM
//displayUpdatePeriod - нужно для принудительного выхода в LPM
int32_t displayUpdatePeriod = 60000 / TIMER_PERIOD;

extern bool acceptNewTourBat;
extern xSemaphoreHandle xSPI1semaphore;
QueueHandle_t xBtnsQueue;
TaskHandle_t xTaskDisplayDriver;
TimerHandle_t xTimer;
unsigned int tikTimer;
static unsigned int timePush;
int cmd;
int lastBtnPush;

static unsigned int cmdTime; // без этого команда начинает срабатывать сама по себе или подряд
bool displayActive;

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Обновление времени ожидания до отключения дисплея.
*/
void updateLcdTimeOff(void)
{    
    int lcdTimeOff;
    
    xSemaphoreTake(xMutexParams, portMAX_DELAY);
    lcdTimeOff = params.lcdTimeOff;
    xSemaphoreGive(xMutexParams);
    
    if((lcdTimeOff >= 20) && (lcdTimeOff <= 300))
    {
        maxPushBtnWait = lcdTimeOff * 1000;
    }
    else
    {
        maxPushBtnWait = 60000;
    }
    displayUpdatePeriod = maxPushBtnWait / TIMER_PERIOD;
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция-обработчик таймера (инкремент tikTimer).
*/
void TimerFunction(TimerHandle_t xTimer)
{
    StrEventForDisplay cmdUpdate;

    if((cmdTime) && (tikTimer > cmdTime))
    {
        cmdTime = 0;
        cmd = lastBtnPush;
        cmdUpdate.status = 1;
        cmdUpdate.codeEventForDisplay = UPDATE_DISPLAY;
        xQueueSendToBack(xBtnsQueue, &cmdUpdate, NULL);
    }

    //сброс watchdog
    if((tikTimer % 20) == 19)
    {
        //IWDG_ReloadCounter();
        // otlad на всякий активируем диспетчер режимов, чтобы не зависать
        StrEventMode mode;
        mode.status = 0;
        mode.modeCode = EMPTY_M;
        xQueueSendToBack(xModesQueue, &mode, NULL);
    }

    tikTimer++;
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция запуска таймера(период 100 млСек) для отсчёта времен нажатия кнопок.
*/
void startTimer(void)
{
    if(!xTimer)
    {
        xTimer = xTimerCreate("Timer", TIMER_PERIOD, pdTRUE, NULL, TimerFunction);
        xTimerStart( xTimer, 0 );
    }
}
//==============================================================================
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Основная функция обратного вызова модуля.
 Функция в бесконечном цикле ожидает информацию о нажатии кнопки.
 Позиционирует в меню и отображает информацию
 В случае бездействия 1 мин - отключение дисплея и уведомление mdModes о
 завершении режима работы с дисплеем.
*/
void DisplayDriver(void *arg)
{
    StrEventForDisplay btnStr;
    //запускаем таймер для контроля времени нажатия кнопок
    startTimer();
    tikTimer = 0;
    timePush = 0;

    while(1)
    {
        //отображение начинается с первого экрана
        firstScreen = 1;

        // tikTimer = 0;
        //узнаем информацию о нажатой\отпущенной кнопке
        while( xQueueReceive(xBtnsQueue, &btnStr, (TickType_t) maxPushBtnWait ) == pdTRUE )
        {
            if(!displayActive)
            {
                xSemaphoreTake(xSPI1semaphore, portMAX_DELAY);
                initGpioForDisplaySPI();
                initGpioForDisplayCommon();
                initGpioForDisplayPower();
                initSPI_2();
                xSemaphoreGive(xSPI1semaphore);
                enableDisplayPower();
                initDisplay();
                vTaskDelay(1);
                displayActive = true;
            }
            
            // анализируем состояние пина кнопки
            if (btnStr.codeEventForDisplay & (CENTER_BTN | RIGHT_BTN | LEFT_BTN))
            {       
                vTaskDelay(10);    // защита от дребезга 
                uint32_t statusPin = GPIOB->IDR;
                
                switch(btnStr.codeEventForDisplay)
                {                     
                    case CENTER_BTN:
                    statusPin = GPIO_Pin_12 & statusPin;                    
                    break;
                    case RIGHT_BTN:
                    statusPin = GPIO_Pin_13 & statusPin;                    
                    break;
                    case LEFT_BTN:
                    statusPin = GPIO_Pin_11 & statusPin;
                    break;
                }
                // отпустили кнопку, подождали, очистили очередь от сигналов шума\дребезга
                if(statusPin)
                {
                    xQueueReset(xBtnsQueue);
                    btnStr.status = 0;
                }else
                {
                    btnStr.status = 1;
                }                      
            }

            //здесь логика работы с кнопками (позиционирование в меню)
            switch(btnStr.codeEventForDisplay)
            {
                case CENTER_BTN:
                    if(btnStr.status)
                    {
                        // нажали кнопку
                        cmdTime = tikTimer + 20;
                        timePush = tikTimer;
                        lastBtnPush = CENTER_BTN;
                    }
                    else
                    {
                        if(!cmdTime)
                        {
                            //уже сработала команда по таймеру
                            break;
                        }

                        cmdTime = 0;
                        navigate(CENTER_BTN, 0);
                    }

                    break;

                case RIGHT_BTN:
                    if(btnStr.status)
                    {
                        // нажали кнопку
                        cmdTime = tikTimer + 20;
                        timePush = tikTimer;
                        lastBtnPush = RIGHT_BTN;
                    }
                    else
                    {
                        if(!cmdTime)
                        {
                            //уже сработала команда по таймеру
                            break;
                        }
                        
                        cmdTime = 0;
                        navigate(RIGHT_BTN, 0);
                    }
                    break;

                case LEFT_BTN:
                    if(btnStr.status)
                    {
                        // нажали кнопку
                        cmdTime = tikTimer + 20;
                        timePush = tikTimer;
                        lastBtnPush = LEFT_BTN;
                    }
                    else
                    {
                        if(!cmdTime)
                        {
                            //уже сработала команда по таймеру
                            break;
                        }
                        
                        cmdTime = 0;
                        navigate(LEFT_BTN, 0);
                    }
                    break;

                case UPDATE_DISPLAY:
                    updateLcdTimeOff();
                    navigate(NULL, UPDATE_DISPLAY | cmd);
                    cmd = 0;
                    break;                   
                    
                default:
                    break;
            }

            if((tikTimer - timePush) > displayUpdatePeriod)
            {
                break;
            }
        }

        tikTimer = 0;
        timePush = 0;
        cmdTime = 0;
        //отключаем дисплей
        turnOffDisplay();
        disableDisplayPower();
        displayActive = false;
        // выходим из меню подтверждения установки новой батареи
        acceptNewTourBat = false;
        //уведомляем поток режимов, об окончании работы пользователя с дисплеем
        StrEventMode modeBTN;
        modeBTN.status = 0;
        modeBTN.modeCode = DISPLAY_M;
        xQueueSendToBack(xModesQueue, &modeBTN, 0);
    }
}
//==============================================================================