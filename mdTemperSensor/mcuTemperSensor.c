/**
 \file
Файл содержит функции для взаимодействия с термодатчиком микроконтроллера.
*/
#include "stm32l1xx.h"
#include "stm32l1xx_rtc.h"
#include "mdHardware/mdHardware.h"

/**
 \ingroup mdTemperSensorGroup
 \author Shuvalov Sergey
 \brief Функция возвращает значение датчика температуры.
*/
float getMCUTemperSensorValue(void)
{
    float res = 0;
    float min = 150;
    float max = -100;
    float sum = 0;
    float temp = 0;
    // берем калибровочные значения (адреса из даташита)
    uint16_t *localTS_CAL1 = (uint16_t *)0x1FF800FA;
    uint16_t *localTS_CAL2 = (uint16_t *)0x1FF800FE;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    ADC_TempSensorVrefintCmd(ENABLE);
    int T_StartupTimeDelay = 1024;

    while (T_StartupTimeDelay--);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_384Cycles);
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
                return (float)100;
            }
        }

        // 1.5 - так как у нас напряжение 2В а калиб.коэфф. получены при 3В
        res = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1) / 1.5;
        ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
        temp = ((110-30) / (double)(*localTS_CAL2 - *localTS_CAL1)) * (res- *localTS_CAL1) + 30;
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