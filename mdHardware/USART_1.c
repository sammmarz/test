/**
 \file
Файл содержит функции инициализации USART1 для работы по оптическому интерфейсу.
*/
#include "mdHardware.h"

/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации USART1
*/
void initUSART_1(int speed, uint16_t USART_Parity)
{
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_Cmd(USART1, DISABLE);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    USART_InitTypeDef USART_InitStructure =
    {
        .USART_BaudRate = speed,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx
    };
    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}