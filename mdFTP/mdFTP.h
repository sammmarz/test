#ifndef mdFTP_h
#define mdFTP_h

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define GPRS_INIT_COUNT                (3)                                      // Tries
#define GPRS_INIT_TIMEOUT_TICK         1000                                     // Timeout, ms

extern TaskHandle_t xTaskFTP;
extern QueueHandle_t xFTPcmdQueue;

void TaskFTP(void *arg);
void enableFTP(void);
void disableFTP(void);

int doFTP();                                                                    // Full FTP process
int modem_init();                                                               // Turn on modem and initialize it
bool GPRS_start();                                                              // Connect to GPRS service

#endif