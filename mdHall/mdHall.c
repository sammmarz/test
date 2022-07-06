/**
 \file
Файл содержит функции для работы датчиков Холла.
*/

/** \defgroup mdHallGroup mdHall
 Специализация модуля - обеспечение и анализ работы датчиков Холла.
*/

#include "mdHardware/mdHardware.h"
#include "mdPower/mdPower.h"
#include "mdRTC/mdRTC.h"

double timeBetweenImp;
double impHallTimeNew;
double impHallTimeOld;

/**
 \ingroup mdHallGroup
 \author Shuvalov Sergey
 \brief Функция подготавливает датчики Холла к работе.
*/
void initHallSensors(void)
{
    initGpioForHallSensorPower();
    GPIO_ResetBits(HALLSENSOR_POWER_GPIO_PORT, HALLSENSOR_POWER_PIN);
    initGpioForHallSensors();
    initHallSensorsInterrupts();
    //оставляем конфигурацию пинов для LPM
    setMaskForLPM(HALLSENSOR_POWER_GPIO_PORT, HALLSENSOR_POWER_PIN);
    setMaskForLPM(HALLSENSOR_GPIO_PORT, HALLSENSOR_A_PIN);
}

/**
 \ingroup mdDebugGroup
 \author Shuvalov Sergey
 \brief Функция определяет вызвано ли прерывание данным пином.
        Если да, то определяет статус пина и анализирует работу датчиков Холла.
*/
void analyzeHALLinterupt(uint32_t extiLine, uint16_t gpioPin, uint32_t *counterHall_X)
{
    if(EXTI_GetITStatus(extiLine) != RESET)
    {
        //высокий уровень - нормальное состояние, низкий - воздействие магнита
        if(GPIO_ReadInputDataBit(HALLSENSOR_GPIO_PORT, gpioPin))
        {
            (*counterHall_X)++;
        }
        else
        {
            // пока не учитываем задний фронт импульса
        }

        EXTI_ClearITPendingBit(extiLine);
    }
}
/**
 \ingroup mdDebugGroup
 \author Shuvalov Sergey
 \brief Функция обработчик прерывания импульса датчика Холла А.
*/
void EXTI0_IRQHandler(void)
{
    analyzeHALLinterupt(EXTI_Line0, HALLSENSOR_A_PIN, &params.counterHall_A);
    // измерение времени между импульсами
    timeBetweenImp = 0;
    double tempTime = 0;
    if(impHallTimeOld != 0)
    {        
        tempTime = impHallTimeNew;
        impHallTimeNew = getTimeWithSubSecAfterPoint();
        timeBetweenImp  = impHallTimeNew - impHallTimeOld;
    }
    impHallTimeOld = tempTime;
}
