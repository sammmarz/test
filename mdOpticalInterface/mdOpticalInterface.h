#ifndef mdOpticalInterface
#define mdOpticalInterface

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern bool firstOptInterfByteExchange;
extern TaskHandle_t xTaskOpticalInterface;
extern xQueueHandle xOptBufQueue;
void opticalInterfaceTask(void *arg);
void initOpticalInterface(void);
void enableOptPower(void);
void disableOptPower(void);
void sendDataWithOpticalInterface(void);

#endif