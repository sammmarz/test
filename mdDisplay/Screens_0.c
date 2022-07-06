/**
 \file
Файл содержит функции для отображения экранов группы 0
*/

#include "DisplayFunctions.h"
#include "StringsForDisplay.h"
#include "Fonts5x8.h"
#include "Fonts12x16.h"
#include "ConverterFunctions.h"
#include "mdDisplay.h"
#include "StringsWarnings.h"
#include "mdRTC/mdRTC.h"

#include "string.h"                                                             // For memset
// otlad
extern double temperature;
// otlad end
int skipWarnings;
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 0 "Текущее показание" группы 0.
*/
void showGroup0Screen0(int cmd)
{
    clearDisplayBuffer();
    // Текущее показание 1\2
    setBlackPage(0);
    setCol(5);
    writeString5x8(sTekusheePokazanie, sizeof(sTekusheePokazanie) / sizeof(CHAR_TYPE),
                   REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_1, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_2, REVERS_COLOR);
    //вывод показаний счётчика
    setPage(2);
    setCol(0);
    double curValue = params.VcO;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 7, bufIndx, POSITION_ZERO_IN_FONT12x16,
                          POSITION_SPACE_IN_FONT12x16, 0);
    writeString12x16(bufIndx, 7, 0);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    //м3
    writeString5x8(sM3, sizeof(sM3) / sizeof(CHAR_TYPE), ADD_BEG_COL);
    //Дата и время
    struct tm dateTime;
    getRtcDateTime(&dateTime);
    setPage(5);
    setCol(0);
    int day = dateTime.tm_mday;
    getIndxFontFromDigits(day, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 2, ADD_BEG_COL);
    writeChar5x8(CH_POINT, 0);
    int month = dateTime.tm_mon + 1;
    getIndxFontFromDigits(month, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_POINT, 0);
    int year = dateTime.tm_year + 1900;
    getIndxFontFromDigits(year, 4, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, 0);
    writeChar5x8(CH_SPACE, 0);
    int hours = dateTime.tm_hour;
    getIndxFontFromDigits(hours, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_DPOINT, 0);
    int minutes = dateTime.tm_min;
    getIndxFontFromDigits(minutes, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_SPACE, 0);
    //UTC
    writeChar5x8(CH_BRACKET_B, 0);
    writeString5x8(sUTC, sizeof(sUTC) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    int utc = params.utc / 60;

    if(utc >= 0)
    {
        writeChar5x8(CH_PlUS, 0);
    }
    else
    {
        writeChar5x8(CH_DASH, 0);
    }

    getIndxFontFromDigits(utc, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_BRACKET_E, 0);
    /*// otlad
    setPage(7);
    setCol(0);
    writeString5x8(sT_izmer, sizeof(sT_izmer) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(60);
    double local_T = temperature;

    if(local_T < 0)
    {
        writeChar5x8(CH_DASH, CHAR_PLUS_COL);
        local_T = -local_T;
    }

    getIndxFontFromDigits((int)local_T, 2, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 2, ADD_BEG_COL);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(local_T*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    writeChar5x8(CH_SPACE, 0);
    writeString5x8(sDegr, sizeof(sDegr) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    // otlad end*/
    showStatus();
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 1 "Тревоги" группы 0.
*/
void showGroup0Screen1(int cmd)
{
    clearDisplayBuffer();
    // Тревоги 2\2
    setBlackPage(0);
    setCol(40);
    writeString5x8(sTrevogi, sizeof(sTrevogi) / sizeof(CHAR_TYPE),
                   REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_2, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_2, REVERS_COLOR);
    // подсчитываем количество тревог
    int countWar = 0;

    for(int st = 0; st < SUM_STATUS_EVENTS; ++st)
    {
        // 16 разрядов в статусе
        for (int i = 0; i < 16; ++i)
        {
            if (1 & (params.regEvents.events[st] >> i))
            {
                countWar++;
            }
        }
    }

    // Нет тревог
    if (countWar == 0)
    {
        setPage(3);
        setCol(35);
        const CHAR_TYPE indxb[10] = {CH_KN, CH_Ke, CH_Kt, CH_SPACE, CH_Kt, CH_Kr, CH_Ke, CH_Kv, CH_Ko, CH_Kg};
        writeString5x8(indxb, sizeof(indxb) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        setBlackPage(7);
        setCol(45);
        const CHAR_TYPE str[3] = {CH_CIRCLEB, CH_CIRCLEE, CH_SPACE};
        writeString5x8(str, sizeof(str) / sizeof(CHAR_TYPE), REVERS_COLOR);
        const CHAR_TYPE str2[4] = {CH_KM, CH_Ke, CH_Kn, CH_Kyu};
        writeString5x8(str2, sizeof(str2) / sizeof(CHAR_TYPE), REVERS_COLOR | CHAR_PLUS_COL);
        return;
    }

    switch (cmd)
    {
        //первый раз
        case CENTER_BTN:
            skipWarnings = 0;
            break;

        //влево
        case LEFT_BTN:
            if (skipWarnings >= 6)
            {
                skipWarnings -= 6;
            }

            break;

        //вправо
        case RIGHT_BTN:
            if (skipWarnings < (countWar - 6))
            {
                skipWarnings += 6;
            }

            break;
    }

    int emptyWar = skipWarnings;

    for(int st = 0, p = 1; st < SUM_STATUS_EVENTS; ++st)
    {
        for (int i = 0; i < 16; ++i)
        {
            if (1 & (params.regEvents.events[st] >> i))
            {
                if (emptyWar == 0)
                {
                    setPage(p);
                    p++;
                    setCol(0);
                    if(1 & (params.curEvents.events[st] >> i))
                    {
                        writeByteToDisplay(0);
                        writeByteToDisplay(0x5e);
                        writeByteToDisplay(0);
                    }
                    writeString5x8(strWarnDescribe[st][i], sizeof(strWarnDescribe[st][i]) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
                }
                else
                {
                    //не выводим эту тревогу
                    emptyWar--;
                }
            }

            //если уже вывели 6 строк а команд больше
            if ((countWar > 6) && (p == 7))
            {
                break;
            }
        }
    }

    // Подсказка перемещния по тревогам если их больше 7
    setBlackPage(7);
    setCol(45);
    const CHAR_TYPE str[3] = {CH_CIRCLEB, CH_CIRCLEE, CH_SPACE};
    writeString5x8(str, sizeof(str) / sizeof(CHAR_TYPE), REVERS_COLOR);
    const CHAR_TYPE str2[4] = {CH_KM, CH_Ke, CH_Kn, CH_Kyu};
    writeString5x8(str2, sizeof(str2) / sizeof(CHAR_TYPE), REVERS_COLOR | CHAR_PLUS_COL);

    if (skipWarnings >= 1)
    {
        setCol(3);
        writeStringWarnings(sArrowL, sizeof(sArrowL) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }

    if (skipWarnings < (countWar - 6))
    {
        setCol(118);
        writeStringWarnings(sArrowR, sizeof(sArrowR) / sizeof(CHAR_TYPE), REVERS_COLOR);
    }
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает черный экран для проверки работы дисплея.
*/
void showGroup0Screen2(int cmd)
{
    setDisplayBuffer(0xff);
}
