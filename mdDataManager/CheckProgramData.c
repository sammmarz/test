/**
\file
В файле организованны алгоритмы проверки файла прошива лежащего во внешней флэш
*/

#include "FreeRTOS.h"
#include "semphr.h"
#include "mdDataManager/mdDataManager.h"
#include "mdDataManager/CheckProgramData.h"
#include "mdModes/mdModes.h"
#include "mdFlashMemory/DataForFlash.h"
#include "mdFlashMemory/mdFlashMemory.h"
#include "Params/Params.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

extern SemaphoreHandle_t xMutexParams;

/**
\ingroup mdDataManagerGroup
\author Shuvalov Sergey
\brief Функция проверяет проверяет, что все данные были записаны во флеш,
    если нет, то ждёт 30 млСек - затем вызывает сама себя.
    После того как данные были записаны проверяет - были ли ошибки в процессе записи.
*/
void checkFlashError(void)
{
    // проверяем, что все ли данные записаны во флеш
    if(uxQueueSpacesAvailable(xFlashWriteQueue) != QUEUE_FLASH_SIZE)
    {
        xSemaphoreTake (xMutexParams, portMAX_DELAY);
        params.RegPr |= PROG_ANALYSE_PROCESS;
        xSemaphoreGive(xMutexParams);        
        // информация для DataManager
        StrDataManager forDataManager;
        forDataManager.cmd = DM_CHECK_FLASH_CRC_ERROR;
        xQueueSendToBackFromISR(xDataManagerQueue, &forDataManager, NULL);
        vTaskDelay(30);        
    }
    else
    {
        xSemaphoreTake (xMutexParams, portMAX_DELAY);        
        if(!isCrcFlashError())
        {
            // не было ошибок записи во флэш в процессе передачи
            params.RegPr |= PROG_IN_FLASH_OK;
        }
        params.RegPr &= ~PROG_ANALYSE_PROCESS;
        xSemaphoreGive(xMutexParams);
    }
}