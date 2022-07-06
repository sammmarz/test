#ifndef reprogram_h
#define reprogram_h

#include "stdio.h"
#include "stdint.h"

#include "_Import/CMSIS/stm32l1xx.h"
#include "_Import/SPL/inc/stm32l1xx_flash.h"
#include "_Import/SPL/inc/stm32l1xx_gpio.h"
#include "mdHardware/mdHardware.h"
#include "mdFlashMemory/HardwareFlashMemory.h"

#define START_FIRMWARE   0x783000
#define ADR_STRUCT       0x783200
#define SIZE_PAGE_FLASH  256
#define N_PAGE_MEM       2048

extern uint16_t result_prog;
extern void reprogramming();

#endif