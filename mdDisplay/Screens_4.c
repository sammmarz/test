/**
 \file
Файл содержит функции для отображения экранов группы 4
*/
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>
#include <stdbool.h>
#include "DisplayFunctions.h"
#include "StringsForDisplay.h"
#include "Fonts5x8.h"
#include "Fonts12x16.h"
#include "ConverterFunctions.h"
#include "../Params/Params.h"
#include "mdOpticalInterface/mdOpticalInterface.h"
#include "mdFTP/mdFTP.h"

bool optPortStatus;
bool gprsStatus;
extern SemaphoreHandle_t xMutexParams;
// otlad
extern CHAR_TYPE optStr[148];
// otlad end
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 0 "Оптический порт" группы 4.
*/
void showGroup4Screen0(int cmd)
{
    clearDisplayBuffer();
    setBlackPage(0);
    setCol(10);
    writeString5x8(sOptPort, sizeof(sOptPort) / sizeof(CHAR_TYPE),
                   REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_1, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_3, REVERS_COLOR);
    //otlad;
    /*setPage(1);
    setCol(0);
    getIndxFontFromDigits((int)searchIRQ, 10, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 10, 0);
    */
    // otlad end
    setPage(3);
    setCol(30);
    writeString5x8(sStatus, sizeof(sStatus) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    writeChar5x8(CH_SPACE, 0);

    if (cmd & 1)
    {
        if (optPortStatus)
        {
            optPortStatus = false;
            uint8_t byte = 0;
            xQueueSendToBack(xOptBufQueue, &byte, NULL);
            disableOptPower();
        }
        else
        {
            optPortStatus = true;
            initOpticalInterface();
            enableOptPower();
            xSemaphoreTake(xMutexParams, portMAX_DELAY);
            setEvent(EV_OPT_READY);
            xSemaphoreGive(xMutexParams);
        }
    }

    if (optPortStatus)
    {
        //порт Вкл.
        writeString5x8(sVkl, sizeof(sVkl) / sizeof(CHAR_TYPE), ADD_BEG_COL | ADD_END_COL | CHAR_PLUS_COL);
        setBlackPage(7);
        setCol(0);
        writeString5x8(sCircle3, sizeof(sCircle3) / sizeof(CHAR_TYPE), REVERS_COLOR);
        writeString5x8(sSecForVikl, sizeof(sSecForVikl) / sizeof(CHAR_TYPE),
                       REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    }
    else
    {
        //порт Выкл.
        writeString5x8(sVikl, sizeof(sVikl) / sizeof(CHAR_TYPE), ADD_BEG_COL | ADD_END_COL | CHAR_PLUS_COL);
        setBlackPage(7);
        setCol(0);
        writeString5x8(sCircle3, sizeof(sCircle3) / sizeof(CHAR_TYPE), REVERS_COLOR);
        writeString5x8(sSecForVkl, sizeof(sSecForVkl) / sizeof(CHAR_TYPE),
                       REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    }

    //otlad
    /*setPage(0);
    setCol(0);
    //optStr[0] = (int)'?' - 32;
    writeString5x8(&optStr[0], 25, CHAR_PLUS_COL);
    setPage(1);
    setCol(0);
    writeString5x8(&optStr[25], 25, CHAR_PLUS_COL);
    setPage(2);
    setCol(0);
    writeString5x8(&optStr[50], 25, CHAR_PLUS_COL);
    setPage(3);
    setCol(0);
    writeString5x8(&optStr[75], 25, CHAR_PLUS_COL);
    setPage(4);
    setCol(0);
    writeString5x8(&optStr[100], 25, CHAR_PLUS_COL);
    setPage(5);
    setCol(0);
    writeString5x8(&optStr[125], 23, CHAR_PLUS_COL);
    */
    // otlad end
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 1 "GPRS" группы 4.

 \todo Реализовать строку статуса подключения GPRS
*/
void showGroup4Screen1(int cmd)
{
    clearDisplayBuffer();
    setBlackPage(0);
    setCol(50);
    writeString5x8(sGPRS, sizeof(sGPRS) / sizeof(CHAR_TYPE), REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_2, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_3, REVERS_COLOR);
    setPage(3);
    setCol(30);
    writeString5x8(sStatus, sizeof(sStatus) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    writeChar5x8(CH_SPACE, 0);

    if (cmd & 1)
    {
        if (gprsStatus)
        {
            gprsStatus = false;
        }
        else
        {
            enableFTP();
            gprsStatus = true;
        }
    }

    if (gprsStatus)
    {
        //GPRS Вкл.
        writeString5x8(sVkl, sizeof(sVkl) / sizeof(CHAR_TYPE), ADD_BEG_COL | ADD_END_COL | CHAR_PLUS_COL);
        setBlackPage(7);
        setCol(0);
        writeString5x8(sCircle3, sizeof(sCircle3) / sizeof(CHAR_TYPE), REVERS_COLOR);
        writeString5x8(sSecForVikl, sizeof(sSecForVikl) / sizeof(CHAR_TYPE),
                       REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    }
    else
    {
        //GPRS Выкл.
        writeString5x8(sVikl, sizeof(sVikl) / sizeof(CHAR_TYPE), ADD_BEG_COL | ADD_END_COL | CHAR_PLUS_COL);
        setBlackPage(7);
        setCol(0);
        writeString5x8(sCircle3, sizeof(sCircle3) / sizeof(CHAR_TYPE), REVERS_COLOR);
        writeString5x8(sSecForVkl, sizeof(sSecForVkl) / sizeof(CHAR_TYPE),
                       REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    }
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 2 "Настройки GPRS" группы 4.

 \todo Нужно заменить константы на реальные данные
*/
void showGroup4Screen2(int cmd)
{
    clearDisplayBuffer();
    setBlackPage(0);
    setCol(15);
    writeString5x8(sSettingsGPRS, sizeof(sSettingsGPRS) / sizeof(CHAR_TYPE),
                   REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_3, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_3, REVERS_COLOR);
    setPage(2);
    setCol(0);
    writeString5x8(sRSSI, sizeof(sRSSI) / sizeof(CHAR_TYPE), CHAR_PLUS_COL | ADD_BEG_COL | ADD_END_COL);
    writeChar5x8(CH_SPACE, 0);
    int rssi = 30; //readIntFromFlash(adress);
    getIndxFontFromDigits(rssi, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 2, CHAR_PLUS_COL);
    setPage(4);
    setCol(0);
    writeString5x8(sAPN, sizeof(sAPN) / sizeof(CHAR_TYPE), CHAR_PLUS_COL | ADD_BEG_COL | ADD_END_COL);
    writeChar5x8(CH_SPACE, 0);
    char *chB = "internet.mts.ru"; //getStringFromFlash(bufChar, max_buf_size = 25);
    int strL = strlen(chB);
    getIndxFontFromString(chB, strL, bufIndx);
    writeString5x8(bufIndx, strL, ADD_BEG_COL | CHAR_PLUS_COL);
    setPage(6);
    setCol(0);
    writeString5x8(sIP, sizeof(sIP) / sizeof(CHAR_TYPE), CHAR_PLUS_COL | ADD_BEG_COL | ADD_END_COL);
    writeChar5x8(CH_SPACE, 0);
    char *chB2 = "82.208.88.82"; //getStringFromFlash(bufChar, max_buf_size = 25);
    strL = strlen(chB2);
    getIndxFontFromString(chB2, strL, bufIndx);
    writeString5x8(bufIndx, strL, ADD_BEG_COL | CHAR_PLUS_COL);
    showStatus();
}
