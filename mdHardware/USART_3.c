/**
 \file
Файл содержит функции инициализации USART3 для работы с модемом.
*/
#include "mdHardware.h"

xQueueHandle USART3_RX_Buf_Q;

/**
 \ingroup mdHardwareGroup
 \author Borodov Maksim
 \brief Init usart USART3
*/
void initUSART_3(int speed, uint16_t USART_Parity)
{
    //USART3_RX_Buf_Q = xQueueCreate(MODEM_BUF_SIZE, 1);
    xQueueReset(USART3_RX_Buf_Q);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);                        // APB1 sources USART3
    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
    USART_Cmd(USART3, DISABLE);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);                     // !!! use GPIO_PinSource8 instead of MODEM_TX_PIN
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);                     // !!! use GPIO_PinSource9 instead of MODEM_RX_PIN
    USART_InitTypeDef USART_InitStructure =
    {
        .USART_BaudRate = speed,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,//USART_HardwareFlowControl_RTS_CTS,//USART_HardwareFlowControl_None,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx
    };
    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    // USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);                             // DMA
    USART_Cmd(USART3, ENABLE);
}

/**
 \ingroup mdHardwareGroup
 \author Borodov Maksim
 \brief Check USART3 CTS
*/
bool isCts_usart3()
{
    return !GPIO_ReadInputDataBit(MODEM_GPIO_PORT, MODEM_CTS_PIN);
}

/**
 \ingroup mdHardwareGroup
 \author Borodov Maksim
 \brief Check USART3 CTS
*/
bool noCTS_usart3(portTickType TimeToWait)
{
    portTickType end_of_wait = xTaskGetTickCount() + (TimeToWait);                // Get the MAX time to wait till

    while (true)
    {
        if (isCts_usart3())                                                         // got CTS? exit!
        {
            return false;
        }

        vTaskDelay(10);                                                             // Wait some time

        if (xTaskGetTickCount() >= end_of_wait)                                     // Waiting too long
        {
            return true;
        }
    }
}

/**
 \ingroup mdHardwareGroup
 \author Borodov Maksim
 \brief Set USART3 RTS
*/
void rtsUp_usart3()
{
    GPIO_SetBits(MODEM_GPIO_PORT, MODEM_RTS_PIN);
}

void rtsDown_usart3()
{
    GPIO_ResetBits(MODEM_GPIO_PORT, MODEM_RTS_PIN);
}

bool recv_usart3(uint8_t *buf, portTickType xTicksToWait)
{
    return xQueueReceive(USART3_RX_Buf_Q, buf, xTicksToWait);
}

void USART3_IRQHandler()
{
    uint16_t status = USART3->SR;                                                 // Read status register
    uint8_t byte = USART3->DR & (uint16_t) 0x01FF;                                // Read byte and reset ORE flag

    if ((USART3->CR1 & USART_CR1_PCE))                                            // Mask even bit
        if (!(USART3->CR1 & USART_CR1_M))
        {
            byte &= 0x7F;
        }

    if (status & USART_FLAG_RXNE)
    {
        // Got data
        portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendToBackFromISR(USART3_RX_Buf_Q, &byte, &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
}

/**
 \ingroup mdDisplayGroup
 \author Borodov Maksim
 \brief Send data to modem via USART3
*/
#pragma optimize=none
void sendDataUsart3(uint8_t *buf, int len)
{
    while (len)
    {
        USART_SendData(USART3, *buf++);

        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
            ;

        len--;
    }
}
