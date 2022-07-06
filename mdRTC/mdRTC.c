/**
 \file
Файл содержит функции настройки RTC.
*/

/** \defgroup mdRTCGroup mdRTC
 Специализация модуля - работа часами.
*/

#include <time.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mdHardware/mdHardware.h"
#include "mdDataManager/mdDataManager.h"
#include "mdFlashMemory/DataForFlash.h"
#include "mdModes/mdModes.h"
#include "mdRTC/mdRTC.h"
#include "mdTemperSensor/mdTemperSensor.h"

// ошибка библиотеки RTC_Format_BIN работает как BCD формат
#define RTC_FORMAT RTC_Format_BIN

/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция заполняет дату и время RTC в соответствии со struct tm *dateTime.
*/
ErrorStatus setRtcDateTime(struct tm *dateTime)
{
    RTC_TimeTypeDef rtime;
    rtime.RTC_H12 = RTC_HourFormat_24;
    rtime.RTC_Hours = dateTime->tm_hour;
    rtime.RTC_Minutes = dateTime->tm_min;
    rtime.RTC_Seconds = dateTime->tm_sec;

    if(RTC_SetTime(RTC_FORMAT, &rtime) == ERROR)
    {
        return ERROR;
    }

    RTC_WaitForSynchro();
    RTC_DateTypeDef rdate;
    rdate.RTC_Date = dateTime->tm_mday;
    rdate.RTC_Month = dateTime->tm_mon + 1;
    //tm_year отсчитывает с 1900, мы будем считать с 2000 (поэтому минус 100) по 2159
    rdate.RTC_Year = dateTime->tm_year - 100;

    if(dateTime->tm_wday == 0)
    {
        rdate.RTC_WeekDay = 7;
    }
    else
    {
        rdate.RTC_WeekDay = dateTime->tm_wday;
    }

    if(RTC_SetDate(RTC_FORMAT, &rdate) == ERROR)
    {
        return ERROR;
    }

    RTC_WaitForSynchro();
    return SUCCESS;
}

/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция заполняет struct tm *dateTime в соответствии с временем и датой RTC.
*/
void getRtcDateTime(struct tm *dateTime)
{
    RTC_TimeTypeDef rtime;
    RTC_GetTime(RTC_FORMAT, &rtime);
    dateTime->tm_hour = rtime.RTC_Hours;
    dateTime->tm_min = rtime.RTC_Minutes;
    dateTime->tm_sec = rtime.RTC_Seconds;
    RTC_DateTypeDef rdate;
    RTC_GetDate(RTC_FORMAT, &rdate);
    dateTime->tm_mday = rdate.RTC_Date;
    dateTime->tm_mon = rdate.RTC_Month-1;
    //tm_year отсчитывает с 1900, мы будем считать с 2000 (поэтому плюс 100) по 2159
    dateTime->tm_year = rdate.RTC_Year + 100;

    if(rdate.RTC_WeekDay==7)
    {
        dateTime->tm_wday = 0;
    }
    else
    {
        dateTime->tm_wday = rdate.RTC_WeekDay;
    }
}
/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция возвращает текущее время с учетом подсекунд.
*/
double getTimeWithSubSecAfterPoint(void)
{
    struct tm st;
    memset(&st, 0, sizeof(struct tm));
    getRtcDateTime(&st);
    // здесь время усекается, но так как используем RTC до 2159 года максимум, то хватит
    double time = (double)mktime(&st);
    time += (double)((RTC->PRER & 0xff) - RTC->SSR) / ((RTC->PRER & 0xff) + 1);
    return time;
}
/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция настраивает тактирование от HSI.
*/
void InitHSIClock(void)
{
    /* Enable HSI Clock */
    RCC_HSICmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
}

/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция обработчик прерывания от RTC ALARM.
*/
void RTC_Alarm_IRQHandler()
{
    if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
    {
        /*StrEventMode mode;
        mode.status = 1;
        mode.modeCode = DATA_M;
        xQueueSendToBackFromISR(xModesQueue, &mode, NULL);
        // информация для DataManager
        StrDataManager forDataManager;
        forDataManager.cmd = DM_INTERVAL_ALARM;
        xQueueSendToBackFromISR(xDataManagerQueue, &forDataManager, NULL);
        */
        RTC_ClearITPendingBit(RTC_IT_ALRA);
        EXTI_ClearITPendingBit(EXTI_Line17);
    }
}

extern bool fromIRQ;
/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция обработчик прерывания от RTC WKUP.
*/
void RTC_WKUP_IRQHandler(void)
{     
    //upPH2();
    IWDG_ReloadCounter();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_RTCAccessCmd(ENABLE);
    RTC_ClearFlag(RTC_FLAG_WUTF);    
    
    RTC_SetWakeUpCounter(PERIOD_WKUP_TIMER);
    
    static bool flagCycleMeasure = true;
    static struct tm st;    
    getRtcDateTime(&st);
        
    if(!(st.tm_sec % params.measPeriod))
    {
        if(flagCycleMeasure)
        {
            //помещаем информацию о режиме в очередь
            StrEventMode mode;
            mode.status = 1;
            mode.modeCode = DATA_M;
            xQueueSendToBackFromISR(xModesQueue, &mode, NULL);
            // информация для DataManager
            StrDataManager forDataManager;
            forDataManager.cmd = DM_CYCLE_MEASURE;
            xQueueSendToBackFromISR(xDataManagerQueue, &forDataManager, NULL);
            // один раз на этой секунде
            flagCycleMeasure = false;
        }
    }
    else
    {
        flagCycleMeasure = true;
    }
                  
	fromIRQ = true;
    //
    initGpioReedSwitch();
    controlMagnitActivity(30);
    //
    initGpioAnaliseBoxOpen();
    controlBoxOpen(30);
    controlBatBoxOpen(30);
    fromIRQ = false;

    // определяем время задержки
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    int delay = 0;
    switch(rcc_clocks.SYSCLK_Frequency)
    {
        case 2097152:
            delay = 8;
        break;
        case 4194304:
            delay = 20;
        break;
        case 16000000:
            delay = 85;
        break;
    }
    // сбрасываем питание с датчика Холла на определенное время
    initGpioForHallSensorPower();
    
    GPIO_SetBits(HALLSENSOR_POWER_GPIO_PORT, HALLSENSOR_POWER_PIN);
    for(int i=0; i<delay; ++i)
        __NOP();
    GPIO_ResetBits(HALLSENSOR_POWER_GPIO_PORT, HALLSENSOR_POWER_PIN);   
    
    //
    EXTI_ClearITPendingBit(EXTI_Line20);
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    RTC_WakeUpCmd(ENABLE);
    
    // уведомляем диспетчер режима что было прерывание
    StrEventMode mode;
    mode.status = 0;
    mode.modeCode = WKUP_M;
    xQueueSendToBackFromISR(xModesQueue, &mode, NULL);    
}

/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция инициализирует работу RTC.
*/
void RTC_Config(void)
{
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* Allow access to RTC */
    PWR_RTCAccessCmd(ENABLE);
    /* Reset RTC Domain */
    RCC_RTCResetCmd(ENABLE);
    RCC_RTCResetCmd(DISABLE);
    RCC_LSEConfig(RCC_LSE_ON);

    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();
    //
    RTC_InitTypeDef rtc;
    RTC_StructInit(&rtc);
    rtc.RTC_AsynchPrediv = 0x7F;
    rtc.RTC_SynchPrediv	=  0xFF;
    rtc.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&rtc);
    
    if(!params.timeData)
    {
        struct tm dt;
        memset(&dt, 0, sizeof(struct tm));
        dt.tm_hour = 9;
        dt.tm_min = 59;
        dt.tm_sec = 40;
        dt.tm_mday = 1;
        dt.tm_wday = 0;
        dt.tm_mon = 1-1;
        dt.tm_year = 2022 - 1900;
        setRtcDateTime(&dt);
    }    
    else
    {
        struct tm *dt = localtime(&params.timeData);
        setRtcDateTime(dt);
    }
}

/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция настройки RTC и прерываний от будильника и WKUP Timer.
*/
void initRTC(StrCorrectorParams *strCorParams)
{
    RTC_Config();
    initRTCforPeriodCycleMeasureInterrupt(strCorParams);
    initRTCforIntervalAlarmInterrupt(strCorParams);
}
//==============================================================================
