/**
 \file
Файл содержит функции для взаимодействия с дисплеем.
*/

#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Fonts5x8.h"
#include "Fonts12x16.h"
#include "StringsWarnings.h"
#include "DisplayFunctions.h"
#include "DisplayHardware.h"
#include "mdHardware/mdHardware.h"
#include "Params/Events.h"

static int page;
static int col;
extern bool optPortStatus;
extern xSemaphoreHandle xSPI1semaphore;
// буфер заполняется перед тем как скопируется в RAM дисплея
unsigned char bufDisplay[8][128] = {0};
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция выводит иконки статусов на экран.
*/
void writeStringWarnings(const CHAR_TYPE *icon, int sumByte, int flags)
{
    for (int colByte = 0; colByte < sumByte; ++colByte)
    {
        int byte = icon[colByte];

        if (flags & REVERS_COLOR)
        {
            byte = ~byte;
        }

        bufDisplay[page][col] = byte;
        col++;
    }

    if (flags & CHAR_PLUS_COL)
    {
        if (flags & REVERS_COLOR)
        {
            bufDisplay[page][col] = 0xff;
        }
        else
        {
            bufDisplay[page][col] = 0;
        }

        col++;
    }
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Инициализация дисплея.
*/
void initDisplay(void)
{
    //initSPI1forDisplay();
    // устанавливаем res в 0
    //DISPLAY_GPIO_PORT->BSRRL = DISPLAY_PIN_RES;
    GPIO_ResetBits(DISPLAY_GPIO_PORT, DISPLAY_PIN_RES);
    //LCD12864B_CS = 1;
    GPIO_SetBits(DISPLAY_GPIO_PORT, DISPLAY_PIN_CS1);
    //LCD12864B_SI = 0;
    //LCD12864B_SCL = 1;
    // задержка 10 мСек как в примере, но в мануале 10 мкрС ???
    vTaskDelay(1);
    // устанавливаем res в 1
    //DISPLAY_GPIO_PORT->BSRRH = DISPLAY_PIN_RES;
    GPIO_SetBits(DISPLAY_GPIO_PORT, DISPLAY_PIN_RES);
    vTaskDelay(1);
    xSemaphoreTake(xSPI1semaphore, portMAX_DELAY);
    sendByteToDisplay(0x2F, DISPLAY_CMD);
    sendByteToDisplay(0x27, DISPLAY_CMD);
    sendByteToDisplay(0x89, DISPLAY_CMD);
    sendByteToDisplay(0x00, DISPLAY_CMD);
    sendByteToDisplay(0x81, DISPLAY_CMD);
    sendByteToDisplay(0x30, DISPLAY_CMD);
    sendByteToDisplay(0x40, DISPLAY_CMD); // dobavil
    sendByteToDisplay(0xA6, DISPLAY_CMD); //0xA6=Normal, 0xA7=Reverse display
    //0xc0
    sendByteToDisplay(0xC8,
                      DISPLAY_CMD); // может нормал\реверс отображение битов в столбце ?????????????
    sendByteToDisplay(0xA0,
                      DISPLAY_CMD); //нормальное (не реверсионное) отображение столбцов
    // sendByteToDisplay(0xAF, DISPLAY_CMD); //Display ON
    xSemaphoreGive(xSPI1semaphore);
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает статусную панель на экране (page 7).
*/
void showStatus(void)
{   
    setPage(7);

    if(CheckEventInStatus(EV_MAGNIT))
    {
        setCol(8+1);
        writeStringWarnings(sMagnitM, sizeof(sMagnitM) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }

    if(CheckEventInStatus(EV_BORDER_ALERT_T))
    {
        setCol(16+2);
        writeStringWarnings(sTemper, sizeof(sTemper) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }

    if(CheckEventInStatus(EV_BORDER_WARN_QP) /*|| CheckEventInRegisterStatus(EV_BORDER_WARN_QC)*/)
    {
        setCol(24+3);
        writeStringWarnings(sQalert, sizeof(sQalert) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }

    /*  if(CheckEventInRegisterStatus())
        {
        setCol(33+4);
        writeStringWarnings(sKlapanHalf, sizeof(sKlapanHalf) / sizeof(CHAR_TYPE), REVERS_COLOR);
        }
    */

    if(CheckEventInStatus(EV_BAT1_LOW))
    {
        setCol(43+5);
        writeStringWarnings(sBatLow, sizeof(sBatLow) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }

    if(CheckEventInStatus(EV_BOX_OPEN))
    {
        setCol(56+6);
        writeStringWarnings(sKorpus, sizeof(sKorpus) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }

    if(CheckEventInStatus(EV_BAT_OPEN))
    {
        setCol(65+7);
        writeStringWarnings(sBatOpen, sizeof(sBatOpen) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }

    if(optPortStatus)
    {
        setCol(93+8);
        writeStringWarnings(sSearchOpt, sizeof(sSearchOpt) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }

    if(CheckEventInStatus(EV_OPT_READY))
    {
        setCol(93+8);
        writeStringWarnings(sConnectOpt, sizeof(sConnectOpt) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }

    /*
    //  для модема
    setCol(79+8);
    writeStringWarnings(sSearch, sizeof(sSearch) / sizeof(CHAR_TYPE), REVERS_COLOR);

    setCol(115);
    writeStringWarnings(sSignal, sizeof(sSignal) / sizeof(CHAR_TYPE), REVERS_COLOR);
    */
    if(CheckEventInStatus(0xFFFF))
    {
        setCol(0);
        writeStringWarnings(sWarning, sizeof(sWarning) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }
    if(CheckEventInRegisterStatus(0xFFFF))
    {
        setCol(119);
        writeStringWarnings(sRegisterStList, sizeof(sRegisterStList) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция включает подсветку дисплея перед формированем изображения
*/
void turnOnDisplay(void)
{
    //xSemaphoreTake(xSPI1semaphore, portMAX_DELAY);
    sendByteToDisplay(0xAF, DISPLAY_CMD); //Display OFN
    //xSemaphoreGive(xSPI1semaphore);
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отключает подсветку дисплея перед формированем изображения
*/
void turnOffDisplay(void)
{
    xSemaphoreTake(xSPI1semaphore, portMAX_DELAY);
    sendByteToDisplay(0xAE, DISPLAY_CMD); //Display OFF
    xSemaphoreGive(xSPI1semaphore);
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Очистка буфера дисплея.
*/
void clearDisplayBuffer(void)
{
    memset(bufDisplay, 0, 1024);
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция заполняет буфер дисплея переданным символом.
*/
void setDisplayBuffer(unsigned char ch)
{
    memset(bufDisplay, ch, 1024);
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Вывод буфера на дисплей.
*/
void showDisplayBuffer(void)
{
    //xSemaphoreTake(xSPI1semaphore, portMAX_DELAY);
    sendByteToDisplay(0x40, DISPLAY_CMD); // вывод с первой линии
    sendByteToDisplay(0xA6, DISPLAY_CMD); // нормал не реверсионное отображение

    for (int page = 0; page < 8; page++)
    {
        //Выбор страницы
        sendByteToDisplay(0xB0 + page, DISPLAY_CMD);
        //Выбор нулевого столбца
        sendByteToDisplay(0x10, DISPLAY_CMD);
        sendByteToDisplay(0x00, DISPLAY_CMD);

        for (int col = 0; col < 128; col++)
        {
            sendByteToDisplay(bufDisplay[page][col], DISPLAY_DATA);
        }
    }
    
    //xSemaphoreGive(xSPI1semaphore);
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Инверсия цветов вывода символов.
*/
void setBlackPage(int newPage)
{
    page = newPage;

    for (int col = 0; col < 128; col++)
    {
        bufDisplay[page][col] = 0xff;
    }
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Установка страницы (строки).
*/
void setPage(int pageNumber)
{
    page = pageNumber;
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Установка столбца.
*/
void setCol(int colNumber)
{
    col = colNumber;
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Запись byte в буфер.
*/
void writeByteToDisplay(unsigned char ch)
{
    bufDisplay[page][col] = ch;
    col++;    
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Запись символа 5х8 в буфер.
*/
void writeChar5x8(int indxChar, int flags)
{
    for (int colByte = 0; colByte < 5; ++colByte)
    {
        int byte = Font5x8[indxChar][colByte];

        if (flags & REVERS_COLOR)
        {
            byte = ~byte;
        }

        bufDisplay[page][col] = byte;
        col++;
    }

    if (flags & CHAR_PLUS_COL)
    {
        if (flags & REVERS_COLOR)
        {
            bufDisplay[page][col] = 0xff;
        }
        else
        {
            bufDisplay[page][col] = 0;
        }

        col++;
    }
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Вывод строки 5х8 в буфер.
*/
void writeString5x8(const CHAR_TYPE *indxBuf, int bufLength, int flags)
{
    if (flags & ADD_BEG_COL)
    {
        if (flags & REVERS_COLOR)
        {
            bufDisplay[page][col] = 0xff;
        }
        else
        {
            bufDisplay[page][col] = 0;
        }

        col++;
    }

    for (int i = 0; i < bufLength; i++)
    {
        writeChar5x8(indxBuf[i], flags);
    }

    if (flags & ADD_END_COL)
    {
        if (flags & REVERS_COLOR)
        {
            bufDisplay[page][col] = 0xff;
        }
        else
        {
            bufDisplay[page][col] = 0;
        }

        col++;
    }
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Вывод символа 12х16 в буфер дисплея.
*/
void writeChar12x16(int indxChar, int flags, int pageF)
{
    for (int colByte = 0; colByte < 12; ++colByte)
    {
        int byte = Font12x16[indxChar][pageF][colByte];

        if (flags & REVERS_COLOR)
        {
            byte = ~byte;
        }

        bufDisplay[page][col] = byte;
        col++;
    }

    if (flags & CHAR_PLUS_COL)
    {
        if (flags & REVERS_COLOR)
        {
            bufDisplay[page][col] = 0xff;
        }
        else
        {
            bufDisplay[page][col] = 0;
        }

        col++;
    }
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Вывод строки на дисплей 12х16.
*/
void writeString12x16(CHAR_TYPE *indxBuf, int bufLength, int flags)
{
    for (int pageF = 0; pageF < 2; pageF++)
    {
        if (pageF == 1)   //вторая часть символов на новой строке
        {
            page++;
            col -= 12*bufLength;

            if (flags & CHAR_PLUS_COL)
            {
                col -= 1*bufLength;
            }
        }

        if (flags & ADD_BEG_COL)
        {
            if (flags & REVERS_COLOR)
            {
                bufDisplay[page][col] = 0xff;
            }
            else
            {
                bufDisplay[page][col] = 0;
            }

            col++;
        }

        for (int i = 0; i < bufLength; i++)
        {
            writeChar12x16(indxBuf[i], flags, pageF);
        }

        if (flags & ADD_END_COL)
        {
            if (flags & REVERS_COLOR)
            {
                bufDisplay[page][col] = 0xff;
            }
            else
            {
                bufDisplay[page][col] = 0;
            }

            col++;
        }
    }
}
