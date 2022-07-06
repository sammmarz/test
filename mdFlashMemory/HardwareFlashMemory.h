#ifndef HardwareFlashMemory
#define HardwareFlashMemory

#include "stm32l1xx.h"

#define FLAG_FLASH_BUSY (1<<0)
#define FLAG_FLASH_WRITE_ENABLE (1<<1)

#define GET_FLASH_STATUS_REGISTER_1 0x05
#define SET_FLASH_READ_DATA 0x03
#define SET_FLASH_WRITE_ENABLE 0x06
#define SET_FLASH_PAGE_PROGRAM 0x02
#define SET_FLASH_ERASE_BLOCK 0x20 /* erase 0x0- 0xfff*/
#define DUMMY_BYTE 0xA5

uint8_t SendReceiveSPIByte(SPI_TypeDef *SPIx, uint8_t byte);
//void sendSPIByte(SPI_TypeDef *SPIx, uint8_t byte);
void waitFlashToFree(void);
void prepareToWriteDataInFlash(void);
void sendFlashAdress(int adress);
void eraseBlock(int adress);
// инициализация
void initGpioForFlashPower(void);
void enableFlashPower(void);
void disableFlashPower(void);
void initFlashSPI(void);
#endif