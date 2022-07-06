#include <stdbool.h>

bool statusKlapan = false;
int serverCmd = 0;
bool stopNavigate = false;
void showGroup3Screen0(int cmd)
{
    /*
    clearDisplayBuffer();

    setBlackPage(0);
    setCol(45);
    //Клапан
    let indxb = [CH._KK, CH._Kl, CH._Ka, CH._Kp, CH._Ka, CH._Kn];
    writeString5x8(indxb, indxb.length, DISPLAY_FLAGS.REVERS_COLOR | DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);

    setPage(3);
    setCol(20);
    if (statusKlapan == false) {
        indxb = [CH._KS, CH._Kt, CH._Ka, CH._Kt, CH._Ku, CH._Ks, CH._DPOINT, CH._SPACE, CH._Kz, CH._Ka, CH._Kk, CH._Kr, CH._Ky, CH._Kt];
        writeString5x8(indxb, indxb.length, DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);

        // от сервера пришло разрешение открыть клапан
        if (serverCmd & 1) {
            setBlackPage(7);
            setCol(0);
            //Удерж. Center откр.
            indxb = [CH._KU, CH._Kd, CH._Ke, CH._Kr, CH._Kzh, CH._POINT, CH._SPACE, CH._C, CH._e, CH._n, CH._t, CH._e, CH._r, CH._SPACE, CH._Ko, CH._Kt, CH._Kk, CH._Kr, CH._Ky, CH._Kt, CH._Ksoft];
            writeString5x8(indxb, indxb.length, DISPLAY_FLAGS.REVERS_COLOR | DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);

            if (cmd & 1) {
                openKlapan();
                return;
            }
        } else {
            //проверка утечки
            if (cmd & 1) {
                checkCloseKlapan();
                return;
            }
            setBlackPage(7);
            setCol(0);
            //Удерж. Center провер.
            indxb = [CH._KU, CH._Kd, CH._Ke, CH._Kr, CH._Kzh, CH._POINT, CH._SPACE, CH._C, CH._e, CH._n, CH._t, CH._e, CH._r, CH._SPACE, CH._Kp, CH._Kr, CH._Ko, CH._Kv, CH._Ke, CH._Kr, CH._POINT];
            writeString5x8(indxb, indxb.length, DISPLAY_FLAGS.REVERS_COLOR | DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);
        }

    } else {
        indxb = [CH._KS, CH._Kt, CH._Ka, CH._Kt, CH._Ku, CH._Ks, CH._DPOINT, CH._SPACE, CH._Ko, CH._Kt, CH._Kk, CH._Kr, CH._Ky, CH._Kt];
        writeString5x8(indxb, indxb.length, DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);

        showStatus();
    }
    */
}
/*
let count = 0;
let indxb1;
let sucOpenKlapan = 0;
function checkCloseKlapan() {
    stopNavigate = true;
    count++;

    if (count == 1) {
        clearDisplayBuffer();
        setBlackPage(2);
        setCol(0);
        indxb1 = [CH._KP, CH._Kr, CH._Ko, CH._Kv, CH._Ke, CH._Kr, CH._Kk, CH._Ka, CH._SPACE, CH._Ku, CH._Kt, CH._Ke, CH._Kch, CH._Kk, CH._Ki, CH._SPACE, CH._Kg, CH._Ka, CH._Kz, CH._Ka];
        writeString5x8(indxb1, indxb1.length, DISPLAY_FLAGS.REVERS_COLOR | DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);
        setBlackPage(3);
        setBlackPage(4);
        setCol(1);
    }
    if (count < 21) {
        indxb1 = [CH._SPACE];
        writeString5x8(indxb1, indxb1.length, 0);
        col++;
        setTimeout(checkCloseKlapan, 500);
    }
    else {
        stopNavigate = false;
        cmd &= ~1;
        count = 0;
        clearDisplayBuffer();
        setPage(3);
        setCol(20);
        indxb1 = [CH._KU, CH._Kt, CH._Ke, CH._Kch, CH._Ke, CH._Kk, CH._SPACE, CH._Kn, CH._Ke, CH._Kt, CH._POINT];
        writeString5x8(indxb1, indxb1.length, DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);
    }
}

function openKlapan() {
    stopNavigate = true;
    count++;

    if (count == 1) {
        clearDisplayBuffer();
        setBlackPage(2);
        setCol(0);
        indxb1 = [CH._KP, CH._Kr, CH._Ko, CH._Kv, CH._Ke, CH._Kr, CH._Kk, CH._Ka, CH._SPACE, CH._Ku, CH._Kt, CH._Ke, CH._Kch, CH._Kk, CH._Ki, CH._SPACE, CH._Kg, CH._Ka, CH._Kz, CH._Ka];
        writeString5x8(indxb1, indxb1.length, DISPLAY_FLAGS.REVERS_COLOR | DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);
        setBlackPage(3);
        setBlackPage(4);
        setCol(1);
    }
    if (count < 21) {
        indxb1 = [CH._SPACE];
        writeString5x8(indxb1, indxb1.length, 0);
        col++;
        setTimeout(openKlapan, 500);
    }
    else {
        sucOpenKlapan++;
        stopNavigate = false;
        cmd &= ~1;
        count = 0;
        if (sucOpenKlapan == 2) {
            statusKlapan = true;
            showGroup3Screen0(0);
        }
        else {
            clearDisplayBuffer();

            setBlackPage(0);
            setCol(45);
            //Клапан
            indxb1 = [CH._KK, CH._Kl, CH._Ka, CH._Kp, CH._Ka, CH._Kn];
            writeString5x8(indxb1, indxb1.length, DISPLAY_FLAGS.REVERS_COLOR | DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);

            setPage(3);
            setCol(10);
            indxb1 = [CH._KO, CH._Kb, CH._Kn, CH._Ka, CH._Kr, CH._Ku, CH._Kzh, CH._Ke, CH._Kn, CH._Ka, CH._SPACE, CH._Ku, CH._Kt, CH._Ke, CH._Kch, CH._Kk, CH._Ka, CH._SPACE, CH._Kg, CH._Ka, CH._Kz, CH._Ka];
            writeString5x8(indxb1, indxb1.length, DISPLAY_FLAGS.ADD_BEG_COL);

            setPage(4);
            setCol(20);
            indxb1 = [CH._KS, CH._Kt, CH._Ka, CH._Kt, CH._Ku, CH._Ks, CH._DPOINT, CH._SPACE, CH._Kz, CH._Ka, CH._Kk, CH._Kr, CH._Ky, CH._Kt];
            writeString5x8(indxb1, indxb1.length, DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);

            //Удерж. Center открыть
            setBlackPage(7);
            setCol(0);
            indxb1 = [CH._KU, CH._Kd, CH._Ke, CH._Kr, CH._Kzh, CH._POINT, CH._SPACE, CH._C, CH._e, CH._n, CH._t, CH._e, CH._r, CH._SPACE, CH._Ko, CH._Kt, CH._Kk, CH._Kr, CH._Ky, CH._Kt, CH._Ksoft];
            writeString5x8(indxb1, indxb1.length, DISPLAY_FLAGS.REVERS_COLOR | DISPLAY_FLAGS.ADD_BEG_COL | DISPLAY_FLAGS.CHAR_PLUS_COL);
        }
    }
}
*/
