/**
 \file
Файл содержит функции для обмена данными по FTP.
*/

/** \defgroup mdFTPGroup mdFTP
 Специализация модуля - обеспечение обмена данными по FTP.
*/
#include <stdbool.h>
#include "mdFTP.h"
#include "driver_GL865.h"                                                       // Driver for TELIT GL865-DUAL V3.1
#include "../mdModes/mdModes.h"
#include "../mdDisplay/mdDisplay.h"
extern bool gprsStatus;                                                         // GPRS global status

TaskHandle_t xTaskFTP;
QueueHandle_t xFTPcmdQueue;

void (*modem_powerOn)();
void (*modem_powerOff)();
void (*modem_initPins)();
int (*modem_check)();
bool (*GPRS_enable)();
void (*GPRS_stop)();
int (*FTP_client)(long *);

/**
 \ingroup mdFTPGroup
 \author Shuvalov Sergey
 \brief Функция ожидает команду на передачу файла по FTP
*/
void TaskFTP(void *arg)
{
    uint8_t byte;
    int result;                                                                 // Result code of FTP exchange
    // 0Y - success
    //   01 - success, 1st trying
    //   02 - success, nd 2 trying
    // 1Y - user dependent
    //   10 - stopped by user (control gprsStatus)
    // 2Y - settings dependent
    // no login, pass....
    // 2Y - FTP server dependent
    // ?????

    while(1)
    {
        // ожидание команды на передачу FTP файла
        xQueueReceive(xFTPcmdQueue, &byte, (TickType_t) portMAX_DELAY);
        // FTP exchange
        result = doFTP();
        //============ после передачи ftp файла
        // отображаем отключение GPRS модуля на экране
        gprsStatus = false;
        StrEventForDisplay cmdUpdate;

        if(displayActive)
        {
            cmdUpdate.status = 1;
            cmdUpdate.codeEventForDisplay = UPDATE_DISPLAY;
            xQueueSendToBack(xBtnsQueue, &cmdUpdate, NULL);
        }

        //помещаем информацию об окончании работы режима в очередь
        StrEventMode modeEvent;
        modeEvent.status = 0;
        modeEvent.modeCode = FTP_M;
        xQueueSendToBack(xModesQueue, &modeEvent, NULL);
    }
}
/**
 \ingroup mdFTPGroup
 \author Shuvalov Sergey
 \brief Функция инициирует передачу файла по FTP.
*/
void enableFTP(void)
{
    uint8_t byte = 0;
    // здесь необходимая настройка переферии
    //помещаем информацию о режиме в очередь
    StrEventMode modeEvent;
    modeEvent.status = 1;
    modeEvent.modeCode = FTP_M;
    xQueueSendToBack(xModesQueue, &modeEvent, NULL);
    //включить модем и начать передачу файла
    xQueueSendToBack(xFTPcmdQueue, &byte, NULL);
}

/**
 \ingroup mdFTPGroup
 \author Borodov Maksim
 \brief Full FTP process
*/
int doFTP()
{
    // Don't forget to control gprsStatus
    int result = GPRS_start1;                                                      // result of function to be saved to LogBook. Default value - FTP start (button or schedule)

    //modeLogBook.modeCode = FTP_M;                                                 // LogBook

    for (int trying =0; trying <3; trying++)                                                 // 3 tries
    {
        //modeLogBook.status = trying+1;                                                 // Make event
        //xQueueSendToBack(xQueueLogBook, &modeLogBook, NULL);                        // Send event to LogBook queue
        if (trying == 1)                                                               // The second trying
        {
            vTaskDelay(5000);                                                         // Wait between restart
        }

        // [1]
        result = modem_init();                                                      // Init pins, modem ON, check modem

        if (result)
        {
            //modeLogBook.status = result;                                              // Make event
            //xQueueSendToBack(xQueueLogBook, &modeLogBook, NULL);                      // Send event to LogBook queue
            modem_powerOff();
            continue;
        }

        // [2] APN, LOGIN, PASS needed!!!!!
        result = GPRS_start();                                                      // connect to GPRS

        if (result)
        {
            //modeLogBook.status = result;                                              // Make event
            //xQueueSendToBack(xQueueLogBook, &modeLogBook, NULL);                      // Send event to LogBook queue
            GPRS_stop();
            modem_powerOff();
            continue;
        }

        long bytesSent = 0;
        result = FTP_client(&bytesSent);                                                     // connect to GPRS

        if (result)
        {
            //modeLogBook.status = result;                                              // Make event
            //xQueueSendToBack(xQueueLogBook, &modeLogBook, NULL);                      // Send event to LogBook queue
            GPRS_stop();
            modem_powerOff();
            continue;
        }

        // [4]
        //modeLogBook.status = result;                                              // Make event
        //xQueueSendToBack(xQueueLogBook, &modeLogBook, NULL);                      // Send event to LogBook queue
        GPRS_stop();
        modem_powerOff();                                                           // Modem power off + deInitPins
        gprsStatus = 0;                                                             // Reset status
        break;
    }

    return result;
}

/**
 \ingroup mdFTPGroup
 \author Borodov Maksim
 \brief Turn on modem and initialize it
*/
int modem_init()
{
    int result = 0;
    // ---> [Init modem functions ]
    modem_initPins = GL865_initPins;
    modem_powerOn = GL865_powerOn;
    modem_powerOff = GL865_powerOff;
    modem_check = GL865_check;
    // ---> [GPRS level]
    GPRS_enable = GL865_GprsEnable;
    GPRS_stop = GL865_GprsStop;
    // ---> [FTP level]
    FTP_client = GL865_FtpClient;
    //
    //  rebootModem = gl868_rebootModem;
    //  afterRebootInitModem = gl868_afterRebootInitModem;
    //
    //  SetTXDiodOff = gl868_SetTXDiodOff;
    //  SetTXDiodOn = gl868_SetTXDiodOn;
    //
    //  modem_hard_reset = gl868_modem_hard_reset;
    //
    //  connectFTP = gl868_connectFTP;
    // <---
    // ---> [Power ON modem ]
    modem_initPins();                                                             // Init pins for modem control: On/Off, Reset, USART
    modem_powerOn();                                                              // Power ON
    result = modem_check();                                                       // Check it
    return result;
}

/**
 \ingroup mdFTPGroup
 \author Borodov Maksim
 \brief Connect to GPRS service, 3 tries with 1000ms timeout
  Input:
    apn
    login
    pass
  Output:
    0 - OK
    >0 - ERRROR code
??? IP-address
*/
static bool GPRS_start()
{
    int result = 0;

    for (int i = 0; i < GPRS_INIT_COUNT; i++)
    {
        result = GPRS_enable();

        if (result)
        {
            vTaskDelay(GPRS_INIT_TIMEOUT_TICK);
            continue;
        }

        break;
    }

    return result;
}


