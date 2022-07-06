/**
 \file
Файл содержит функции для перехода\выхода в режим пониженного энергопотребления.
*/

/** \defgroup mdPowerGroup mdPower
 Специализация модуля - работа с питанием микроконтроллера.
*/

#include "stm32l1xx.h"
#include "stm32l1xx_lcd.h"

#include "mdRTC/mdRTC.h"
#include "mdDisplay/mdDisplay.h"
#include "mdDisplay/DisplayFunctions.h"
#include "mdDisplay/DisplayHardware.h"
#include "mdHardware/mdHardware.h"
#include "mdPrintToUART/printToUART.h"
// otlad
#include "mdTemperSensor/mdTemperSensor.h"
#include "mdTemperSensor/HardwareTemperSensor.h"
#include "mdOpticalInterface/mdOpticalInterface.h"
#include "mdFlashMemory/HardwareFlashMemory.h"
#include "mdPower.h"

uint8_t activeBat;

extern double temperature;

void upPB14(void);
void downPB14(void);
// otlad end

enum{indxGpioA, indxGpioB, indxGpioC, indxGpioD, indxGpioE, indxGpioH};

uint32_t maskForLPM[indxGpioH+1];

/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция устанавливает маску для пинов которые не нужно переводить в особое
состояние при переходе в LPM
*/
void setMaskForLPM(GPIO_TypeDef const * const GPIOx, uint32_t pin)
{
    switch((int)GPIOx)
    {
        case (int)GPIOA:
            maskForLPM[indxGpioA] |= pin;
        break;
        case (int)GPIOB:
            maskForLPM[indxGpioB] |= pin;
        break;
        case (int)GPIOC:
            maskForLPM[indxGpioC] |= pin;
        break;
        case (int)GPIOD:
            maskForLPM[indxGpioD] |= pin;
        break;
        case (int)GPIOE:
            maskForLPM[indxGpioE] |= pin;
        break;
        case (int)GPIOH:
            maskForLPM[indxGpioH] |= pin;
        break;
    }
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция снимает маску для пинов которые не нужно переводить в особое
состояние при переходе в LPM
*/
void unsetMaskForLPM(GPIO_TypeDef const * const GPIOx, uint32_t pin)
{
    switch((int)GPIOx)
    {
        case (int)GPIOA:
            maskForLPM[indxGpioA] &= ~pin;
        break;
        case (int)GPIOB:
            maskForLPM[indxGpioB] &= ~pin;
        break;
        case (int)GPIOC:
            maskForLPM[indxGpioC] &= ~pin;
        break;
        case (int)GPIOD:
            maskForLPM[indxGpioD] &= ~pin;
        break;
        case (int)GPIOE:
            maskForLPM[indxGpioE] &= ~pin;
        break;
        case (int)GPIOH:
            maskForLPM[indxGpioH] &= ~pin;
        break;
    }
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция устанавливает пины для перехода в режим пониженного энергопотребления.
*/
void setPinsForLpm(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  
      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
    //valve in
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    setMaskForLPM(GPIOB, GPIO_Pin_6);
    //request bat
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_3); //!!! this is right
    setMaskForLPM(GPIOD, GPIO_Pin_3);
    // LCD en
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = DISPLAY_POWER_PIN;
    GPIO_Init(DISPLAY_POWER_GPIO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(DISPLAY_POWER_GPIO_PORT, DISPLAY_POWER_PIN);
    setMaskForLPM(DISPLAY_POWER_GPIO_PORT, DISPLAY_POWER_PIN);
    // temper en
    if(!(maskForLPM[indxGpioE] & TEMPSENSOR_ENABLE_PIN))
    {
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Pin = TEMPSENSOR_ENABLE_PIN;
        GPIO_Init(TEMPSENSOR_ENABLE_GPIO_PORT, &GPIO_InitStructure);
        GPIO_SetBits(TEMPSENSOR_ENABLE_GPIO_PORT, TEMPSENSOR_ENABLE_PIN);
        setMaskForLPM(TEMPSENSOR_ENABLE_GPIO_PORT, TEMPSENSOR_ENABLE_PIN);
    }
    // flash en
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = FLASH_POWER_PIN;
    GPIO_Init(FLASH_POWER_GPIO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(FLASH_POWER_GPIO_PORT, FLASH_POWER_PIN);
    setMaskForLPM(FLASH_POWER_GPIO_PORT, FLASH_POWER_PIN);
    // Modem en
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GL865_POWER_PIN;
    GPIO_Init(GL865_POWER_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(GL865_POWER_PORT, GL865_POWER_PIN);
    setMaskForLPM(GL865_POWER_PORT, GL865_POWER_PIN);
    // GPRS reset
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);
    setMaskForLPM(GPIOA, GPIO_Pin_11);
    // opt en
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = OPT_ENABLE_PIN;
    GPIO_Init(OPT_GPIO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(OPT_GPIO_PORT, OPT_ENABLE_PIN);
    setMaskForLPM(OPT_GPIO_PORT, OPT_ENABLE_PIN);
    // opt tx
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = OPT_TX_PIN;
    GPIO_Init(OPT_GPIO_PORT, &GPIO_InitStructure);
    GPIO_SetBits(OPT_GPIO_PORT, OPT_TX_PIN);
    setMaskForLPM(OPT_GPIO_PORT, OPT_TX_PIN); 
    //
    // IN
    //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    // вскрытие корпуса
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = BOX_PIN;
    GPIO_Init(BOX_GPIO_PORT, &GPIO_InitStructure);
    setMaskForLPM(BOX_GPIO_PORT, BOX_PIN);
    // вскрытие бат. отсека
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = BATBOX_PIN;
    GPIO_Init(BATBOX_GPIO_PORT, &GPIO_InitStructure);   
    setMaskForLPM(BATBOX_GPIO_PORT, BATBOX_PIN);
    // Герконы (воздействие магнитом)
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = REED_SWITCH_1_PIN;
    GPIO_Init(REED_SWITCH_1_GPIO_PORT, &GPIO_InitStructure);
    setMaskForLPM(REED_SWITCH_1_GPIO_PORT, REED_SWITCH_1_PIN);
    //
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = REED_SWITCH_2_PIN;
    GPIO_Init(REED_SWITCH_2_GPIO_PORT, &GPIO_InitStructure);
    setMaskForLPM(REED_SWITCH_2_GPIO_PORT, REED_SWITCH_2_PIN);
    // lock
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = LOCK_PIN;
    GPIO_Init(LOCK_GPIO_PORT, &GPIO_InitStructure);
    setMaskForLPM(LOCK_GPIO_PORT, LOCK_PIN);
    // head control
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = HEAD_CONTROL_PIN;
    GPIO_Init(HEAD_CONTROL_GPIO_PORT, &GPIO_InitStructure);
    setMaskForLPM(HEAD_CONTROL_GPIO_PORT, HEAD_CONTROL_PIN);
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция управляет перключателем батареи А
*/
void switchBatA(bool set)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(SWITCH_BAT_A_RCC_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = SWITCH_BAT_A_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(SWITCH_BAT_A_GPIO_PORT, &GPIO_InitStructure);
    if(set)
        GPIO_SetBits(SWITCH_BAT_A_GPIO_PORT, SWITCH_BAT_A_PIN);    
    else
        GPIO_ResetBits(SWITCH_BAT_A_GPIO_PORT, SWITCH_BAT_A_PIN);
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция управляет перключателем батареи В
*/
void switchBatB(bool set)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(SWITCH_BAT_B_RCC_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = SWITCH_BAT_B_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(SWITCH_BAT_B_GPIO_PORT, &GPIO_InitStructure);
    if(set)
        GPIO_SetBits(SWITCH_BAT_B_GPIO_PORT, SWITCH_BAT_B_PIN);    
    else
        GPIO_ResetBits(SWITCH_BAT_B_GPIO_PORT, SWITCH_BAT_B_PIN);
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция анализирует состояние пина, значение 1 которого обозначает,
 что питание опустилось ниже 3.1 В
*/
bool isBatALowPower(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(SWITCH_DISINCREASY_RCC_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = SWITCH_DISINCREASY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(SWITCH_DISINCREASY_GPIO_PORT, &GPIO_InitStructure);
    return GPIO_ReadInputDataBit(SWITCH_DISINCREASY_GPIO_PORT, SWITCH_DISINCREASY_PIN);
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция выставляет питание либо от одной либо другой батареи
*/
void controlPower(void)
{
    static int count = 0;
    if(isBatALowPower())
    {
        switchBatA(false);
        switchBatB(true);          
        count++;
        if(count == 30)
            setEvent(EV_BAT1_LOW);
        activeBat = 2;
    }
    else
    {
        count = 0;        
        switchBatA(true);
        switchBatB(false);  
        activeBat = 1;
    }
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция перехода в режим пониженного энергопотребления.
*/
void LowPowerON(void)
{    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC |
                          RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, ENABLE);
    
    setPinsForLpm();
    
    GPIO_InitTypeDef GPIO_InitStructure;    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~maskForLPM[indxGpioA] & ~SYS_JTMS_SWDIO & ~SYS_JTCK_SWCLK & ~SYS_JTDI;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~maskForLPM[indxGpioB];
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~maskForLPM[indxGpioC];
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~maskForLPM[indxGpioD];
    GPIO_Init(GPIOD, &GPIO_InitStructure);    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~maskForLPM[indxGpioE];
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~maskForLPM[indxGpioH];
    GPIO_Init(GPIOH, &GPIO_InitStructure);
    //InitBattery();
    
    RCC_DeInit();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);
    while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET);
   
    PWR_UltraLowPowerCmd(ENABLE);
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); 
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция выхода из режима пониженного энергопотребления.
 Настройка микроконтроллера в штатный  режим работы
*/
void LowPowerOFF(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);
    while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET);
    
    InitHSIClock();    
    PWR_UltraLowPowerCmd(DISABLE); 
    /// todo раскоментировать когда будет установлен монитор питания    
    // автоматическое переключение на резервное питание при необходимости
    // controlPower();
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Инициализация пина requestBat
*/
void initRequestBat(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(SWITCH_REQ_BAT_RCC_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = SWITCH_REQ_BAT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(SWITCH_REQ_BAT_GPIO_PORT, &GPIO_InitStructure);
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Установка 1 на пине requestBat
*/
void setRequestBat(void)
{
    GPIO_SetBits(SWITCH_REQ_BAT_GPIO_PORT, SWITCH_REQ_BAT_PIN);
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Функция сброса остаточного срока службы при установке новой батареи
*/
void setNewBat(void)
{
    params.volBat1 = def.volBat1;
    params.lifeBat1 = def.lifeBat1;
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Установка 0 на пине requestBat
*/
void resetRequestBat(void)
{
    GPIO_ResetBits(SWITCH_REQ_BAT_GPIO_PORT, SWITCH_REQ_BAT_PIN);
}
/**
 \ingroup mdPowerGroup
 \author Shuvalov Sergey
 \brief Получение напряжения на батареи
*/
float getVoltageBat(int numBat)
{
    float res = 0;
    float min = 150;
    float max = -100;
    float sum = 0;
    float temp = 0;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    uint8_t ADC_ch;
    if(numBat == BAT_A)
    {
        ADC_ch = ADC_Channel_10;
    }
    else if(numBat == BAT_B)
    {
        ADC_ch = ADC_Channel_11;
    }
    else
        return 0;
    ADC_InjectedChannelConfig(ADC1, ADC_ch, 1, ADC_SampleTime_384Cycles);
    ADC_Cmd (ADC1, ENABLE);

    // измерение сигнала
    for(int i = 0; i<10; ++i)
    {
        ADC_SoftwareStartInjectedConv(ADC1);
        int count = 0;

        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC)) // ожидание завершения преобразования
        {
            count++;

            if(count > 1000)
            {
                return (float)0;
            }
        }
        
        res = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
        // Сигнал по схеме макс. = 1.5 В, что соответствуeт макс. напряжению батареи 3.6В
        // 2 - 2 В питание MCU, 4096 - 12 бит
        /// todo установить правильные данные вместо "(3.3/1.85)"
        temp = res * (2. / 4096) * (3.3/1.85);
        sum += temp;

        if(temp <= min)
        {
            min = temp;
        }

        if(temp >= max)
        {
            max = temp;
        }
    }

    sum -= max + min;
    temp = sum / 8;
    ADC_Cmd (ADC1, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
    return temp;
}


