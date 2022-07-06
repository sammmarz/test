#include "FreeRTOS.h"
#include "mdHardware/mdHardware.h"
#include "mdModes/mdModes.h"
#include "mdPower/mdPower.h"
#include "mdRTC/mdRTC.h"
#include "mdDisplay/mdDisplay.h"
#include "mdTemperSensor/mdTemperSensor.h"
#include "mdTemperSensor/HardwareTemperSensor.h"
#include "mdHall/mdHall.h"
#include "mdDataManager/mdDataManager.h"
#include "mdFlashMemory/mdFlashMemory.h"
#include "mdFlashMemory/DataForFlash.h"
#include "mdFlashMemory/HardwareFlashMemory.h"
#include "mdPrintToUART/printToUART.h"
#include "mdOpticalInterface/mdOpticalInterface.h"
#include "mdFTP/mdFTP.h"

void initPinsDefault(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC |
                          RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_Init(GPIOH, &GPIO_InitStructure);
}
//============================================================================
void initPH2(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOH, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOH, &GPIO_InitStructure);    
    setMaskForLPM(GPIOH, GPIO_Pin_2);
}
void upPH2(void)
{
    initPH2();
    GPIO_SetBits(GPIOH, GPIO_Pin_2);
}
void downPH2(void)
{
    initPH2();
    GPIO_ResetBits(GPIOH, GPIO_Pin_2);
}
//============================================================================

//============================================================================
void upPB14(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

void downPB14(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void upPC11(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_11);
}

void downPC11(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_11);
}
bool testTemperSensor(void)
{
    initMCUforTemperSensor();
    enableTemperSensor();
    vTaskDelay(2); // время для готовности датчика
    configTemperSensor();       
    initTemperSensorInterrupt();
    startConversionTemperSensor();
    vTaskDelay(130);  
    params.Ttek = 0;
    getTemperFromSensor(&params.Ttek, &params.TtekBin);
    if(params.Ttek < 2)
        return false;
    else 
        return true;
}
//    // otlad
//    extern uint16_t adressLis200;
//    extern uint16_t instansLis200;
//    adressLis200 = 0x0A60;
//    instansLis200 = 3;
//    char *ptr = "0x5102";
//    int indx = GetIndex (0xA60, 1, 0);
//    WrtValueForTrigger(indx, ptr);
//    // otlad end
//    //otlad
//    eraseFlashMemory(paramsInfo.adressFlashBegin, paramsInfo.adressFlashEnd);
//    eraseFlashMemory(intervalArchiveInfo.adressFlashBegin, intervalArchiveInfo.adressFlashEnd);
//    intervalArchiveInfo.adress = intervalArchiveInfo.adressFlashBegin
//    //otlad end

/*
    eraseFlashMemory(intervalArchiveInfo.adressFlashBegin, intervalArchiveInfo.adressFlashEnd);
    eraseFlashMemory(changesArchiveInfo.adressFlashBegin, changesArchiveInfo.adressFlashEnd);
    eraseFlashMemory(eventsArchiveInfo.adressFlashBegin, eventsArchiveInfo.adressFlashEnd);    
    eraseFlashMemory(paramsInfo.adressFlashBegin, paramsInfo.adressFlashEnd);        
    eraseFlashMemory(eventsArchiveInfo.adressFlashBegin, eventsArchiveInfo.adressFlashEnd);
    eraseFlashMemory(monthArchiveInfo.adressFlashBegin, monthArchiveInfo.adressFlashEnd);
    eraseFlashMemory(dayArchiveInfo.adressFlashBegin, dayArchiveInfo.adressFlashEnd);
    fillFlashTestData();
*/    