#ifndef mdFlashMemory
#define mdFlashMemory

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "DataForFlash.h"

// данные, которые нужно переложить во флеш
typedef struct
{
    StrFlashInfoData *dataInfo;
    uint8_t data[256];
    int length;  // количество байт данных
} StrQueueInfoData;

extern TaskHandle_t xTaskWriteBufferToFlash;
extern xSemaphoreHandle flashSemaphore;
#define QUEUE_FLASH_SIZE 26
extern QueueHandle_t xFlashWriteQueue;

void WriteToFlashThread(void *arg);
void writeDataToFlashQueue(uint8_t *buf, int length, StrFlashInfoData *dataInfo);
void readDataFromFlash(uint8_t *buf, int length, int adress);
void eraseFlashMemory(uint32_t beg, uint32_t end);
void resetCrcFlashError(void);
bool isCrcFlashError(void);
#endif