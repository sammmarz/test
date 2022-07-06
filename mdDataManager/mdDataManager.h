#ifndef mdDataManager_h
#define mdDataManager_h

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

typedef enum
{
    DM_CYCLE_MEASURE = 1,
    DM_INTERVAL_ALARM,
    DM_DAY_ALARM,
    DM_MONTH_ALARM,
    DM_EVENT,
    DM_CHANGE,
    DM_SAVE_PARAMS_IN_FLASH,
    DM_CHECK_FLASH_CRC_ERROR
} eCmdDataManager;

typedef struct
{
    uint16_t event;
    eCmdDataManager cmd;
} StrDataManager;

#define QUEUE_DT_MANAGER_SIZE 50
extern QueueHandle_t xDataManagerQueue;
extern TaskHandle_t xTaskDataManager;

void dataControlThread(void *arg);

#endif