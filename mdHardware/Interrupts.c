/**
 \file
Файл настройки прерываний.
Приоритеты прерываний NVIC с 0-4 заняты RTOS поэтому нужно использовать 5-15
(15 - низший приоритет)
*/

#include "mdHardware.h"
#include "mdDisplay/mdDisplay.h"
#include "mdModes/mdModes.h"
#include "mdRTC/mdRTC.h"

#define BTNS_PUSH_IRQ_PRIORITY 8
#define TEMPER_SENSOR_IRQ_PRIORITY 9
#define HALL_SENSOR_IRQ_PRIORITY 8
#define OPT_INTERF_IRQ_PRIORITY 13
#define MODEM_IRQ_PRIORITY 13
#define RTC_WKUP_IRQ_PRIORITY 11
#define RTC_ALARMA_IRQ_PRIORITY 11

/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации прерываний для кнопок
*/
void initUserButtonsInterrupts(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    // тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    NVIC_DisableIRQ(EXTI15_10_IRQn);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource11);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);
    EXTI_ClearITPendingBit(EXTI_Line11);
    EXTI_ClearITPendingBit(EXTI_Line12);
    EXTI_ClearITPendingBit(EXTI_Line13);
    //настройка линий прерываний
    EXTI_InitStructure.EXTI_Line = EXTI_Line11 | EXTI_Line12 | EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    //разрешение прерываний и установка приоритета
    NVIC_SetPriority(EXTI15_10_IRQn, BTNS_PUSH_IRQ_PRIORITY);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации прерываний от датчика температуры TEMP ALERT
*/
void initTemperSensorInterrupt(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    // тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    //настройка линий прерываний
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    //разрешение прерываний и установка приоритета
    NVIC_SetPriority(EXTI9_5_IRQn, TEMPER_SENSOR_IRQ_PRIORITY);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации прерываний при вскрытии корпуса
*/
void initControlBoxOpenInterrupt(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    // тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    //настройка линий прерываний
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
    EXTI_InitStructure.EXTI_Line = EXTI_Line15;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    //разрешение прерываний и установка приоритета
    NVIC_SetPriority(EXTI15_10_IRQn, BTNS_PUSH_IRQ_PRIORITY);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации прерываний при вскрытии корпуса
*/
void initControlBatBoxOpenInterrupt(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    // тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    //настройка линий прерываний
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    //разрешение прерываний и установка приоритета
    NVIC_SetPriority(EXTI2_IRQn, BTNS_PUSH_IRQ_PRIORITY);
    NVIC_EnableIRQ(EXTI2_IRQn);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации прерываний для датчиков Холла
*/
void initHallSensorsInterrupts(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    // тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    //настройка линий прерываний
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    //разрешение прерываний и установка приоритета
    NVIC_SetPriority(EXTI0_IRQn, HALL_SENSOR_IRQ_PRIORITY);
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_SetPriority(EXTI1_IRQn, HALL_SENSOR_IRQ_PRIORITY);
    NVIC_EnableIRQ(EXTI1_IRQn);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации прерываний для оптического интерфейса
*/
void initOpticalInterfaceInterrupt(void)
{
    //настройка NVIC прерывания по RX
    NVIC_SetPriority(USART1_IRQn, OPT_INTERF_IRQ_PRIORITY);
    NVIC_EnableIRQ(USART1_IRQn);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации прерываний для работы с модемом
*/
void initModemInterrupt(void)
{
    //настройка NVIC прерывания по RX
    NVIC_SetPriority(USART3_IRQn, MODEM_IRQ_PRIORITY);
    NVIC_EnableIRQ(USART3_IRQn);
    //NVIC_SetPriority(DMA1_Channel2_IRQn, PERIRH_INTERRUPT_PRIORITY);          // DMA
    //NVIC_EnableIRQ(DMA1_Channel2_IRQn);                                       // DMA
    rtsUp_usart3();
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция настраивает работу wkup timer(для работы по циклам измерений).
 Настройка выполняется в соответствии с параметризацией корректора.
*/
void initRTCforPeriodCycleMeasureInterrupt(StrCorrectorParams *strCorParams)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_ClearITPendingBit(EXTI_Line20);
    EXTI_InitStructure.EXTI_Line = EXTI_Line20;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_SetPriority(RTC_WKUP_IRQn, RTC_WKUP_IRQ_PRIORITY);
    NVIC_EnableIRQ(RTC_WKUP_IRQn);
    //выставляем частоту 1 Гц
    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);//(RTC_WakeUpClock_CK_SPRE_16bits);
    RTC_WakeUpCmd(DISABLE);    
    RTC_SetWakeUpCounter(PERIOD_WKUP_TIMER);
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    RTC_WakeUpCmd(ENABLE);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция настраивает работу будильникa(для записи интервального архива).
 Настройка выполняется в соответствии с параметризацией корректора.
*/
void initRTCforIntervalAlarmInterrupt(StrCorrectorParams *strCorParams)
{
    /* EXTI_InitTypeDef EXTI_InitStructure;
     EXTI_ClearITPendingBit(EXTI_Line17);
     EXTI_InitStructure.EXTI_Line = EXTI_Line17;
     EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
     EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
     EXTI_InitStructure.EXTI_LineCmd = ENABLE;
     EXTI_Init(&EXTI_InitStructure);
     NVIC_SetPriority(RTC_Alarm_IRQn, RTC_ALARMA_IRQ_PRIORITY);
     NVIC_EnableIRQ(RTC_Alarm_IRQn);
     //alarm_A
     RTC_TimeTypeDef alarmTime;
     uint32_t alarmMask = 0;

     switch(strCorParams->periodInterval)
     {
         case INTERVAL_HOUR:
             alarmMask = RTC_AlarmMask_DateWeekDay & ~RTC_AlarmMask_Hours;
             break;

         case INTERVAL_DAY:
             alarmMask = RTC_AlarmMask_DateWeekDay;
             break;

         case INTERVAL_MONTH:
             alarmMask = 0;
             break;
     }

     alarmTime.RTC_H12 = RTC_HourFormat_24;
     alarmTime.RTC_Hours = strCorParams->dayBegin;
     alarmTime.RTC_Minutes = 00;
     alarmTime.RTC_Seconds = 00;
     RTC_AlarmTypeDef alarm;
     RTC_AlarmStructInit(&alarm);
     alarm.RTC_AlarmTime = alarmTime;
     alarm.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date; // даты 1-31
     alarm.RTC_AlarmDateWeekDay = strCorParams->dateIntervalBegin;
     alarm.RTC_AlarmMask = alarmMask;
     RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &alarm);
     RTC_ITConfig(RTC_IT_ALRA, ENABLE);
     RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
     RTC_ClearFlag(RTC_FLAG_ALRAF);
    */
}

void doLogicStatusEXTI(uint32_t extiLine, int btnNum);

/**
 \ingroup mdDebugGroup
 \author Shuvalov Sergey
 \brief Функция обработчик прерывания нажатия кнопки на плате, анализ вскрытия корпуса.
*/
void EXTI15_10_IRQHandler(void)
{  
    if(EXTI_GetITStatus(EXTI_Line11) != RESET)
    {
        doLogicStatusEXTI(EXTI_Line11, LEFT_BTN);
    }

    if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        doLogicStatusEXTI(EXTI_Line12, CENTER_BTN);
    }

    if(EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        doLogicStatusEXTI(EXTI_Line13, RIGHT_BTN);
    }

    if(EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        //StrEventForDisplay cmdUpdate;

        /*if(controlBoxOpen() && displayActive)
        {
            cmdUpdate.status = 1;
            cmdUpdate.codeEventForDisplay = UPDATE_DISPLAY;
            xQueueSendToBackFromISR(xBtnsQueue, &cmdUpdate, NULL);
        }
*/
        StrEventMode mode;
        mode.status = 0;
        mode.modeCode = EMPTY_M;
        xQueueSendToBackFromISR(xModesQueue, &mode, NULL);
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}
/**
 \ingroup mdDebugGroup
 \author Shuvalov Sergey
 \brief Пока не используется.
*/
void EXTI2_IRQHandler(void)
{
    //StrEventForDisplay cmdUpdate;
/*
    if(controlBatBoxOpen() && displayActive)
    {
        cmdUpdate.status = 1;
        cmdUpdate.codeEventForDisplay = UPDATE_DISPLAY;
        xQueueSendToBackFromISR(xBtnsQueue, &cmdUpdate, NULL);
    }
*/
    StrEventMode mode;
    mode.status = 0;
    mode.modeCode = EMPTY_M;
    xQueueSendToBackFromISR(xModesQueue, &mode, NULL);
    EXTI_ClearITPendingBit(EXTI_Line2);
}