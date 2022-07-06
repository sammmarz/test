/**
 \file
Файл содержит функции для работы со статусами и событиями.
*/

#include "Events.h"
#include "Params.h"
#include "mdRTC/mdRTC.h"
#include "mdDataManager/mdDataManager.h"
#include "mdModes/mdModes.h"

extern uint16_t adressLis200;
extern uint16_t instansLis200;

bool fromIRQ;

/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция инициирует запись в архив, если данное событие отслеживается
    триггерами архива
*/
void checkTriggersArchive(StrEvents *trigArchive, uint16_t bitSt, uint8_t numSt, uint16_t event_arc, eCmdDataManager cmd)
{
    // установка проверка отслеживания события в архивах
    for(int trigger = 0; trigger < SUM_TRIGGERS; trigger++)
    {
        if(trigArchive[trigger].events[numSt] & bitSt)
        {
            // инициируем режим работы с данными
            StrEventMode mode;
            mode.status = 1;
            mode.modeCode = DATA_M;
            if(fromIRQ)
            {
                xQueueSendToBackFromISR(xModesQueue, &mode, 0);
            }
            else
            {
                xQueueSendToBack(xModesQueue, &mode, 0);
            }
            //
            StrDataManager forDataManager;
            forDataManager.event = event_arc;
            forDataManager.cmd = cmd;
            if(fromIRQ)
            {
                xQueueSendToBackFromISR(xDataManagerQueue, &forDataManager, NULL);
            }
            else
            {
                xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
            }
            break;
        }
    }
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция производит все необходимые действия при установке события
*/
int setEvent(uint16_t event)
{
    uint16_t event_arc = event;
    uint8_t numSt = event & 0xff;

    //для совместимости со старыми прошивками
    if((event & 0x3000) == 0x3000)
    {
        event &= 0xEF00;
        numSt = 0;
    }

    uint8_t bitSt = (event >> 8) & 0xf;

    if(params.curEvents.events[numSt] & (1<<bitSt))
    {
        // уже установлено
        return 0;
    }

    //в архив прописывает только текущие изменения не глядя на регистр статусов
    if(!(params.curEvents.events[numSt] & (1<<bitSt)) )
    {
        checkTriggersArchive(params.arcIntervSt, (1<<bitSt), numSt, event_arc, DM_INTERVAL_ALARM);
        checkTriggersArchive(params.arcDaySt, (1<<bitSt), numSt, event_arc, DM_DAY_ALARM);
        checkTriggersArchive(params.arcMonthSt, (1<<bitSt), numSt, event_arc, DM_MONTH_ALARM);
        checkTriggersArchive(params.arcEventsSt, (1<<bitSt), numSt, event_arc, DM_EVENT);
        checkTriggersArchive(params.arcChangeSt, (1<<bitSt), numSt, event_arc, DM_CHANGE);
    }

    //установка события в статусе
    params.curEvents.events[numSt] |= (1<<bitSt);
    //установка события в статусном регистре
    params.regEvents.events[numSt] |= (1<<bitSt);
    return 1;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция производит все необходимые действия при снятии события
*/
int resetEvent(uint16_t event)
{
    uint16_t event_arc = event;
    uint8_t numSt = event & 0xff;

    //для совместимости со старыми прошивками
    if((event & 0x3000) == 0x3000)
    {
        event &= 0xEF00;
        numSt = 0;
    }

    uint8_t bitSt = (event >> 8) & 0xf;

    if( !(params.curEvents.events[numSt] & (1<<bitSt)) )
    {
        // событие не прописано в статусах (или уже снято)
        return 0;
    }

    //в архив прописывает только текущие изменения не глядя на регистр статусов
    if((params.curEvents.events[numSt] & (1<<bitSt)) )
    {
        checkTriggersArchive(params.arcIntervSt, (1<<bitSt), numSt, event_arc, DM_INTERVAL_ALARM);
        checkTriggersArchive(params.arcDaySt, (1<<bitSt), numSt, event_arc, DM_DAY_ALARM);
        checkTriggersArchive(params.arcMonthSt, (1<<bitSt), numSt, event_arc, DM_MONTH_ALARM);
        checkTriggersArchive(params.arcEventsSt, (1<<bitSt), numSt, event_arc, DM_EVENT);
        checkTriggersArchive(params.arcChangeSt, (1<<bitSt), numSt, event_arc, DM_CHANGE);
    }

    //сброс события в статусе
    params.curEvents.events[numSt] &= ~(1<<bitSt);
    return 1;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция проверяет фиксирование события в статусном регистре
*/
bool CheckEventInRegisterStatus(uint16_t event)
{
    // проверяем наличие любого установленного события
    if(event == 0xFFFF)
    {
        for(uint8_t st = 0; st < SUM_STATUS_EVENTS; ++st)
        {
            for(uint8_t bit = 0; bit<16; ++bit)
            {
                if(params.regEvents.events[st] & (1<<bit))
                {
                    return true;
                }
            }
        }
    }

    uint8_t numSt = event & 0xff;

    //для совместимости со старыми прошивками
    if((event & 0x3000) == 0x3000)
    {
        event &= 0xEF00;
        numSt = 0;
    }

    uint8_t bitSt = (event >> 8) & 0xf;
    bool status = false;

    if(params.regEvents.events[numSt] & (1<<bitSt))
    {
        status = true;
    }

    return status;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция проверяет фиксирование события в статусах
*/
bool CheckEventInStatus(uint16_t event)
{
    // проверяем наличие любого установленного события
    if(event == 0xFFFF)
    {
        for(uint8_t st = 0; st < SUM_STATUS_EVENTS; ++st)
        {
            for(uint8_t bit = 0; bit<16; ++bit)
            {
                if(params.curEvents.events[st] & (1<<bit))
                {
                    return true;
                }
            }
        }
    }

    uint8_t numSt = event & 0xff;

    //для совместимости со старыми прошивками
    if((event & 0x3000) == 0x3000)
    {
        event &= 0xEF00;
        numSt = 0;
    }

    uint8_t bitSt = (event >> 8) & 0xf;
    bool status = false;

    if(params.curEvents.events[numSt] & (1<<bitSt))
    {
        status = true;
    }

    return status;
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция контролирует вскрытие корпуса прибора
*/
void controlBoxOpen(int countForSignal)
{
    static int countBoxOpen = 1;
    bool status = CheckEventInStatus(EV_BOX_OPEN);
    //вдруг удар и кратковременный отход корпуса от кнопки
    if(GPIO_ReadInputDataBit(BOX_GPIO_PORT, BOX_PIN))
    {
        // тревога пока не установлена
        if((!status) && (countBoxOpen < countForSignal))
        {
            countBoxOpen++;
            if(countBoxOpen == countForSignal)
            {
                setEvent(EV_BOX_OPEN);
                countBoxOpen = 0; // теперь будем считать для снятия тревоги
            }
        }    
        //воздействие магнита, но тревога уже установлена
        else
        {
            countBoxOpen = 0;
        }
    }
    else
    {       
        if(status && (countBoxOpen < countForSignal))
        {
            countBoxOpen++;
            resetEvent(EV_BOX_OPEN);
        }
        //нет воздействия магнита, тревога не установлена
        else
        {
            countBoxOpen = 0;
        }
    }
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция контролирует вскрытие батарейного отсека
*/
void controlBatBoxOpen(int countForSignal)
{
    static int countBoxOpen = 1;

    bool status = CheckEventInStatus(EV_BAT_OPEN);
    RCC_AHBPeriphClockCmd(BATBOX_RCC_PORT, ENABLE);
    //вдруг удар и кратковременный отход корпуса от кнопки
    if(GPIO_ReadInputDataBit(BATBOX_GPIO_PORT, BATBOX_PIN))
    {
        // тревога пока не установлена
        if((!status) && (countBoxOpen < countForSignal))
        {
            countBoxOpen++;
            if(countBoxOpen == countForSignal)
            {
                setEvent(EV_BAT_OPEN);
                countBoxOpen = 0; // теперь будем считать для снятия тревоги
            }
        }    
        //воздействие магнита, но тревога уже установлена
        else
        {
            countBoxOpen = 0;
        }
    }
    else
    {       
        if(status && (countBoxOpen < countForSignal))
        {
            countBoxOpen++;
            resetEvent(EV_BAT_OPEN);
        }
        //нет воздействия магнита, тревога не установлена
        else
        {
            countBoxOpen = 0;
        }
    }
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция контролирует воздействие магнитом на прибор
*/
void controlMagnitActivity(int countForSignal)
{
    static int countMagnit = 1;

    bool status = CheckEventInStatus(EV_MAGNIT);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
    //возможно кратковременное, случайное воздействие
    if(!GPIO_ReadInputDataBit(REED_SWITCH_1_GPIO_PORT, REED_SWITCH_1_PIN) ||
            !GPIO_ReadInputDataBit(REED_SWITCH_2_GPIO_PORT, REED_SWITCH_2_PIN))
    {
        // тревога пока не установлена
        if((!status) && (countMagnit < countForSignal))
        {
            countMagnit++;
            if(countMagnit == countForSignal)
            {
                setEvent(EV_MAGNIT);
                countMagnit = 0; // теперь будем считать для снятия тревоги
            }
        }    
        //воздействие магнита, но тревога уже установлена
        else
        {
            countMagnit = 0;
        }
    }
    else
    {       
        if(status && (countMagnit < countForSignal))
        {
            countMagnit++;
            resetEvent(EV_MAGNIT);
        }
        //нет воздействия магнита, тревога не установлена
        else
        {
            countMagnit = 0;
        }
    }
}
