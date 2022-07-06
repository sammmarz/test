#include "FreeRTOS.h"
#include "queue.h"
#include "stm32l1xx.h"
#include "stm32l1xx_usart.h"

#define PERIRH_INTERRUPT_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1)
#define USART_RECIEVE_BUF_LEN 64
//приемный буфер для USART1
//static xQueueHandle recieveBuf;

void send_usart1_cpu(uint8_t *buf, int len);

int MyLowLevelPutchar(int x)
{
    send_usart1_cpu((uint8_t *)&x, 1);
    return x;
}

void initUSARTforHyperTerminal(void)
{
    //recieveBuf = xQueueCreate(USART_RECIEVE_BUF_LEN, 1);
    //инициализация ног
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //USART1 Rx и CTS as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //инициализация UART1
    USART_InitTypeDef USART_InitStructure =
    {
        .USART_BaudRate = 9600,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx
    };
    USART_Init(UART4, &USART_InitStructure);
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    //USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(UART4, ENABLE);
    //настройка NVIC прерывания по RX
    NVIC_SetPriority(UART4_IRQn, 10);
    NVIC_EnableIRQ(UART4_IRQn);
}

void send_usart1_cpu(uint8_t *buf, int len)
{
    while (len)
    {
        USART_SendData(UART4, *buf++);

        while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
            ;

        len--;
    }
}

