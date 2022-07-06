#ifndef mdModes
#define mdModes

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define QUEUE_MODES_SIZE 10
typedef struct
{
    int status;
    int modeCode;
} StrEventMode;

enum eModesCode
{
    DISPLAY_M = (1<<0),
    METERS_M = (1<<1),
    OPTICAL_M = (1<<2),
    FLASH_M = (1<<3),
    LOGGING_M = (1<<4),
    DATA_M = (1<<5),   //требуется обработка данных
    FTP_M = (1<<6),
    EMPTY_M = (1<<7), //для активация потока режимов, чтобы быстро уйти в LPM
    WKUP_M = (1<<8)
};

extern int modes; //содержит флаги об активности режимов

extern QueueHandle_t xModesQueue;
extern TaskHandle_t xTaskCheckModes;

void CheckModes(void *arg);

#endif