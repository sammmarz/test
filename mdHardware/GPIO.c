/**
 \file
Файл содержит функции инициализации GPIO.
*/

#include "mdHardware.h"
// ================================= for FLASH ================================
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для запитывания flash-памяти
*/
void initGpioForFlashPower(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(FLASH_POWER_RCC_PORT, ENABLE);
    //настройка GPIO пина для питания FLASH
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = FLASH_POWER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(FLASH_POWER_GPIO_PORT, &GPIO_InitStructure);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для обмена с flash-памятью по SPI1
*/
void initGpioForFlashSPI(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(FLASH_SPI_PINS_RCC_PORT, ENABLE);
    //
    GPIO_InitTypeDef gpio_spi1;
    gpio_spi1.GPIO_Pin = FLASH_PIN_SCK | FLASH_PIN_MOSI;
    gpio_spi1.GPIO_Speed= GPIO_Speed_10MHz;
    gpio_spi1.GPIO_Mode=GPIO_Mode_AF;
    gpio_spi1.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(FLASH_GPIO_PORT, &gpio_spi1);
    gpio_spi1.GPIO_Pin = FLASH_PIN_MISO;
    gpio_spi1.GPIO_Mode = GPIO_Mode_AF;
    //gpio_spi1.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(FLASH_GPIO_PORT, &gpio_spi1);
    //
    gpio_spi1.GPIO_Pin = FLASH_PIN_CS;
    gpio_spi1.GPIO_Mode = GPIO_Mode_OUT;
    gpio_spi1.GPIO_OType = GPIO_OType_PP;
    gpio_spi1.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_spi1.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(FLASH_GPIO_PORT, &gpio_spi1);
}
// ================================= for DISPLAY ===============================
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для запитывания дисплея
*/
void initGpioForDisplayPower(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(DISPLAY_POWER_RCC_PORT, ENABLE);
    //настройка GPIO пина для питания дисплея
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DISPLAY_POWER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(DISPLAY_POWER_GPIO_PORT, &GPIO_InitStructure);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для обмена с дисплеем по SPI1
*/
void initGpioForDisplaySPI(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(DISPLAY_SPI_RCC_PORT, ENABLE);
    //
    // настройка SCL(pin 5) SI(pin 7)
    GPIO_InitTypeDef gpio_spi1;
    gpio_spi1.GPIO_Pin = DISPLAY_PIN_SCL | DISPLAY_PIN_S1;
    gpio_spi1.GPIO_Speed = GPIO_Speed_40MHz;
    gpio_spi1.GPIO_Mode = GPIO_Mode_AF;
    gpio_spi1.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(DISPLAY_SPI_PORT, &gpio_spi1);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для работы с дисплеем
*/
void initGpioForDisplayCommon(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(DISPLAY_GPIO_RCC_PORT, ENABLE);
    //инициализация A0(pin 0) RES(pin 1) CS1(pin 2)
    GPIO_InitTypeDef gpio_spi1;
    gpio_spi1.GPIO_Pin = DISPLAY_PIN_A0 | DISPLAY_PIN_RES | DISPLAY_PIN_CS1;
    gpio_spi1.GPIO_Mode = GPIO_Mode_OUT;
    gpio_spi1.GPIO_OType = GPIO_OType_PP;
    gpio_spi1.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_spi1.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(DISPLAY_GPIO_PORT, &gpio_spi1);
}
// ================================= Users Buttons =============================
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для кнопок
 По умолчанию заряд положительный, поэтому подтяжка к питанию.
*/
void initGpioForUserButtons(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(BTNS_PINS_RCC_PORT, ENABLE);
    //
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = BTN_LEFT_PIN | BTN_CENTER_PIN | BTN_RIGHT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;// GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
// ================================= Модем =====================================
/**
 \ingroup mdHardwareGroup
 \author Borodov Maksim
 \brief Init GPIO pin for modem. POWER
*/
void initGpioForModemPOWER(void)
{
    // [Power ON/OFF pin] Init
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GL865_POWER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GL865_POWER_PORT, &GPIO_InitStructure);
    // [Power ON/OFF pin] SET default
    GPIO_ResetBits(GL865_POWER_PORT, GL865_POWER_PIN);                            // Module OFF
}

void initGpioForModemTX(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MODEM_TX_PIN;                                   // TX alternate function push-pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                  //GPIO_PuPd_UP or GPIO_PuPd_DOWN;
    GPIO_Init(MODEM_GPIO_PORT, &GPIO_InitStructure);
}

void initGpioForModemRX(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MODEM_RX_PIN;                                   //Rx
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                  // ?GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                              // ?GPIO_PuPd_NOPULL;//
    GPIO_Init(MODEM_GPIO_PORT, &GPIO_InitStructure);
}

void initGpioForModemCTS(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin =  MODEM_CTS_PIN;                                 //CTS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(MODEM_GPIO_PORT, &GPIO_InitStructure);
}

void initGpioForModemRTS(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MODEM_RTS_PIN;                                  //RTS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(MODEM_GPIO_PORT, &GPIO_InitStructure);
}

void initGpioForModemRESET(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MODEM_RESET_PIN;                                //RESET
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(MODEM_GPIO_PORT, &GPIO_InitStructure);
    // [Reset] SET default
    GPIO_ResetBits(MODEM_GPIO_PORT, MODEM_RESET_PIN);                             // Modem RESET to 1 (no reset)
}

void deInitGpioForModem(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, DISABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin =  MODEM_TX_PIN | MODEM_RX_PIN | MODEM_CTS_PIN | MODEM_RTS_PIN | MODEM_RESET_PIN ;                                 //CTS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(MODEM_GPIO_PORT, &GPIO_InitStructure);
}

// ============================= Оптический нтерфейс ===========================
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для оптического интерфейса.
*/
void initGpioForOpticalInterface(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(OPT_RCC_PORT, ENABLE);
    //
    GPIO_InitTypeDef GPIO_InitStructure;
    //TX alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = OPT_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;//
    GPIO_Init(OPT_GPIO_PORT, &GPIO_InitStructure);
    //Rx
    GPIO_InitStructure.GPIO_Pin = OPT_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //GPIO_PuPd_NOPULL;//
    GPIO_Init(OPT_GPIO_PORT, &GPIO_InitStructure);
    //OPT Enable
    GPIO_InitStructure.GPIO_Pin = OPT_ENABLE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(OPT_GPIO_PORT, &GPIO_InitStructure);
}
// ============================= Датчик температуры ===========================
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pins для датчика температуры.
*/
void initGpioForTemperSensor(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(TEMPSENSOR_GPIO_RCC_PORT, ENABLE);
    //
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = TEMPSENSOR_SCL_PIN | TEMPSENSOR_SDA_PIN;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_OType = GPIO_OType_PP;//
    gpio.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_DOWN;//
    GPIO_Init(TEMPSENSOR_GPIO_PORT, &gpio);
    GPIO_PinAFConfig(TEMPSENSOR_GPIO_PORT, GPIO_PinSource8, GPIO_AF_I2C1);
    GPIO_PinAFConfig(TEMPSENSOR_GPIO_PORT, GPIO_PinSource9, GPIO_AF_I2C1);
    //TEMP ALERT
    gpio.GPIO_Pin = TEMPSENSOR_ALERT_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(TEMPSENSOR_GPIO_PORT, &gpio);
}
// ============================= Датчики Холла ================================
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pins для датчиков Холла.
*/
void initGpioForHallSensors(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(HALLSENSOR_RCC_PORT, ENABLE);
    //
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = HALLSENSOR_A_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(HALLSENSOR_GPIO_PORT, &GPIO_InitStructure);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для питания датчика Холла.
*/
void initGpioForHallSensorPower(void)
{
    //тактируем порт
    RCC_AHBPeriphClockCmd(HALLSENSOR_POWER_RCC_PORT, ENABLE);
    //
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = HALLSENSOR_POWER_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(HALLSENSOR_POWER_GPIO_PORT, &GPIO_InitStructure);
}
// ============================= Вскрытие корпуса ===========================
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для анализа вскрытия корпуса.
*/
void initGpioAnaliseBoxOpen(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //для анализа вскрытия корпуса
    RCC_AHBPeriphClockCmd(BOX_RCC_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = BOX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BOX_GPIO_PORT, &GPIO_InitStructure);
    //для анализа вскрытия батарейного отсека
    RCC_AHBPeriphClockCmd(BATBOX_RCC_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = BATBOX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BATBOX_GPIO_PORT, &GPIO_InitStructure);
}
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации GPIO pin для анализа воздействия магнитом.
*/
void initGpioReedSwitch(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(REED_SWITCH_1_RCC_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = REED_SWITCH_1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(REED_SWITCH_1_GPIO_PORT, &GPIO_InitStructure);
    RCC_AHBPeriphClockCmd(REED_SWITCH_2_RCC_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = REED_SWITCH_2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(REED_SWITCH_2_GPIO_PORT, &GPIO_InitStructure);
}
