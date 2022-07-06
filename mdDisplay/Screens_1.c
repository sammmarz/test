/**
 \file
Файл содержит функции для отображения экранов группы 1
*/

#include "DisplayFunctions.h"
#include "StringsForDisplay.h"
#include "Fonts5x8.h"
#include "Fonts12x16.h"
#include "ConverterFunctions.h"
#include "Params\Params.h"

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 0 "Объёмы" группы 1.
*/
void showGroup1Screen0(int cmd)
{
    clearDisplayBuffer();
    setBlackPage(0);
    setCol(45);
    writeString5x8(sVolumes, sizeof(sVolumes) / sizeof(CHAR_TYPE),
                   REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_1, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_3, REVERS_COLOR);
    //Vco
    setPage(2);
    setCol(0);
    writeString5x8(sVco, sizeof(sVco) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(30);
    double curValue = params.VcO;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 7, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 7, CHAR_PLUS_COL);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    //м3
    writeString5x8(sM3, sizeof(sM3) / sizeof(CHAR_TYPE), ADD_BEG_COL);
    //Vpo
    setPage(3);
    setCol(0);
    writeString5x8(sVpo, sizeof(sVpo) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(30);
    curValue = params.VpO;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 7, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 7, CHAR_PLUS_COL);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    //м3
    writeString5x8(sM3, sizeof(sM3) / sizeof(CHAR_TYPE), ADD_BEG_COL);
    //Vc
    setPage(4);
    setCol(0);
    writeString5x8(sVc, sizeof(sVc) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(30);
    curValue = params.Vc;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 7, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 7, CHAR_PLUS_COL);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    //м3
    writeString5x8(sM3, sizeof(sM3) / sizeof(CHAR_TYPE), ADD_BEG_COL);
    //Vp
    setPage(5);
    setCol(0);
    writeString5x8(sVp, sizeof(sVp) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(30);
    curValue = params.Vp;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 7, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 7, CHAR_PLUS_COL);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    //м3
    writeString5x8(sM3, sizeof(sM3) / sizeof(CHAR_TYPE), ADD_BEG_COL);
    showStatus();
}
/*
void showGroup1Screen0(int cmd)
{
    clearDisplayBuffer();
    setBlackPage(0);
    setCol(45);
    writeString5x8(sVolumes, sizeof(sVolumes) / sizeof(CHAR_TYPE),
                   REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_1, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_3, REVERS_COLOR);
    //Vco
    setPage(2);
    setCol(0);
    const CHAR_TYPE VpCP[5] = {CH_V, CH_p, CH_C, CH_P, CH_DPOINT};
    writeString5x8(VpCP, sizeof(VpCP) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(30);
    double curValue = params.VPCP;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 7, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 7, CHAR_PLUS_COL);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    //м3
    writeString5x8(sM3, sizeof(sM3) / sizeof(CHAR_TYPE), ADD_BEG_COL);
    //Vpo
    setPage(3);
    setCol(0);
    writeString5x8(sVpo, sizeof(sVpo) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(30);
    curValue = params.VpO;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 7, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 7, CHAR_PLUS_COL);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    //м3
    writeString5x8(sM3, sizeof(sM3) / sizeof(CHAR_TYPE), ADD_BEG_COL);
    //Vc
    setPage(4);
    setCol(0);
    const CHAR_TYPE impulses[9] = {CH_I, CH_m, CH_p, CH_u, CH_l, CH_s, CH_e, CH_s, CH_DPOINT};
    writeString5x8(impulses, sizeof(impulses) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(30);
    curValue = params.impulses;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 7, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 7, CHAR_PLUS_COL);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    //м3
    writeString5x8(sM3, sizeof(sM3) / sizeof(CHAR_TYPE), ADD_BEG_COL);
    //Vp
    setPage(5);
    setCol(0);
    writeString5x8(sVp, sizeof(sVp) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(30);
    curValue = params.Vp;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 7, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 7, CHAR_PLUS_COL);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL | ADD_END_COL);
    //м3
    writeString5x8(sM3, sizeof(sM3) / sizeof(CHAR_TYPE), ADD_BEG_COL);
    showStatus();
}
*/
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 1 "Подст. коэф" группы 1.
*/
void showGroup1Screen1(int cmd)
{
    clearDisplayBuffer();
    setBlackPage(0);
    setCol(35);
    writeString5x8(sParams, sizeof(sParams) / sizeof(CHAR_TYPE),
                   REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_2, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_3, REVERS_COLOR);
    // T измер.:
    setPage(2);
    setCol(0);
    writeString5x8(sT_izmer, sizeof(sT_izmer) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(60);
    int temp = (int)params.T;

    if(temp < 0)
    {
        writeChar5x8(CH_DASH, 0);
    }

    getIndxFontFromDigits(temp, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8,
                          ZERO_TO_SPACE);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_SPACE, 0);
    writeString5x8(sDegr, sizeof(sDegr) / sizeof(CHAR_TYPE), 0);
    //P подст.: 101325 Па
    setPage(3);
    setCol(0);
    writeString5x8(sP_podst, sizeof(sP_podst) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(60);
    int pres = params.Ppod;
    getIndxFontFromDigits(pres, 6, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8,
                          ZERO_TO_SPACE);
    writeString5x8(bufIndx, 6, 0);
    writeChar5x8(CH_SPACE, 0);
    writeString5x8(sPa, sizeof(sPa) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    //T подст.: 20 C
    setPage(4);
    setCol(0);
    writeString5x8(sT_podst, sizeof(sT_podst) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(60);
    temp = (int)params.Tpod;

    if(temp < 0)
    {
        writeChar5x8(CH_DASH, 0);
    }

    getIndxFontFromDigits(temp, 2, bufIndx, POSITION_ZERO_IN_FONT5x8, POSITION_SPACE_IN_FONT5x8,
                          ZERO_TO_SPACE);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_SPACE, 0);
    writeString5x8(sDegr, sizeof(sDegr) / sizeof(CHAR_TYPE), 0);
    //Ксж.: 0.98000
    setPage(5);
    setCol(0);
    writeString5x8(sKszh, sizeof(sKszh) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(60);
    double curValue = params.K;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 1, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 1, 0);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 3 знака после запятой
    getIndxFontFromDigits((int)(curValue*1000)%1000, 3, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 3, CHAR_PLUS_COL);
    //Ккор.: 0.95699
    setPage(6);
    setCol(0);
    writeString5x8(sKkor, sizeof(sKkor) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(60);
    curValue = params.Kkor;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 1, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 1, 0);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 3 знака после запятой
    getIndxFontFromDigits((int)(curValue*1000)%1000, 3, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 3, CHAR_PLUS_COL);
    showStatus();
}
/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция отображает экран 3 "Расход" группы 1 .
*/
void showGroup1Screen2(int cmd)
{
    clearDisplayBuffer();
    setBlackPage(0);
    setCol(40);
    writeString5x8(sRashod, sizeof(sRashod) / sizeof(CHAR_TYPE), REVERS_COLOR | ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(110);
    writeChar5x8(CH_3, REVERS_COLOR);
    writeChar5x8(CH_SLASH, REVERS_COLOR);
    writeChar5x8(CH_3, REVERS_COLOR);
    //Расход: 1.2536
    setPage(2);
    setCol(0);
    writeString5x8(sRashod, sizeof(sRashod) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    writeChar5x8(CH_DPOINT, 0);
    setCol(55);
    double curValue = params.Qp;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 2, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL);
    writeChar5x8(CH_SPACE, 0);
    writeString5x8(sM3_Ch, sizeof(sM3_Ch) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    //Qmax: 22.1234 м3\ч
    setPage(3);
    setCol(0);
    writeString5x8(sQmax, sizeof(sQmax) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(55);
    curValue = params.QalertU;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 2, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL);
    writeChar5x8(CH_SPACE, 0);
    writeString5x8(sM3_Ch, sizeof(sM3_Ch) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    //Qmin: 22.1234 м3\ч
    setPage(4);
    setCol(0);
    writeString5x8(sQmin, sizeof(sQmin) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(55);
    curValue = params.QalertD;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 2, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL);
    writeChar5x8(CH_SPACE, 0);
    writeString5x8(sM3_Ch, sizeof(sM3_Ch) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    //QрПн: 22.1234 м3\ч
    setPage(5);
    setCol(0);
    writeString5x8(sQpPn, sizeof(sQpPn) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(55);
    curValue = params.QpodD;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 2, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL);
    writeChar5x8(CH_SPACE, 0);
    writeString5x8(sM3_Ch, sizeof(sM3_Ch) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    //QрПв: 22.1234 м3\ч
    setPage(6);
    setCol(0);
    writeString5x8(sQpPv, sizeof(sQpPv) / sizeof(CHAR_TYPE), ADD_BEG_COL | CHAR_PLUS_COL);
    setCol(55);
    curValue = params.QpodU;//readDoubleFromFlash(adress);
    getIndxFontFromDigits((int)curValue, 2, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, ZERO_TO_SPACE);
    writeString5x8(bufIndx, 2, 0);
    writeChar5x8(CH_COMMA, CHAR_PLUS_COL);
    //берем 4 знака после запятой
    getIndxFontFromDigits((int)(curValue*10000)%10000, 4, bufIndx, POSITION_ZERO_IN_FONT5x8,
                          POSITION_SPACE_IN_FONT5x8, 0);
    writeString5x8(bufIndx, 4, CHAR_PLUS_COL);
    writeChar5x8(CH_SPACE, 0);
    writeString5x8(sM3_Ch, sizeof(sM3_Ch) / sizeof(CHAR_TYPE), CHAR_PLUS_COL);
    showStatus();
}
