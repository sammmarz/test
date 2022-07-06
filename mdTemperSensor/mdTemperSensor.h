#ifndef mdTemperSensor_h
#define mdTemperSensor_h

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define TEMPER_START_CONVERSION 6
#define TEMPER_DATA_READY 5


void initMCUforTemperSensor(void);
void enableTemperSensor(void);
void disableTemperSensor(void);
int configTemperSensor(void);
void startConversionTemperSensor(void);
int getTemperFromSensor(double *temp, int16_t *tempBin);
float getMCUTemperSensorValue(void);
void TemperSensorTask(void *arg);

extern TaskHandle_t xTemperSensor;
extern QueueHandle_t xTemperSensorQueue;
extern int temperCountError;
#endif