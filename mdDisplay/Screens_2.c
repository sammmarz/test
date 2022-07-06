/**
 \file
Файл содержит функции для отображения экранов группы 2
*/
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>
#include "DisplayFunctions.h"
#include "mdDisplay.h"
#include "StringsForDisplay.h"
#include "Fonts5x8.h"
#include "Fonts12x16.h"
#include "ConverterFunctions.h"
#include "Params/Params.h"
#include "mdPower/mdPower.h"
#include "StringsWarnings.h"

void countChecksum(void);

extern uint16_t checksumMainCode;
extern uint16_t checksumMeterCode;
extern SemaphoreHandle_t xMutexParams;

bool acceptNewTourBat;

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 0 "ПО" группы 2.
*/
void showGroup2Screen0(int cmd)
{
    clearDisplayBuffer();
    setBlackPage(0);
    setCol(55);
    writeString5x8(sPO, sizeof(sPO) / sizeof(CHAR_TYPE), REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_1, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_2, REVERS_COLOR);
    setPage(2);
    setCol(0);
    writeString5x8(sVersM, sizeof(sVersM) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    setCol(55);
    writeChar5x8(params.versMeter[0] - 32, CHAR_PLUS_COL);
    writeChar5x8(params.versMeter[1] - 32, CHAR_PLUS_COL);
    writeChar5x8(CH_POINT, CHAR_PLUS_COL);
    writeChar5x8(params.versMeter[3] - 32, CHAR_PLUS_COL);
    writeChar5x8(params.versMeter[4] - 32, CHAR_PLUS_COL);
    
    setPage(3);
    setCol(12);
    writeString5x8(sCRCM, sizeof(sCRCM) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    setCol(55);
    countChecksum();
    bufChar[0] = '0';
    bufChar[1] = 'x';
    sprintf(bufChar+2, "%04X", checksumMeterCode);
    getIndxFontFromString(bufChar, 6, bufIndx);
    writeString5x8(bufIndx, 6, CHAR_PLUS_COL);
    setPage(5);
    setCol(0);
    writeString5x8(sVers, sizeof(sVers) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    setCol(55);
    writeChar5x8(params.versMain[0] - 32, CHAR_PLUS_COL);
    writeChar5x8(params.versMain[1] - 32, CHAR_PLUS_COL);
    writeChar5x8(CH_POINT, CHAR_PLUS_COL);
    writeChar5x8(params.versMain[3] - 32, CHAR_PLUS_COL);
    writeChar5x8(params.versMain[4] - 32, CHAR_PLUS_COL);

    setPage(6);
    setCol(12);
    writeString5x8(sCRC, sizeof(sCRC) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    setCol(55);
    bufChar[0] = '0';
    bufChar[1] = 'x';
    sprintf(bufChar+2, "%04X", checksumMainCode);
    getIndxFontFromString(bufChar, 6, bufIndx);
    writeString5x8(bufIndx, 6, CHAR_PLUS_COL);
    showStatus();
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 1 "Батареи" группы 2.
*/
void showGroup2Screen1(int cmd)
{
    clearDisplayBuffer();
    //Батареи
    setBlackPage(0);
    setCol(45);
    writeString5x8(sBatteries, sizeof(sBatteries) / sizeof(CHAR_TYPE),
                   REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);  
    if(!acceptNewTourBat)
    {
        if(cmd & CENTER_BTN)
        {
            cmd = 0;
            acceptNewTourBat = true;        
        }
    }
    else
    {
        if(cmd & CENTER_BTN)
        {
            xSemaphoreTake(xMutexParams, portMAX_DELAY);
            setNewBat();            
            xSemaphoreGive(xMutexParams);
        }
        acceptNewTourBat = false;
        StrEventForDisplay cmdUpdate;
        cmdUpdate.status = 1;
        cmdUpdate.codeEventForDisplay = UPDATE_DISPLAY;
        xQueueSendToBack(xBtnsQueue, &cmdUpdate, NULL);
    }
    if(!acceptNewTourBat)
    {
        setCol(110);
        writeChar5x8(CH_2, REVERS_COLOR);
        writeChar5x8(CH_SLASH, REVERS_COLOR);
        writeChar5x8(CH_2, REVERS_COLOR);
        //Батарея 1
        setPage(2);
        setCol(0);
        writeString5x8(sBattery_1, sizeof(sBattery_1) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        setCol(80);
        int charge = (int)(params.lifeBat1 * (100. / TOUR_BAT));
        getIndxFontFromDigits(charge, 3, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8, 0);
        writeString5x8(bufIndx, 3, CHAR_PLUS_COL);
        writeChar5x8(CH_PERCENT, 0);
        setPage(3);
        setCol(0);
        writeString5x8(sOstatSrok, sizeof(sOstatSrok) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        setCol(80);
        int serviceLife = params.lifeBat1;
        getIndxFontFromDigits(serviceLife, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8,
                              0);
        writeString5x8(bufIndx, 2, CHAR_PLUS_COL);
        writeChar5x8(CH_SPACE, 0);
        writeString5x8(sMes, sizeof(sMes) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        //Батарея 2
        setPage(5);
        setCol(0);
        writeString5x8(sBattery_2, sizeof(sBattery_2) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        setCol(80);
        charge = (int)(params.lifeBat2 * (100. / TOUR_BAT));
        getIndxFontFromDigits(charge, 3, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8, 0);
        writeString5x8(bufIndx, 3, CHAR_PLUS_COL);
        writeChar5x8(CH_PERCENT, 0);
        setPage(6);
        setCol(0);
        writeString5x8(sOstatSrok, sizeof(sOstatSrok) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        setCol(80);
        serviceLife = params.lifeBat2;
        getIndxFontFromDigits(serviceLife, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8,
                              0);
        writeString5x8(bufIndx, 2, CHAR_PLUS_COL);
        writeChar5x8(CH_SPACE, 0);
        writeString5x8(sMes, sizeof(sMes) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        //showStatus();
        setBlackPage(7);
        setCol(0);
        writeString5x8(sCircle3, sizeof(sCircle3) / sizeof(CHAR_TYPE), REVERS_COLOR);
        writeString5x8(sChangeBat, sizeof(sChangeBat) / sizeof(CHAR_TYPE),
                       REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    }
    // меню подтверждения установки новой батареи
    else
    {            
        setPage(2);
        setCol(0);
        writeString5x8(sAcceptNewBat, sizeof(sAcceptNewBat) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        setPage(3);
        setCol(20);
        writeString5x8(sAcceptNewBat2, sizeof(sAcceptNewBat2) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        
        setPage(5);
        setCol(3);
        writeStringWarnings(sArrowL, sizeof(sArrowL) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
        writeChar5x8(CH_SPACE, CHAR_PLUS_COL);
        writeChar5x8(CH_3, CHAR_PLUS_COL);
        writeChar5x8(CH_SPACE, CHAR_PLUS_COL);
        writeChar5x8(CH_Ks, CHAR_PLUS_COL);
        writeChar5x8(CH_Ke, CHAR_PLUS_COL);
        writeChar5x8(CH_Kk, CHAR_PLUS_COL);
        writeChar5x8(CH_POINT, CHAR_PLUS_COL);
        writeChar5x8(CH_SPACE, CHAR_PLUS_COL);
        writeChar5x8(CH_SPACE, CHAR_PLUS_COL);
        writeChar5x8(CH_KN, CHAR_PLUS_COL);
        writeChar5x8(CH_KE, CHAR_PLUS_COL);
        writeChar5x8(CH_KT, CHAR_PLUS_COL);
        setPage(6);
        setCol(3);
        writeString5x8(sCircle3, sizeof(sCircle3) / sizeof(CHAR_TYPE), 0);
        writeChar5x8(CH_Ks, CHAR_PLUS_COL);
        writeChar5x8(CH_Ke, CHAR_PLUS_COL);
        writeChar5x8(CH_Kk, CHAR_PLUS_COL);
        writeChar5x8(CH_POINT, CHAR_PLUS_COL);
        writeChar5x8(CH_SPACE, CHAR_PLUS_COL);
        writeChar5x8(CH_SPACE, CHAR_PLUS_COL);
        writeChar5x8(CH_KD, CHAR_PLUS_COL);
        writeChar5x8(CH_KA, CHAR_PLUS_COL);
    }
}
