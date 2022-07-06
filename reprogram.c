/**
 \file
Файл содержит функции для обеспечения перезаписи файла прошива из внешней во внутреннюю память
*/
#include "reprogram.h"
#include "semphr.h"
extern xSemaphoreHandle xSPI1semaphore;
unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);
extern const unsigned short CRCInit;

__no_init uint16_t result_prog @0x20007ABC; // признак успешного окончания перепрошивки
/**
 \ingroup mdReprogram
 \author Melnikov Sergey
 \brief Функция разблокировки внутренней флэш

*/
void init_flash_ram() 
{
    //разблокируем регистр PECR
    FLASH->PEKEYR = FLASH_PEKEY1;//0x89ABCDEF;
    FLASH->PEKEYR = FLASH_PEKEY2;//0x02030405;
    
    //разблокируем память программ
    FLASH->PRGKEYR = FLASH_PRGKEY1;//0x8C9DAEBF;
    FLASH->PRGKEYR = FLASH_PRGKEY2;//0x13141516;
  
}
/**
 \ingroup mdReprogram
 \author Melnikov Sergey
 \brief Функция записи страницы (256 байт) внутренней флэш. Запись ведется словами по 4 байта

*/

__ramfunc void write_flash_ram(uint32_t* pageAdress,uint8_t* data)
{
    //ожидаем освобождения флэш
    while (FLASH->SR & FLASH_SR_BSY);
    
    //выставляем флаг записи половины страницы(по факту дает и записать целую страницу памяти)
    FLASH->PECR |= FLASH_PECR_FPRG;
    //пишем 64 слова по 4 байта(256 байт) во флэш
    for (unsigned int i = 0; i < 64; i++)
    {
        *(pageAdress) = ((uint32_t*)data)[i];
        //ожидаем освобождения флэш
        while (FLASH->SR & FLASH_SR_BSY);
        
        pageAdress++;
    }
    FLASH->PECR &= ~FLASH_PECR_FPRG;
}

/**
 \ingroup mdReprogram
 \author Melnikov Sergey
 \brief Функция очистки страницы (256 байт) внутренней флэш

*/

 __ramfunc void erase_flash_ram(uint32_t pageAdress) 
{
   //ожидаем освобождения флэш
    while (FLASH->SR & FLASH_SR_BSY);
   
    //выставляем флаг очистки страницы
    FLASH->PECR |= FLASH_PECR_ERASE;
    FLASH->PECR |= FLASH_PECR_PROG;
 
    *(__IO uint32_t *)(pageAdress) = 0x00000000;
    //while (FLASH->SR & FLASH_SR_BSY);
     while (!(FLASH->SR & FLASH_SR_EOP));
     
     FLASH->SR |=  FLASH_SR_EOP;
	

    FLASH->PECR &= ~(FLASH_PECR_PROG);
    FLASH->PECR &= ~(FLASH_PECR_ERASE);
}

/**
 \ingroup mdReprogram
 \author Melnikov Sergey
 \brief Функция отправки/чтения данных с SPI1

*/

__ramfunc uint8_t SendReceiveSPIByte_ram(SPI_TypeDef *SPIx, uint8_t byte)
{
    while ( (SPI1->SR & SPI_I2S_FLAG_TXE) == RESET)
        ;
   
    
    SPI1->DR = byte;
   // for(int i = 0; i < 5;i++);
    while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET)
       ;
    
    SPI1->SR &= ~SPI_I2S_FLAG_RXNE;
    
    return (SPI1->DR);
}
/**
 \ingroup mdReprogram
 \author Melnikov Sergey
 \brief Функция ожидания освобождения внешней флэш

*/

__ramfunc void waitFlashToFree_ram(void)
{
    
    while (1)
    {
        
        FLASH_GPIO_PORT-> BSRRH = FLASH_PIN_CS;
        SendReceiveSPIByte_ram(SPI1, GET_FLASH_STATUS_REGISTER_1);
        uint8_t status = SendReceiveSPIByte_ram(SPI1, DUMMY_BYTE);
        FLASH_GPIO_PORT->BSRRL = FLASH_PIN_CS;
        
        if((status & FLAG_FLASH_BUSY) == 0)
        {
            break;
        }
    }
}
/**
 \ingroup mdReprogram
 \author Melnikov Sergey
 \brief Функция чтения страницы внешней памяти

*/
__ramfunc void ram_sendFlashAdress(int adress)
{
    //передача адреса
    SendReceiveSPIByte_ram(SPI1, (adress >> 16) & 0xff);
    SendReceiveSPIByte_ram(SPI1, (adress >> 8) & 0xff);
    SendReceiveSPIByte_ram(SPI1, adress & 0xff);
}
__ramfunc void  ReadPageInterFlash(uint8_t* read_buf, uint32_t adr_begin)
{       
    waitFlashToFree_ram();
    FLASH_GPIO_PORT-> BSRRH = FLASH_PIN_CS;
    SendReceiveSPIByte_ram(SPI1, SET_FLASH_READ_DATA);
    //ram_sendFlashAdress(adr_begin);
    //выставление адреса
    SendReceiveSPIByte_ram(SPI1, (adr_begin >> 16) & 0xff);
    SendReceiveSPIByte_ram(SPI1, (adr_begin >> 8) & 0xff);
    SendReceiveSPIByte_ram(SPI1, adr_begin & 0xff);

    //SendReceiveSPIByte_ram(SPI1,DUMMY_BYTE);
   // for(int i=0; i<10000; i++);
    for(int i=0; i<SIZE_PAGE_FLASH; i++)
    {
        read_buf[i] = SendReceiveSPIByte_ram(SPI1, DUMMY_BYTE);
       // count_err++;
    }
    FLASH_GPIO_PORT->BSRRL = FLASH_PIN_CS;
    
}
/**
 \ingroup mdReprogram
 \author Melnikov Sergey
 \brief Функция постранично читает данные с внешней флэш и переносит их на внутренний накопитель 
*/


unsigned short CRCInit_ram = 0xFFFF; //Initial value of CRC

 unsigned short CRC16Table_ram [] = 		//CRC16 table based on 0xA001 polynom, revers CRC alghorithm
{
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};//CRC16Table []



__ramfunc  unsigned short GetCRC16_ram(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1)
{
    unsigned short crc = CRCInit1;

    while (p_Length--)
    {
        crc = CRC16Table_ram[(crc ^ *p_Buff++) & 0xFF] ^ (crc >> 8);
    }

    return ~crc;
}

__ramfunc void   ReadWriteFlash()
{
    uint32_t ind = 0;  
    uint32_t adr_erase = 0x00;
    uint32_t adr_read = START_FIRMWARE;
    uint32_t size_firm;
    uint32_t size_firm_page;
    uint8_t buf_read[SIZE_PAGE_FLASH];
    uint16_t crc,crc_pack;
    
       
    
    //читаем из внешней памяти страницу,содержащую  размер прошивки в байтах
    ReadPageInterFlash(buf_read,ADR_STRUCT);
    crc = GetCRC16_ram(buf_read, 54, CRCInit_ram);
    crc_pack = (buf_read[55] << 8) | (buf_read[54]);//((uint16_t*)buf_read)[26];
    //определяем размер файла прошива в страницах
    size_firm = ((uint32_t*)buf_read)[11];
    size_firm_page = size_firm / SIZE_PAGE_FLASH;
    if(size_firm % SIZE_PAGE_FLASH) size_firm_page++;  
    //переписываем  size_firm_page страниц из внешней флэш во внутреннюю
     if(crc == crc_pack){
       //постраничная очистка всей внутренней памяти
       for (ind = 0; ind < N_PAGE_MEM; ind++)
    {
       erase_flash_ram(adr_erase); 
       adr_erase += SIZE_PAGE_FLASH;
       IWDG->KR = 0xAAAA;
    }
    adr_erase = 0;
    
        for(ind = 0; ind <= (size_firm_page); ind++)
        {  
            ReadPageInterFlash(buf_read,adr_read); 
            write_flash_ram((uint32_t*) adr_erase,buf_read);
            adr_read += SIZE_PAGE_FLASH;
            adr_erase += SIZE_PAGE_FLASH; 
            IWDG->KR = 0xAAAA;
        
        }
    } else{
        result_prog = 0xBEDA;
        __DSB();
        
        SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | (SCB ->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | (SCB_AIRCR_VECTRESET_Msk)); 
        __DSB();
    
        while(1)
        ;
    }
}


/**
 \ingroup mdReprogram
 \author Melnikov Sergey
 \brief Функция инициирует перенос данных с внешней на внутреннюю флэш и отправляет МК в RESET

*/
__ramfunc void appy_ram_code()
{       
    //read built-in memory and write internal flash
    ReadWriteFlash();
    //выставление признака конца перепрограммирования
    result_prog = 0xA5A5;
    //GPIOB->BSRRL = GPIO_Pin_14;
   // рестарт
    
   __DSB();
    /* Keep priority group unchanged  0x5FA -разблокировка*/
    SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | (SCB ->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | (SCB_AIRCR_VECTRESET_Msk)); 
   __DSB();/* Ensure completion of memory access */
   
    while(1)
      ;
    
}
/**
 \ingroup mdReprogram
 \author Melnikov Sergey
 \brief 
  Диспетчет перепрограммирования. Функция отключает прерывания, разблокирует внутреннюю флэш-память,
  включает HSI и вызывает функцию применения прошивки 

*/
void reprogramming()
{
    //off interupt
   // __disable_irq();
    __disable_interrupt();
   NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
    NVIC->ICER[2] = 0xFFFFFFFF;
    NVIC->ICER[3] = 0xFFFFFFFF;
    NVIC->ICER[4] = 0xFFFFFFFF;
    NVIC->ICER[5] = 0xFFFFFFFF;
    NVIC->ICER[6] = 0xFFFFFFFF;
    NVIC->ICER[7] = 0xFFFFFFFF;
    //unlock flash
    init_flash_ram();
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR);
    //timer and watchdog
    RCC_HSICmd(ENABLE);
    IWDG->KR = 0xAAAA;
    //сброс флага конца перепрограммирования
    result_prog = 0;
    //непосредственно процедура перепрошива внутренней флэш
    
    appy_ram_code();
  
}