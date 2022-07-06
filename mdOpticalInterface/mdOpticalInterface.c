/**
 \file
Файл содержит функции настройки RTC.
*/

/** \defgroup mdOpticalInterfaceGroup mdOpticalInterface
 Специализация модуля - работа по оптическому интерфейсу.
При отсутствии данных в канале 60 сек. - отключаем интерфейс (как и в GPRS)
*/
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "mdModes/mdModes.h"
#include "mdHardware/mdHardware.h"
#include "mdDataManager/mdDataManager.h"
#include "../mdLIS200/mdLis200.h"
#include "../mdDisplay/mdDisplay.h"

//define INVERS_OPT_PIN

extern bool optPortStatus;
extern QueueHandle_t xBtnsQueue;
extern SemaphoreHandle_t xMutexParams;

void initOpticalInterface(void);
void stopControlTimeOut(void);
extern int exchangeRegim;

bool firstOptInterfByteExchange;
TaskHandle_t xTaskOpticalInterface;
xQueueHandle xOptBufQueue;
StrDataUART optData;
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция посылает данные через USART1.
*/
void sendDataUsart1(uint8_t *buf, int len)
{
    while (len)
    {
        USART_SendData(USART1, *buf++);

        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
            ;

        len--;
    }
}
//otlad
#include "../mdDisplay/Fonts5x8.h"
extern double temperature;
CHAR_TYPE optStr[148];
int indx;
// otlad end

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отправляет данные по оптическому интерфейсу
*/
void sendDataWithOpticalInterface(void)
{
    //отключаем прерывания на прием
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    //
    sendDataUsart1((uint8_t *)optData.bufTX, strlen(optData.bufTX) + optData.BCCisZERO);
    optData.BCCisZERO = 0;
    
    // формируем событие начала передачи по оптике
    if(!firstOptInterfByteExchange)
    {
        setEvent(EV_OPT_DATA_START);
        firstOptInterfByteExchange = true;
    }
    
    //ждем когда последние данные пройдут в канале и вкл. прерывания на прием
    //int count = 0;
    while (USART_GetFlagStatus(USART1, USART_IT_IDLE) == RESET)
    {
        if(!optPortStatus)
        {
            break;
        }
    }
    
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Основная функция обратного вызова модуля.
 Функция в бесконечном цикле ожидает прихода байта.

 В случае бездействия 1 мин - завершении режима работы с оптикой.
 Задержка перед разрешением прерываний по RX, так как приемный датчик оптики реагирует при передаче
*/
void opticalInterfaceTask(void *arg)
{
    uint8_t byte;
    bool needSendTXbuf = false;

    while(1)
    {
        //обработка пришедшего байта
        while( xQueueReceive(xOptBufQueue, &byte, (TickType_t) params.timeOutForOpticIntOff ) == pdTRUE )
        {
            if(!optPortStatus)
            {
                break;
            }

            needSendTXbuf = analyseReceivedByteUARTasLis200(byte, &optData);

            if(needSendTXbuf)
            {
                sendDataWithOpticalInterface();
                needSendTXbuf = false;
            }
        }

        exchangeRegim = 0;
        stopControlTimeOut();
        // снова переключаемся на скорость 300
        initOpticalInterface();
        // отображаем отключение оптики на экране
        optPortStatus = false;
        firstOptInterfByteExchange = false;
        xSemaphoreTake(xMutexParams, portMAX_DELAY);
        resetEvent(EV_OPT_READY);
        resetEvent(EV_OPT_DATA_START);
        xSemaphoreGive(xMutexParams);
        StrEventForDisplay cmdUpdate;

        if(displayActive)
        {
            cmdUpdate.status = 1;
            cmdUpdate.codeEventForDisplay = UPDATE_DISPLAY;
            xQueueSendToBack(xBtnsQueue, &cmdUpdate, NULL);
        }

        //уведомляем поток режимов, об окончании работы с оптикой
        StrEventMode modeEvent;
        modeEvent.status = 0;
        modeEvent.modeCode = OPTICAL_M;
        xQueueSendToBack(xModesQueue, &modeEvent, 0);
    }
}
/**
 \ingroup mdOpticalInterfaceGroup
 \author Shuvalov Sergey
 \brief Функция обработчик прерывания оптического интерфейса.
*/
void USART1_IRQHandler()
{
    //читаем статус
    uint16_t status = USART1->SR;
    //читаем байт (чтение статуса и данных сбрасывает флаг ORE)
    uint8_t byte = USART1->DR & (uint16_t) 0x01FF;

    //если контроль чётности и длина слова 8 то маскируем бит чётности
    if ((USART1->CR1 & USART_CR1_PCE))
        if (!(USART3->CR1 & USART_CR1_M))
        {
            byte &= 0x7F;
        }

    if ((status & USART_FLAG_RXNE) && optPortStatus)
    {
        //пришли данные
        xQueueSendToBackFromISR(xOptBufQueue, &byte, NULL);
        //помещаем информацию о режиме в очередь
        StrEventMode modeEvent;
        modeEvent.status = 1;
        modeEvent.modeCode = OPTICAL_M;
        xQueueSendToBackFromISR(xModesQueue, &modeEvent, NULL);
    }
}
/**
 \ingroup mdOpticalInterfaceGroup
 \author Shuvalov Sergey
 \brief Функция инициализации работы оптического интерфейса.
*/
void initOpticalInterface(void)
{
    initGpioForOpticalInterface();
    initUSART_1(300, USART_Parity_Even);
    initOpticalInterfaceInterrupt();
}
/**
 \ingroup mdOpticalInterfaceGroup
 \author Shuvalov Sergey
 \brief Функция устанавливает сигнал GPIO pin для запитывания оптики
*/
void enableOptPower(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
#ifdef INVERS_OPT_PIN
    GPIO_ResetBits(OPT_GPIO_PORT, OPT_ENABLE_PIN);
#else
    GPIO_SetBits(OPT_GPIO_PORT, OPT_ENABLE_PIN);
#endif
}
/**
 \ingroup mdOpticalInterfaceGroup
 \author Shuvalov Sergey
 \brief Функция снимает сигнал GPIO pin для снятия питания оптики
*/
void disableOptPower(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
#ifdef INVERS_OPT_PIN
    GPIO_SetBits(OPT_GPIO_PORT, OPT_ENABLE_PIN);
#else
    GPIO_ResetBits(OPT_GPIO_PORT, OPT_ENABLE_PIN);
#endif
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}