/**
 \file
Файл содержит функции для обеспечнеия работы с флеш-памятью.
*/

#include "FreeRTOS.h"
#include "semphr.h"
#include "mdFlashMemory.h"
#include "HardwareFlashMemory.h"
#include "mdHardware/mdHardware.h"

#define DELAY_SPI 5000

//#define INVERS_FLASH_PIN
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция отправляет байт по SPI, ожидает и возвращает принятый байт
*/
uint8_t SendReceiveSPIByte(SPI_TypeDef *SPIx, uint8_t byte)
{    
    int i = 1;
    //ожидание освобождения буфера для выдачи
    while(i++)
    {
        if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != RESET)
        {
            break;
        }
        if(i >= DELAY_SPI)
        {
            return 0;
        }
    }
    
    i = 1;
    
    //запись байта на выдачу
    SPI_I2S_SendData(SPI1, byte);       
    while(i++)
    {
        if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET)
        {
            break;
        }
        if(i >= DELAY_SPI)
        {
            return 0;
        }
    }

    //возвращение принятого байта
    return SPI_I2S_ReceiveData(SPI1);
}
/*
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция отправляет байт по SPI

void sendSPIByte(SPI_TypeDef *SPIx, uint8_t byte)
{
    //ожидание освобождения буфера для выдачи
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE ) == RESET)
        ;

    //запись байта на выдачу
    SPI_I2S_SendData(SPI1, byte);

    //ждём пока байт будет отправлен
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE ) == RESET)
        ;
}
*/
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция ожидает окончания работы контроллера флеш, готовности принять новые команды.
*/
void waitFlashToFree(void)
{
    //ожидание пока контроллер флеш-памяти освободится (проверка разряда BUSY)
    while (1)
    {
        GPIO_ResetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
        SendReceiveSPIByte(SPI1, GET_FLASH_STATUS_REGISTER_1);
        uint8_t status = SendReceiveSPIByte(SPI1, DUMMY_BYTE);
        GPIO_SetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);

        if((status & FLAG_FLASH_BUSY) == 0)
        {
            break;
        }
    }
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция выставляет флаг разрешения записи и ожидает подтверждение выставки
*/
void prepareToWriteDataInFlash(void)
{
    while (1)
    {
        //разрешение записи во флеш
        GPIO_ResetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
        SendReceiveSPIByte(SPI1, SET_FLASH_WRITE_ENABLE);
        GPIO_SetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
        //vTaskDelay(1);
        //ожидание пока контроллер флеш-памяти не готов к записи
        GPIO_ResetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
        SendReceiveSPIByte(SPI1, GET_FLASH_STATUS_REGISTER_1);
        uint8_t status = SendReceiveSPIByte(SPI1, DUMMY_BYTE);
        GPIO_SetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);

        if(status & FLAG_FLASH_WRITE_ENABLE)
        {
            break;
        }
    }
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция передаёт адресс во флеш
*/
void sendFlashAdress(int adress)
{
    //передача адреса
    SendReceiveSPIByte(SPI1, (adress >> 16) & 0xff);
    SendReceiveSPIByte(SPI1, (adress >> 8) & 0xff);
    SendReceiveSPIByte(SPI1, adress & 0xff);
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция отправляет команду на стирание 4 Кбайт памяти с указанного адреса
*/
void eraseBlock(int adress)
{    
    GPIO_SetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
    waitFlashToFree();
    prepareToWriteDataInFlash();
    GPIO_ResetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
    SendReceiveSPIByte(SPI1, SET_FLASH_ERASE_BLOCK);
    sendFlashAdress(adress);
    GPIO_SetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
}

/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция устанавливает сигнал GPIO pin для запитывания flash-памяти
*/
void enableFlashPower(void)
{
    //GPIO_ResetBits(FLASH_POWER_GPIO_PORT, FLASH_POWER_PIN);
#ifdef INVERS_FLASH_PIN
    GPIO_ResetBits(FLASH_POWER_GPIO_PORT, FLASH_POWER_PIN);
#else
    GPIO_SetBits(FLASH_POWER_GPIO_PORT, FLASH_POWER_PIN);
#endif
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция снимает сигнал GPIO pin для снятия питания flash-памяти
*/
void disableFlashPower(void)
{
#ifdef INVERS_FLASH_PIN
    GPIO_SetBits(FLASH_POWER_GPIO_PORT, FLASH_POWER_PIN);
#else
    GPIO_ResetBits(FLASH_POWER_GPIO_PORT, FLASH_POWER_PIN);
#endif
}
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция инициализации SPI для работы с flash-памятью
*/
void initFlashSPI(void)
{
    // настройка GPIO
    initGpioForFlashSPI();
    GPIO_SetBits(FLASH_GPIO_PORT, FLASH_PIN_CS);
    // настройка SPI
    initSPI_1();
}
