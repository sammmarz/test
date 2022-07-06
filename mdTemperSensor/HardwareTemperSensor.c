/**
 \file
Файл содержит функции для взаимодействия с термодатчиком TMP117.
*/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "mdHardware/mdHardware.h"
#include "mdModes/mdModes.h"
#include "mdDataManager/mdDataManager.h"
#include "HardwareTemperSensor.h"
#include "mdTemperSensor.h"

#define DI2C I2C1
#define DELAY_I2C 5000

#define TEMPER_SENSOR_ADR 0x90 //AD00 подведен к земле

/**
 \ingroup mdTemperSensor
 \author Shuvalov Sergey
 \brief Функция запитывает датчик температуры.
 \param [in] register_ адрес регистра в который надо записать значение
 \param [in] data записываемое значение
*/
int sendToTemperSensor(uint8_t register_, uint16_t data)
{
    int i = 1;
    I2C_AcknowledgeConfig(DI2C, ENABLE);
    //I2C_AcknowledgeConfig(DI2C, DISABLE);
    // На всякий случай ждем, пока шина осовободится
    i = 1;

    while(i++)
    {
        if(!I2C_GetFlagStatus(DI2C, I2C_FLAG_BUSY))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    // Генерируем старт
    I2C_GenerateSTART(DI2C, ENABLE);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_MODE_SELECT))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    I2C_Send7bitAddress(DI2C, TEMPER_SENSOR_ADR, I2C_Direction_Transmitter);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    //регистр
    I2C_SendData(DI2C, register_);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    //сначала старший байт потом младший
    I2C_SendData(DI2C, data >> 8);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    I2C_SendData(DI2C, data & 0xff);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    //стоп
    I2C_GenerateSTOP(DI2C, ENABLE);
    return 1;
}
/**
 \ingroup mdTemperSensor
 \author Shuvalov Sergey
 \brief Функция запитывает датчик температуры.
 \param [in] register_ адрес регистра c которого надо считать значение
*/
uint16_t receiveFromTemperSensor(uint8_t register_, uint16_t *dataTemp)
{
    int i = 1;
    I2C_AcknowledgeConfig(DI2C, ENABLE);

    // На всякий случай ждем, пока шина осовободится
    while(i++)
    {
        if(!I2C_GetFlagStatus(DI2C, I2C_FLAG_BUSY))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    // Генерируем старт
    I2C_GenerateSTART(DI2C, ENABLE);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_MODE_SELECT))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    //адрес
    I2C_Send7bitAddress(DI2C, TEMPER_SENSOR_ADR, I2C_Direction_Transmitter);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    //регистер
    I2C_SendData(DI2C, register_);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    // Повторный старт
    I2C_GenerateSTART(DI2C, ENABLE);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_MODE_SELECT))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    //адрес
    I2C_Send7bitAddress(DI2C, TEMPER_SENSOR_ADR, I2C_Direction_Receiver);
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    uint16_t data = 0;
    //сначала старший байт потом младший
    //читаем байт
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_SLAVE_BYTE_RECEIVED))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    data = I2C_ReceiveData(DI2C);
    data <<= 8;
    i = 1;

    while(i++)
    {
        if(I2C_CheckEvent(DI2C, I2C_EVENT_SLAVE_BYTE_RECEIVED))
        {
            break;
        }

        if(i >= DELAY_I2C)
        {
            return 0;
        }
    }

    data |= I2C_ReceiveData(DI2C);
    I2C_NACKPositionConfig(DI2C, I2C_NACKPosition_Current);
    I2C_AcknowledgeConfig(DI2C, DISABLE);
    //стоп
    I2C_GenerateSTOP(DI2C, ENABLE);
    *dataTemp = data;
    return 1;
}
/**
 \ingroup mdTemperSensor
 \author Shuvalov Sergey
 \brief Функция обработчик прерывания сигнала TEMP ALERT.
*/
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {               
        uint8_t eventTemper = TEMPER_DATA_READY;
        xQueueSendToBackFromISR(xTemperSensorQueue, &eventTemper, NULL);
        //помещаем информацию о режиме в очередь
        StrEventMode mode;
        mode.status = 1;
        mode.modeCode = METERS_M;
        xQueueSendToBackFromISR(xModesQueue, &mode, NULL);

        EXTI_ClearITPendingBit(EXTI_Line5);
    }    
}