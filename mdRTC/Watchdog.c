#include "mdRTC/mdRTC.h"
#include "mdHardware/mdHardware.h"

// период цикла измерения может быть больше времени обнуления счетчика watchdog
uint16_t remnantCycleMeasurePeriod;
/**
 \ingroup mdRTCGroup
 \author Shuvalov Sergey
 \brief Функция инициализирует Watchdog.
*/
void initWatchdog(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    //WDG сработает через 28 сек (28.339 сек reference manual)
    IWDG_SetReload(0x0FFF);
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_ReloadCounter();
    //запускает счетчик
    IWDG_Enable();
}