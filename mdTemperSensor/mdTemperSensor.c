/**
 \file
Файл содержит функции для работы с датчиком температуры.
*/

/** \defgroup mdTemperSensorGroup mdTemperSensor
 Специализация модуля - работа с датчиком температуры.
Логика работы с датчиком:
1. Подать питание(подождать 2 млСек).
2. Сконфигурировать (без прерывания по готовности данных)
3. При наступлении начала окна обработки данных, подать команду на начало обработки
(прерывание по готовности данных).
- MCU переходит в режим "Stop".
4. Прерывание по готовности данных - читаем значение температуры.
5. Заканчиваем вычисления (для которых нужно значение температуры).
6. Сохраняем вычисления - переходим в режим "Stop".
*/

#include "stm32l1xx.h"
#include "stm32l1xx_rtc.h"
#include "mdHardware/mdHardware.h"
#include "HardwareTemperSensor.h"
#include "mdTemperSensor/mdTemperSensor.h"
#include "mdModes/mdModes.h"
#include "mdPower/mdPower.h"

extern SemaphoreHandle_t xMutexParams;

TaskHandle_t xTemperSensor;
QueueHandle_t xTemperSensorQueue;
int temperCountError;

/**
 \ingroup mdTemperSensorGroup
 \author Shuvalov Sergey
 \brief Функция настраивает микроконтроллер для работы с датчиком темепературы.
*/
void initMCUforTemperSensor(void)
{
    initGpioForTemperSensor();
    initI2cForTemperSensor();
}

/**
 \ingroup mdTemperSensorGroup
 \author Shuvalov Sergey
 \brief Функция запитывает датчик температуры.
*/
void enableTemperSensor(void)
{
    RCC_AHBPeriphClockCmd(TEMPSENSOR_ENABLE_RCC_PORT, ENABLE);
    GPIO_InitTypeDef gpio_spi1;
    gpio_spi1.GPIO_Pin = TEMPSENSOR_ENABLE_PIN;
    gpio_spi1.GPIO_Mode = GPIO_Mode_OUT;
    gpio_spi1.GPIO_OType = GPIO_OType_PP;
    gpio_spi1.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_spi1.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(TEMPSENSOR_ENABLE_GPIO_PORT, &gpio_spi1);
    GPIO_ResetBits(TEMPSENSOR_ENABLE_GPIO_PORT, TEMPSENSOR_ENABLE_PIN);
}

/**
 \ingroup mdTemperSensorGroup
 \author Shuvalov Sergey
 \brief Функция снимает питание с датчика температуры.
*/
void disableTemperSensor(void)
{
    RCC_AHBPeriphClockCmd(TEMPSENSOR_ENABLE_RCC_PORT, ENABLE);
    GPIO_InitTypeDef gpio_spi1;
    gpio_spi1.GPIO_Pin = TEMPSENSOR_ENABLE_PIN;
    gpio_spi1.GPIO_Mode = GPIO_Mode_OUT;
    gpio_spi1.GPIO_OType = GPIO_OType_PP;
    gpio_spi1.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_spi1.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(TEMPSENSOR_ENABLE_GPIO_PORT, &gpio_spi1);
    GPIO_SetBits(TEMPSENSOR_ENABLE_GPIO_PORT, TEMPSENSOR_ENABLE_PIN);
}

/**
 \ingroup mdTemperSensorGroup
 \author Shuvalov Sergey
 \brief Функция конфигурирует датчик температуры.
    После reset необходима задержка 2 млСек.
*/
int configTemperSensor(void)
{
    if(!sendToTemperSensor(TMP117_EEPROM_Uclock, EEPROM_UNLOCK_BIT))
    {
        return 0;
    }

    if(!sendToTemperSensor(TMP117_ConfigurationRegister, 0x43c))
    {
        return 0;
    }

    uint16_t flag = EEPROM_BUSY_BIT;
    int count = 0;

    while(flag & EEPROM_BUSY_BIT)
    {
        if(!receiveFromTemperSensor(TMP117_EEPROM_Uclock, &flag))
        {
            return 0;
        }

        if(count > 10000)
        {
            return 0;
        }

        count++;
        vTaskDelay(1);
    }

    if(!sendToTemperSensor(TMP117_ConfigurationRegister, 2))
    {
        return 0;
    }

    vTaskDelay(5);
    return 1;
}

/**
 \ingroup mdTemperSensorGroup
 \author Shuvalov Sergey
 \brief Функция стартует работу датчика для получения значения температуры.
*/
void startConversionTemperSensor(void)
{
    sendToTemperSensor(TMP117_ConfigurationRegister, 0xc3c);
}

/**
 \ingroup mdTemperSensorGroup
 \author Shuvalov Sergey
 \brief Функция-поток стартует работу датчика для получения значения температуры.
*/
void TemperSensorTask(void *arg)
{
    uint8_t eventTemper;
    static bool temperSensorDataReady = false;
    double d;
    int16_t t;
    while(1)
    {
        xQueueReceive(xTemperSensorQueue, &eventTemper, (TickType_t) portMAX_DELAY);
        
        
        switch(eventTemper)
        {
            case TEMPER_START_CONVERSION:
                // включаем термодатчик 
                disableTemperSensor();
                NVIC_DisableIRQ(EXTI9_5_IRQn);             
                I2C_DeInit(I2C1);
                initMCUforTemperSensor();
                enableTemperSensor();
                vTaskDelay(2); // время для готовности датчика
                // после инициализации датчика появляется строб - сбрасываем на всякий
                getTemperFromSensor(&d, &t);
                
                // если предыдущая попытка была неудачной
                if(!temperSensorDataReady)
                {
                    temperCountError++;                   
                    if(temperCountError >= 2)
                    {                        
                        if(configTemperSensor())
                        {
                            temperCountError = 0;
                        }
                    }
                    if(temperCountError >= 3)
                    {
                        setEvent(EV_SIGNAL_T);
                        params.T = params.Tpod;
                    }
                }
                initTemperSensorInterrupt();
                temperSensorDataReady = false;
                
                //сбрасываем очередь - вдруг не пустая (наблюдалось такое)
                xQueueReset(xTemperSensorQueue);
                startConversionTemperSensor();   
                //в LPM режиме датчик продолжает работать пока не получим температуру
                setMaskForLPM(TEMPSENSOR_GPIO_PORT, TEMPSENSOR_SCL_PIN | TEMPSENSOR_SDA_PIN | TEMPSENSOR_ALERT_PIN);
                setMaskForLPM(TEMPSENSOR_ENABLE_GPIO_PORT, TEMPSENSOR_ENABLE_PIN);
            break;
            
            case TEMPER_DATA_READY:
                temperSensorDataReady = true;
                xSemaphoreTake(xMutexParams, portMAX_DELAY);
                if(!getTemperFromSensor(&params.Ttek, &params.TtekBin))
                {
                    temperCountError++;
                }
                else
                {
                    temperCountError = 0;
                    resetEvent(EV_SIGNAL_T);
                }                
                if(temperCountError >= 3)
                {
                    setEvent(EV_SIGNAL_T);
                    params.T = params.Tpod;
                }
                xSemaphoreGive(xMutexParams);
                //в LPM режиме датчик будет отключаться
                unsetMaskForLPM(TEMPSENSOR_GPIO_PORT, TEMPSENSOR_SCL_PIN | TEMPSENSOR_SDA_PIN | TEMPSENSOR_ALERT_PIN);
                unsetMaskForLPM(TEMPSENSOR_ENABLE_GPIO_PORT, TEMPSENSOR_ENABLE_PIN);
            break;   
            
        }
             
        //помещаем информацию о режиме в очередь
        StrEventMode mode;
        mode.status = 0;
        mode.modeCode = METERS_M;
        xQueueSendToBack(xModesQueue, &mode, NULL);
    }
}

/**
 \ingroup mdTemperSensorGroup
 \author Shuvalov Sergey
 \brief Функция считывает полученные данные.
*/
int getTemperFromSensor(double *temp, int16_t *tempBin)
{
    uint16_t data = 0;
    if(receiveFromTemperSensor(TMP117_TemperatureRegister, &data))
    {
        *temp = (int16_t)data;
        *tempBin = (int16_t)data;
    }
    else
    {
        return 0;
    }

    *temp *= 0.0078125;
    return 1;
}
