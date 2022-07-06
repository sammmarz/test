/**
* \file
* Файл иницииализирует и создает все необходимое перед работой прошивки
*/

/*! \mainpage ПО ВК-ЭСУ

Программа состоит из нескольких модулей с четким разграничением функционала.
После инициализации необходимой периферии, создания ресурсов контроллер переходит в режим пониженного энергопотребления и ожидает прерываний.


Модули:
- \ref mdModes - контроль активности режимов, переход\выход в режим пониженного энергопотребления.
- \ref mdDisplay - обработка нажатия кнопок пользователем, работа с навигацией и дисплеем.
- \ref mdPower - работа с питанием микроконтроллера.
- \ref mdRTC - работа часами.
- \ref mdDebug - отладка. Работа с функциями из данного модуля не должна идти в релиз версию.
- \ref mdFlashMemory - работы с флэш-памятью.
- \ref mdHardware - настройка микроконтроллера.
- \ref mdOpticalInterface - работа с оптическим интерфейсом.
- \ref mdTemperSensor - работа с датчиком температуры.
- \ref mdDataManager - централизованное управление данными (формирование, хранение, отправка).
- \ref mdHall - работа с датчиками Холла.
- \ref mdFTP - обеспечение обмена данными по FTP.
*/

#include <arm_itm.h>
#include <stdio.h>
#include "stm32l1xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
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
#include "reprogram.h"

#include "semphr.h"


#include "mdDisplay/DisplayFunctions.h"                                         // for showStart0ScreenDebug, showGroup0ScreenDebug, showDisplayBuffer, turnOnDisplay

//otlad
void writeDataInFlash(StrQueueInfoData *bufInfo);
void fillFlashTestData(void);
#include "mdLIS200/mdLis200.h"
#include <string.h>
extern StrDataUART optData;
// otlad end
//otlad
int resetReason;

//otlad end
/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

//==============================================================================
xSemaphoreHandle xSPI1semaphore;
SemaphoreHandle_t xMutexParams;
TaskHandle_t xTaskInitAll;
// otlad
uint8_t rdata[512];
extern const unsigned short CRCInit;
unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);
// otlad test
/**
\author Shuvalov Sergey
 \brief Поток для инициализации. После инициализации MCU и FreeRTOS прекращает работать.
*/

void initAll(void *arg)
{
    // при выходе из LPM 4 МГц
    RCC_MSIRangeConfig(RCC_MSIRange_6);  
 
    //определение причины перезагрузки
    resetReason = (RCC->CSR >> 16) & 0xffff;  
    RCC_ClearFlag();   
    vTaskDelay(5);
    //   
    InitHSIClock();

    vTaskDelay(5);
    /// todo раскоментировать когда будет установлен монитор питания 
    // автоматическое переключение на резервное питание при необходимости
    //controlPower();
    activeBat = 1; // временно
    //
    /// todo раскоментировать 
    //initWatchdog();
    
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
    
    
    
    xModesQueue = xQueueCreate (QUEUE_MODES_SIZE, sizeof(StrEventMode));
    xBtnsQueue = xQueueCreate (10, sizeof(StrEventForDisplay));
    xDataManagerQueue = xQueueCreate(QUEUE_DT_MANAGER_SIZE, sizeof(StrDataManager));
    xFlashWriteQueue = xQueueCreate(QUEUE_FLASH_SIZE, sizeof(StrQueueInfoData));
    xOptBufQueue = xQueueCreate(256, 1);
    xFTPcmdQueue = xQueueCreate(1, 1);
    vSemaphoreCreateBinary(xSPI1semaphore);
    xSemaphoreGive(xSPI1semaphore);
    xMutexParams = xSemaphoreCreateMutex ();
    xTemperSensorQueue = xQueueCreate(3, 1); //достаточно 2, 3 на всякий
    USART3_RX_Buf_Q = xQueueCreate(MODEM_BUF_SIZE, 1);
    //
    // flash
    initGpioForFlashPower();
    initFlashSPI();
    initInfoStructDataForFlash();
    enableFlashPower();
    // Datasheet: (Device is fully accessible after 10 mlSec)
    vTaskDelay(10);
    //eraseFlashMemory(paramsInfo.adressFlashBegin, paramsInfo.adressFlashEnd);
//#define testDevice
#ifdef testDevice
    int errorCount = 0;
    if(!testFlash())
    {
        errorCount++;
    }
    if(!testTemperSensor())
    {
        errorCount++;
    }
#endif
    
//    if(initParamsFromFlash())
//    {
//        setEvent(EV_DATA_UPDATE);
//    }
//    else
    {
        initParamsDefault();
    }
    
  /*  setEvent(EV_TIMER_NOT_SET);
    setEvent(EV_RESET);

    if (result_prog == 0xBEDA)  setEvent(EV_ERROR_REPROGRAMM);
    if (result_prog == 0xA5A5)  setEvent(EV_REPROGRAMM); */
    //
    // концевики
    
    /* осторожно если использовать прерывания то в функциях
    controlBatBoxOpen и controlBoxOpen задержки по пол сек.
    initControlBoxOpenInterrupt();
    initControlBatBoxOpenInterrupt();
    */
    
    //
    // инициализация клавиатуры
    initUserButtons();
    setMaskForLPM(GPIOB, BTN_LEFT_PIN | BTN_CENTER_PIN | BTN_RIGHT_PIN);
    //
    //датчик температуры
    initMCUforTemperSensor();
    enableTemperSensor();   
    vTaskDelay(2); // время для готовности датчика
    configTemperSensor();       
    initTemperSensorInterrupt();    
    startConversionTemperSensor();

    vTaskDelay(130);  
//    params.Ttek = 0;
//    getTemperFromSensor(&params.Ttek);
    //
    // датчики Холла
    initHallSensors();
    //    
    
    xTaskCreate(DisplayDriver, "DISPLAY_DRIVER", configMINIMAL_STACK_SIZE+100, NULL,
                configMAX_PRIORITIES - 3, & xTaskDisplayDriver );
    xTaskCreate(dataControlThread, "DATA_CONTROL", configMINIMAL_STACK_SIZE+100, NULL,
                configMAX_PRIORITIES - 3, & xTaskDataManager );
    xTaskCreate(TemperSensorTask, "TEMPER_SENSOR", configMINIMAL_STACK_SIZE+100, NULL,
                configMAX_PRIORITIES - 3, & xTemperSensor );
    xTaskCreate(WriteToFlashThread, "FLASH_THREAD", configMINIMAL_STACK_SIZE+100, NULL,
                configMAX_PRIORITIES - 2, & xTaskWriteBufferToFlash );
    xTaskCreate(opticalInterfaceTask, "OPTICAL_INTERFACE", configMINIMAL_STACK_SIZE+100, NULL,
                configMAX_PRIORITIES - 3, & xTaskOpticalInterface );
    xTaskCreate(TaskFTP, "FTP_INTERFACE", configMINIMAL_STACK_SIZE+512, NULL,
                configMAX_PRIORITIES - 3, & xTaskFTP );

    xTaskCreate(CheckModes, "CHECK_MODES", configMINIMAL_STACK_SIZE+100, NULL, configMAX_PRIORITIES - 1,
                & xTaskCheckModes );
    
    // RTC
    initRTC(&params);
    //
    //
    StrEventForDisplay btnPush;
    btnPush.codeEventForDisplay = LEFT_BTN;   
    xQueueSendToBack(xBtnsQueue, &btnPush, NULL);
    //
    StrEventMode modeBTN;
    modeBTN.status = 1;
    modeBTN.modeCode = DISPLAY_M;
    xQueueSendToBack(xModesQueue, &modeBTN, NULL);
  
    vTaskDelete(NULL);
}
/**
 \author Shuvalov Sergey
 \brief Точка входа в программу.
*/
void main()
{
    // для нормальной работы пина PH1, иначе он настроен на работу HSE
    RCC->CR &= ~((uint32_t)RCC_CR_HSEON);
    
    xTaskCreate(initAll, "INIT_ALL", configMINIMAL_STACK_SIZE+300, NULL, configMAX_PRIORITIES,
                &xTaskInitAll );
    
    //стартуем планировщик FreeRTOS
    vTaskStartScheduler();  
}

void vApplicationMallocFailedHook( void )
{
    printf("vApplicationMallocFailedHook");
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();

    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    //printf("vApplicationIdleHook");
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();

    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    // printf("t");
    /* This function will be called by each tick interrupt if
    configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
    added here, but the tick hook is called from an interrupt context, so
    code must not attempt to block, and only the interrupt safe FreeRTOS API
    functions can be used (those that end in FromISR()). */
}
/*-----------------------------------------------------------*/

void vAssertCalled( unsigned long ulLine, const char *const pcFileName )
{
    volatile unsigned long ulSetToNonZeroInDebuggerToContinue = 0;
    // Parameters are not used.
    ( void ) ulLine;
    ( void ) pcFileName;
    taskENTER_CRITICAL();
    {
        while( ulSetToNonZeroInDebuggerToContinue == 0 )
        {
            // Use the debugger to set ulSetToNonZeroInDebuggerToContinue to a
            // non zero value to step out of this function to the point that raised
            // this assert().
            __asm volatile( "NOP" );
            __asm volatile( "NOP" );
        }
    }
    taskEXIT_CRITICAL();
}

