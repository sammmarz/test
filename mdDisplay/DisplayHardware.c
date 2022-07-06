/**
 \file
Файл содержит функции настройки контроллера для взаимодействия c дисплеем,
настройки\обработка прерываний нажатия кнопок.
*/

#include "stm32l1xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "DisplayHardware.h"
#include "mdModes/mdModes.h"
#include "mdDisplay/mdDisplay.h"
#include "mdHardware/mdHardware.h"

//#define INVERS_DISPLAY_PIN
//мьютекс для работы с SPI1
extern xSemaphoreHandle xSPI1semaphore;


/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция передает данные или команду на дисплей
 \param [in] data передаваемые данные\команда
 \param [in] cmd команда - 1, данные - 0
*/
void sendByteToDisplay(uint8_t data, bool cmd)
{
    //xSemaphoreTake(xSPI1semaphore, portMAX_DELAY);

    // для формирования команды нужно выставить А0 в 0
    if(cmd)
    {
        GPIO_ResetBits(DISPLAY_GPIO_PORT, DISPLAY_PIN_A0);
    }
    else
    {
        GPIO_SetBits(DISPLAY_GPIO_PORT, DISPLAY_PIN_A0);
    }

    GPIO_ResetBits(DISPLAY_GPIO_PORT, DISPLAY_PIN_CS1);

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE ) == RESET)
        ;

    SPI_I2S_SendData(SPI2, data);
    const uint32_t delay = 400 / 20;// 128;

    for(int i=0; i<delay; i++); //задержка перед выставкой DISPLAY_PIN_CS1

    GPIO_SetBits(DISPLAY_GPIO_PORT, DISPLAY_PIN_CS1);
    //xSemaphoreGive(xSPI1semaphore);
}
/**
 \ingroup mdDebugGroup
 \author Shuvalov Sergey
 \brief Функция уведомляет поток обработки нажатия кнопок пользователем.
*/
void doLogicStatusEXTI(uint32_t extiLine, int btnNum)
{
    StrEventForDisplay btnPush;
    btnPush.codeEventForDisplay = btnNum;   
    xQueueSendToBackFromISR(xBtnsQueue, &btnPush, NULL);
    //
    StrEventMode modeBTN;
    modeBTN.status = 1;
    modeBTN.modeCode = DISPLAY_M;
    xQueueSendToBackFromISR(xModesQueue, &modeBTN, NULL);
    EXTI_ClearITPendingBit(extiLine);
}

/**
 \ingroup mdDebugGroup
 \author Shuvalov Sergey
 \brief Функция инициализации работы с кнопками.
*/
void initUserButtons(void)
{
    // инициализация GPIO
    initGpioForUserButtons();
    //настройка прерываний
    initUserButtonsInterrupts();
}
/**
 \ingroup mdDebugGroup
 \author Shuvalov Sergey
 \brief Функция устанавливает сигнал GPIO pin для запитывания дисплея
*/
void enableDisplayPower(void)
{
    RCC_AHBPeriphClockCmd(DISPLAY_POWER_RCC_PORT, ENABLE);
    GPIO_InitTypeDef gpio_spi1;
    gpio_spi1.GPIO_Pin = DISPLAY_POWER_PIN;
    gpio_spi1.GPIO_Mode = GPIO_Mode_OUT;
    gpio_spi1.GPIO_OType = GPIO_OType_PP;
    gpio_spi1.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_spi1.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(DISPLAY_POWER_GPIO_PORT, &gpio_spi1);
#ifdef INVERS_DISPLAY_PIN
    GPIO_ResetBits(DISPLAY_POWER_GPIO_PORT, DISPLAY_POWER_PIN);
#else
    GPIO_SetBits(DISPLAY_POWER_GPIO_PORT, DISPLAY_POWER_PIN);
#endif
}
/**
 \ingroup mdDebugGroup
 \author Shuvalov Sergey
 \brief Функция снимает сигнал GPIO pin для снятия питания дисплея
*/
void disableDisplayPower(void)
{
    RCC_AHBPeriphClockCmd(DISPLAY_POWER_RCC_PORT, ENABLE);
    GPIO_InitTypeDef gpio_spi1;
    gpio_spi1.GPIO_Pin = DISPLAY_POWER_PIN;
    gpio_spi1.GPIO_Mode = GPIO_Mode_OUT;
    gpio_spi1.GPIO_OType = GPIO_OType_PP;
    gpio_spi1.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_spi1.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(DISPLAY_POWER_GPIO_PORT, &gpio_spi1);
#ifdef INVERS_DISPLAY_PIN
    GPIO_SetBits(DISPLAY_POWER_GPIO_PORT, DISPLAY_POWER_PIN);
#else
    GPIO_ResetBits(DISPLAY_POWER_GPIO_PORT, DISPLAY_POWER_PIN);
#endif
}
