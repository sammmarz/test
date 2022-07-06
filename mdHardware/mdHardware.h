#ifndef mdHardwareSTM32_h
#define mdHardwareSTM32_h

#include "stm32l1xx.h"
#include "stm32l1xx_i2c.h"
#include "stm32l1xx_rtc.h"
#include "stm32l1xx_adc.h"
#include "stm32l1xx_iwdg.h"
#include "stm32l1xx_usart.h"
#include "mdFlashMemory/DataForFlash.h"
#include "../Params/Params.h"
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

    
// определение пинов для переключения батареи А
#define SWITCH_BAT_A_RCC_PORT RCC_AHBPeriph_GPIOH
#define SWITCH_BAT_A_GPIO_PORT GPIOH
#define SWITCH_BAT_A_PIN GPIO_Pin_1
// определение пинов для переключения батареи B
#define SWITCH_BAT_B_RCC_PORT RCC_AHBPeriph_GPIOB
#define SWITCH_BAT_B_GPIO_PORT GPIOB
#define SWITCH_BAT_B_PIN GPIO_Pin_7   
// определение пинов для disincreasy
#define SWITCH_DISINCREASY_RCC_PORT RCC_AHBPeriph_GPIOC
#define SWITCH_DISINCREASY_GPIO_PORT GPIOC
#define SWITCH_DISINCREASY_PIN GPIO_Pin_2 
// определение пинов для requestBat
#define SWITCH_REQ_BAT_RCC_PORT RCC_AHBPeriph_GPIOD
#define SWITCH_REQ_BAT_GPIO_PORT GPIOD
#define SWITCH_REQ_BAT_PIN GPIO_Pin_3


// определение пинов для работы для отладчика
#define SYS_JTMS_SWDIO GPIO_Pin_13
#define SYS_JTCK_SWCLK GPIO_Pin_14
#define SYS_JTDI GPIO_Pin_15

// определение пинов для работы с дисплеем
#define DISPLAY_POWER_RCC_PORT RCC_AHBPeriph_GPIOC
#define DISPLAY_POWER_GPIO_PORT GPIOC
#define DISPLAY_POWER_PIN GPIO_Pin_3
//
#define DISPLAY_GPIO_RCC_PORT RCC_AHBPeriph_GPIOA
#define DISPLAY_GPIO_PORT GPIOA
#define DISPLAY_PIN_A0 GPIO_Pin_0
#define DISPLAY_PIN_RES GPIO_Pin_1
#define DISPLAY_PIN_CS1 GPIO_Pin_2
//
#define DISPLAY_SPI_RCC_PORT RCC_AHBPeriph_GPIOD
#define DISPLAY_SPI_PORT GPIOD
#define DISPLAY_PIN_SCL GPIO_Pin_1
#define DISPLAY_PIN_S1 GPIO_Pin_4

// определение пинов для работы с флэш-памятью
#define FLASH_POWER_RCC_PORT RCC_AHBPeriph_GPIOB
#define FLASH_POWER_GPIO_PORT GPIOB
#define FLASH_POWER_PIN GPIO_Pin_10
//
#define FLASH_SPI_PINS_RCC_PORT RCC_AHBPeriph_GPIOE
#define FLASH_GPIO_PORT GPIOE
#define FLASH_PIN_CS GPIO_Pin_12
#define FLASH_PIN_SCK GPIO_Pin_13
#define FLASH_PIN_MISO GPIO_Pin_14
#define FLASH_PIN_MOSI GPIO_Pin_15

// определение пинов для кнопок
#define BTNS_PINS_RCC_PORT RCC_AHBPeriph_GPIOB
#define BTN_LEFT_PIN GPIO_Pin_11
#define BTN_CENTER_PIN GPIO_Pin_12
#define BTN_RIGHT_PIN GPIO_Pin_13

// определение пинов для модема
#define MODEM_GPIO_PORT GPIOD
#define MODEM_TX_PIN GPIO_Pin_8
#define MODEM_RX_PIN GPIO_Pin_9
#define MODEM_RING_PIN GPIO_Pin_10
#define MODEM_CTS_PIN GPIO_Pin_11
#define MODEM_RTS_PIN GPIO_Pin_12
#define MODEM_RESET_PIN GPIO_Pin_13

#define GL865_POWER_PORT   	 GPIOA
#define GL865_POWER_PIN    	 GPIO_Pin_3                                     // GL865 on/off

// определение пинов для оптического интерфейса
#define OPT_RCC_PORT RCC_AHBPeriph_GPIOA
#define OPT_GPIO_PORT GPIOA
#define OPT_ENABLE_PIN GPIO_Pin_8
#define OPT_TX_PIN GPIO_Pin_9
#define OPT_RX_PIN GPIO_Pin_10

// определение пинов для датчика температуры
#define TEMPSENSOR_GPIO_RCC_PORT RCC_AHBPeriph_GPIOB
#define TEMPSENSOR_GPIO_PORT GPIOB
#define TEMPSENSOR_SCL_PIN GPIO_Pin_8
#define TEMPSENSOR_SDA_PIN GPIO_Pin_9
#define TEMPSENSOR_ALERT_PIN GPIO_Pin_5
//
#define TEMPSENSOR_ENABLE_RCC_PORT RCC_AHBPeriph_GPIOE
#define TEMPSENSOR_ENABLE_GPIO_PORT GPIOE
#define TEMPSENSOR_ENABLE_PIN GPIO_Pin_11

// определение пинов для датчиков Холла
#define HALLSENSOR_POWER_RCC_PORT RCC_AHBPeriph_GPIOC
#define HALLSENSOR_POWER_GPIO_PORT GPIOC
#define HALLSENSOR_POWER_PIN GPIO_Pin_5
//
#define HALLSENSOR_GPIO_PORT GPIOB
#define HALLSENSOR_RCC_PORT RCC_AHBPeriph_GPIOB
#define HALLSENSOR_A_PIN GPIO_Pin_0

// вскрытие корпуса
#define BOX_GPIO_PORT GPIOA
#define BOX_RCC_PORT RCC_AHBPeriph_GPIOA
#define BOX_PIN GPIO_Pin_12
// вскрытие бат.отсека
#define BATBOX_GPIO_PORT GPIOD
#define BATBOX_RCC_PORT RCC_AHBPeriph_GPIOD
#define BATBOX_PIN GPIO_Pin_2

// воздействие магнитом на прибор
#define REED_SWITCH_1_GPIO_PORT GPIOE
#define REED_SWITCH_1_RCC_PORT RCC_AHBPeriph_GPIOE
#define REED_SWITCH_1_PIN GPIO_Pin_0
//
#define REED_SWITCH_2_GPIO_PORT GPIOE
#define REED_SWITCH_2_RCC_PORT RCC_AHBPeriph_GPIOE
#define REED_SWITCH_2_PIN GPIO_Pin_1

//head control
#define HEAD_CONTROL_GPIO_PORT GPIOD
#define HEAD_CONTROL_PIN GPIO_Pin_14

// lock
#define LOCK_GPIO_PORT GPIOD
#define LOCK_PIN GPIO_Pin_7

// GPIO
void initGpioForDisplayPower(void);
void initGpioForDisplaySPI(void);
void initGpioForDisplayCommon(void);
//
void initGpioForUserButtons(void);
//
void initGpioForTemperSensor(void);
//
void initGpioForHallSensors(void);
void initGpioForHallSensorPower(void);
//
void initGpioForFlashPower(void);
void initGpioForFlashSPI(void);
//
void initGpioForOpticalInterface(void);
//
void initGpioAnaliseBoxOpen(void);
//
void initGpioReedSwitch(void);

// USART1
void initUSART_1(int speed, uint16_t USART_Parity);

// Modem
void initGpioForModemPOWER(void);
void initGpioForModemTX(void);
void initGpioForModemRX(void);
void initGpioForModemCTS(void);
void initGpioForModemRTS(void);
void initGpioForModemRESET(void);
void deInitGpioForModem(void);

void initModemInterrupt(void);

// USART3
void initUSART_3(int speed, uint16_t USART_Parity);
bool isCts_usart3();
bool noCTS_usart3(portTickType TimeToWait);
void rtsUp_usart3();
void rtsDown_usart3();

extern xQueueHandle USART3_RX_Buf_Q;                                            // QUEUE: IRQ RX buffer for USART3
#define MODEM_BUF_SIZE 512                                                      // RX, TX buffer length
typedef struct
{
    unsigned char RX[MODEM_BUF_SIZE];
    unsigned char TX[MODEM_BUF_SIZE];
} MODEM_BUF;
void sendDataUsart3(uint8_t *buf, int len);
bool recv_usart3(uint8_t *buf, portTickType xTicksToWait);

// SPI1
void initSPI_1(void);
// SPI2
void initSPI_2(void);
//I2C
void initI2cForTemperSensor(void);
// Interrupts
void initUserButtonsInterrupts(void);
void initTemperSensorInterrupt(void);
void initHallSensorsInterrupts(void);
void initControlBoxOpenInterrupt(void);
void initControlBatBoxOpenInterrupt(void);
void initOpticalInterfaceInterrupt(void);

void initRTCforIntervalAlarmInterrupt(StrCorrectorParams *strCorParams);
void initRTCforPeriodCycleMeasureInterrupt(StrCorrectorParams *strCorParams);
/*

//


//
void initGpioForOpticalInterface(void);

//
void initGpioForHallSensor(void);



//USART1
void initUSART_1(void);
// Interrupts

void initOpticalInterfaceInterrupt(void);

void initHallSensorInterrupt(void);

*/
#endif