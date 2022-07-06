/**
 \file
Файл содержит функции определяющие логику работы с флэш-памятью.
*/

/** \defgroup mdFlashMemoryGroup mdFlashMemory
 Модуль определяет функции работы с флэш-памятью.
*/
//#define _DLIB_TIME_USES_64
#include <time.h>
#include <string.h>
#include "stm32l1xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "mdHardware/mdHardware.h"
#include "HardwareFlashMemory.h"
#include "mdFlashMemory.h"
#include "mdModes/mdModes.h"
#include "mdDataManager/mdDataManager.h"

//мьютекс для работы с SPI1
extern xSemaphoreHandle xSPI1semaphore;

extern const unsigned short CRCInit;
unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);

QueueHandle_t xFlashWriteQueue;
TaskHandle_t xTaskWriteBufferToFlash;

static bool crcError;

/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция сбрасывает флаг ошибки работы с flash-памятью
*/
void resetCrcFlashError(void)
{
    crcError = false;
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция возвращает значение флага ошибки работы с flash-памятью
*/
bool isCrcFlashError(void)
{
    return crcError;
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция очищает flash-память в диапазоне адрессов по 4 кб со сбросом watchdog.
*/
void eraseFlashMemory(uint32_t beg, uint32_t end)
{
    for(int adress = beg; adress <= end; adress+= 0x1000)
    {
        eraseBlock(adress);
        IWDG_ReloadCounter();
    }
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция записи во flash-память.
*/
void writeDataInFlash(StrQueueInfoData *bufInfo)
{
    xSemaphoreTake(xSPI1semaphore, portMAX_DELAY);
    //просто чтобы не прописывать каждый раз bufInfo->dataInfo->adress
    int adress = bufInfo->dataInfo->adress;

    //если это новый блок, то сначала его нужно очистить
    if((adress % 0x1000) == 0)
    {
        eraseBlock(adress);
    }

    waitFlashToFree();
    prepareToWriteDataInFlash();
    waitFlashToFree();
    //команда на запись
    GPIO_ResetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
    SendReceiveSPIByte(SPI1, SET_FLASH_PAGE_PROGRAM);
    sendFlashAdress(adress);
    //передача данных
    for(int i=0; i<bufInfo->length; ++i)
    {
        
        SendReceiveSPIByte(SPI1, bufInfo->data[i]);
        
        if(bufInfo->dataInfo->flags & BLOCK_4KB)
        {
            //идентифицируем передачу последнего байта данных в структуре
            int lastAdr = (adress & 0xfff000) + bufInfo->dataInfo->size - 1;

            if(adress == lastAdr)
            {
                // след. запись в новый блок
                adress &= 0xfff000;
                adress += 0x1000;

                if(adress > bufInfo->dataInfo->adressFlashEnd)
                {
                    adress = bufInfo->dataInfo->adressFlashBegin;
                }

                break;
            }
        }

        adress++;

        if(adress > bufInfo->dataInfo->adressFlashEnd)
        {
            adress = bufInfo->dataInfo->adressFlashBegin;
        }

        // переход на новый блок
        if((adress % 0x1000) == 0)
        {                        
            eraseBlock(adress);
            waitFlashToFree();
        }

        // переход на новую страницу
        if((adress % 0x100) == 0)
        {
            GPIO_SetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
            prepareToWriteDataInFlash();
            GPIO_ResetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
            SendReceiveSPIByte(SPI1, SET_FLASH_PAGE_PROGRAM);
            sendFlashAdress(adress);
        }
    }

    // выравнивание по размеру страницы, 256 байт
    if(adress % 0x100)
    {
        // обнуляем начальный адрес на странице
        adress &= 0xffffff00;
        // след. запись будет с новой страницы
        adress += 0x100;

        if(adress > bufInfo->dataInfo->adressFlashEnd)
        {
            adress = bufInfo->dataInfo->adressFlashBegin;
        }

        /*
            проверка на необходимость очистки нового блока в начале функции
            т.е. при след. заходе в функцию блок будет очищен if((adress % 0x1000) == 0)
        */
    }

    bufInfo->dataInfo->adress = adress;
    GPIO_SetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
    xSemaphoreGive(xSPI1semaphore);
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция чтения из flash-памяти
*/
void readDataFromFlash(uint8_t *buf, int length, int adress)
{
    xSemaphoreTake(xSPI1semaphore, portMAX_DELAY);
    waitFlashToFree();
    //команда на запись
    GPIO_ResetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
    SendReceiveSPIByte(SPI1, SET_FLASH_READ_DATA);
    sendFlashAdress(adress);

    for(int i=0; i<length; ++i)
    {
        buf[i] = SendReceiveSPIByte(SPI1, DUMMY_BYTE);
    }

    GPIO_SetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
    xSemaphoreGive(xSPI1semaphore);
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция записи в промежуточный буфер (очередь), чтобы не ждать освобождения флеш.
*/
void writeDataToFlashQueue(uint8_t *buf, int length, StrFlashInfoData *dataInfo)
{
    StrQueueInfoData queueData;
    queueData.dataInfo = dataInfo;

    //уведомляем поток режимов, о начале работы с флеш
    StrEventMode modeFlashEvent;
    modeFlashEvent.status = 1;
    modeFlashEvent.modeCode = FLASH_M;
    xQueueSendToBack(xModesQueue, &modeFlashEvent, 0);
    
    if(length <= 256)
    {
        queueData.length = length;
        memcpy(queueData.data, buf, length);
        xQueueSendToBack(xFlashWriteQueue, &queueData, 0);
    }
    else
    {
        //определяем сколько блоков понадобиться для размещения во флеш
        int sumBlock = length / 256;

        if(length % 256)
        {
            sumBlock++;
        }

        for(int i=1, adress = 0; i<=sumBlock; ++i)
        {
            if(i == sumBlock)
            {
                queueData.length = length % 256;
            }
            else
            {
                queueData.length = 256;
            }

            memcpy(queueData.data, &buf[adress], queueData.length);
            adress += 256; //256 == 0x100
            xQueueSendToBack(xFlashWriteQueue, &queueData, 0);
        }
    }
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция обновляет адреса в структуре params.
*/
void paramsFlashAdressUpdate(void)
{
    params.adrFlashIntervArc = intervalArchiveInfo.adress;
    params.adrFlashDayArc = dayArchiveInfo.adress;
    params.adrFlashMonthArc = monthArchiveInfo.adress;
    params.adrFlashEventArc = eventsArchiveInfo.adress;
    params.adrFlashChangesArc = changesArchiveInfo.adress;
    params.adrFlashParams = paramsInfo.adress;
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Основная функция обратного вызова модуля.

 Функция в бесконечном цикле ожидает сигнала о необходимости переписать данные из
буфера во флеш-память.
*/
void WriteToFlashThread(void *arg)
{
    StrQueueInfoData queueDataInfo;
    uint16_t CRCbefore = CRCInit;
    uint16_t CRCafter = CRCInit;
    int adress = 0;
    bool flashActive = false;
    while(1)
    {
        xQueueReceive(xFlashWriteQueue, &queueDataInfo, (TickType_t) portMAX_DELAY);
        if(!flashActive)
        {
            flashActive = true;
            initGpioForFlashPower();
            enableFlashPower();
            initFlashSPI();
            vTaskDelay(10);
        }
        if(queueDataInfo.dataInfo->flags & CONTROL_CRC)
        {
            adress = queueDataInfo.dataInfo->adress;            
            CRCbefore = GetCRC16(queueDataInfo.data, queueDataInfo.length, CRCInit);
            //          
            writeDataInFlash(&queueDataInfo);
            //
            readDataFromFlash(queueDataInfo.data, queueDataInfo.length, adress);
            CRCafter = GetCRC16(queueDataInfo.data, queueDataInfo.length, CRCInit);
            if(CRCbefore != CRCafter)
            {        
                crcError = true;
            }
        }
        else
        {
            writeDataInFlash(&queueDataInfo);
        }
        paramsFlashAdressUpdate();
        //уведомляем поток режимов об окончании работы с флеш если очередь пуста
        if(uxQueueSpacesAvailable(xFlashWriteQueue) == QUEUE_FLASH_SIZE)
        {
            flashActive = false;
            StrEventMode modeEvent;
            modeEvent.status = 0;
            modeEvent.modeCode = FLASH_M;
            xQueueSendToBack(xModesQueue, &modeEvent, 0);
        }
    }
}
