/**
 \file
Файл настройки SPI2.
*/

#include "mdHardware.h"
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации SPI2
    При смене частоты поменять задержку выдачи в sendByteToDisplay
*/
void initSPI_2(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    SPI_InitTypeDef spi1;
    spi1.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi1.SPI_Mode=SPI_Mode_Master;
    spi1.SPI_DataSize=SPI_DataSize_8b;
    spi1.SPI_CPOL=SPI_CPOL_High;
    spi1.SPI_CPHA=SPI_CPHA_2Edge;
    spi1.SPI_NSS= SPI_NSS_Soft;
    spi1.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //SPI_BaudRatePrescaler_256;//
    spi1.SPI_FirstBit= SPI_FirstBit_MSB;
    SPI_Init( SPI2, &spi1);
    //GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    //GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
    // /* otlad flash
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_SPI2);
    // end otlad flash*/
    SPI_Cmd( SPI2, ENABLE);
    // SS = 1
    // SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}