/**
 \file
Файл содержит массивы кодов тревог, а также индексы строк описания тревок
*/

#include <stdint.h>
#include "Fonts5x8.h"
#include "Params/Events.h"

const CHAR_TYPE sWarning[7] = {0x00, 0x00, 0x00, 0x5e, 0x00, 0x00, 0x00};
const CHAR_TYPE sMagnitM[7] = {0x00, 0x7e, 0x04, 0x18, 0x04, 0x7e, 0x00};
const CHAR_TYPE sRegisterStList[9] = {0x00, 0x00, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x00, 0x00};
const CHAR_TYPE sKlapanAll[9] = {0x00, 0x7e, 0x7e, 0x3c, 0x18, 0x3c, 0x7e, 0x7e, 0x00};
const CHAR_TYPE sKlapanHalf[9] = {0x00, 0x7e, 0x7e, 0x3c, 0x18, 0x24, 0x42, 0x7e, 0x00};
const CHAR_TYPE sTemper [7] = {0x00, 0x02, 0x02, 0x7e, 0x02, 0x02, 0x00};
const CHAR_TYPE sBatLow[12] = {0x00, 0x7e, 0x7e, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7e, 0x3c, 0x00};
const CHAR_TYPE sBatOpen[13] = {0x00, 0x7e, 0x02, 0x7a, 0x7a, 0x78, 0x48, 0x48, 0x4a, 0x32, 0x02, 0x7e, 0x00};
const CHAR_TYPE sQalert[8] = {0x00, 0x3c, 0x42, 0x52, 0x72, 0x3c, 0x60, 0x00};
const CHAR_TYPE sKorpus[8] = {0x00, 0x7e, 0x42, 0x40, 0x4e, 0x4c, 0x6a, 0x00};
const CHAR_TYPE sSignal[13] = {0x00, 0x60, 0x60, 0x00, 0x70, 0x70, 0x00, 0x78, 0x78, 0x00, 0x7c, 0x7c, 0x00};
const CHAR_TYPE sSearch[13] = {0x00, 0x3c, 0x42, 0x18, 0x24, 0x18, 0x78, 0x18, 0x24, 0x18, 0x42, 0x3c, 0x00};
const CHAR_TYPE sSearchOpt[13] = {0x00, 0x3c, 0x6e, 0x7e, 0x7e, 0x76, 0x3c, 0x00, 0x24, 0x18, 0x42, 0x3c, 0x00};
const CHAR_TYPE sConnectOpt[13] = {0x00, 0x00, 0x00, 0x3c, 0x6e, 0x7e, 0x7e, 0x76, 0x3c, 0x00, 0x00, 0x00, 0x00};
const CHAR_TYPE sArrowR[7] = {0x18, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18};
const CHAR_TYPE sArrowL[7] = {0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x18};
const CHAR_TYPE  strWarnDescribe[SUM_STATUS_EVENTS][16][20] =
{
    //st_0 (system)
    {
        //bit_0 рестарт
        {CH_KR, CH_Ke, CH_Ks, CH_Kt, CH_Ka, CH_Kr, CH_Kt},
        //bit_1 перепрограмм.
        {CH_KP, CH_Ke, CH_Kr, CH_Ke, CH_Kp, CH_Kr, CH_Ko, CH_Kg, CH_Kr, CH_Ka, CH_Km, CH_POINT},
        //bit_2 данные восстан.
        {CH_KD, CH_Ka, CH_Kn, CH_Ky, CH_Ke, CH_SPACE, CH_Kv, CH_Ko, CH_Ks, CH_Ks, CH_Kt, CH_Ka, CH_Kn, CH_POINT},
        //bit_3 замена батареи 2
        {CH_KZ, CH_Ka, CH_Km, CH_Ke, CH_Kn, CH_Ka, CH_SPACE, CH_Kb, CH_Ka, CH_Kt, CH_Ka, CH_Kr, CH_Ke, CH_Ki, CH_2},
        //bit_4 ошибка данных
        {CH_KO, CH_Ksh, CH_Ki, CH_Kb, CH_Kk, CH_Ka, CH_SPACE, CH_Kd, CH_Ka, CH_Kn, CH_Kn, CH_Ky, CH_Kh},
        //bit_5 клапан закрыт
        {CH_KK, CH_Kl, CH_Ka, CH_Kp, CH_Ka, CH_Kn, CH_SPACE, CH_Kz, CH_Ka, CH_Kk, CH_Kr, CH_Ky, CH_Kt},
        //bit_6 сбой ПО
        {CH_KS, CH_Kb, CH_Ko, CH_Kiy, CH_SPACE, CH_KP, CH_KO},
        //bit_7 ошибка параметров
        {CH_KO, CH_Ksh, CH_Ki, CH_Kb, CH_Kk, CH_Ka, CH_SPACE, CH_Kp, CH_Ka, CH_Kr, CH_Ka, CH_Km, CH_Ke, CH_Kt, CH_Kr, CH_Ko, CH_Kv},
        //bit_8 замена батареи 1
        {CH_KZ, CH_Ka, CH_Km, CH_Ke, CH_Kn, CH_Ka, CH_SPACE, CH_Kb, CH_Ka, CH_Kt, CH_Ka, CH_Kr, CH_Ke, CH_Ki, CH_1},
        //bit_9
        {CH_0},
        //bit_10 проверьте часы
        {CH_KP, CH_Kr, CH_Ko, CH_Kv, CH_Ke, CH_Kr, CH_Ksoft, CH_Kt, CH_Ke, CH_SPACE, CH_Kch, CH_Ka, CH_Ks, CH_Ky},
        //bit_11
        {CH_0},
        //bit_12 Передача данных
        {CH_KP, CH_Ke, CH_Kr, CH_Ke, CH_Kd, CH_Ka, CH_Kch, CH_Ka, CH_SPACE, CH_Kd, CH_Ka, CH_Kn, CH_Kn, CH_Ky, CH_Kh},
        //bit_13
        {CH_0},
        //bit_14
        {CH_0},
        //bit_15 летнее время
        {CH_KL, CH_Ke, CH_Kt, CH_Kn, CH_Ke, CH_Ke, CH_SPACE, CH_Kv, CH_Kr, CH_Ke, CH_Km, CH_Kya}
    },
    //st_1
    {
        //bit_0
        {CH_1},
        //bit_1 обрыв цепи 1
        {CH_KO, CH_Kb, CH_Kr, CH_Ky, CH_Kv, CH_SPACE, CH_Kts, CH_Ke, CH_Kp, CH_Ki, CH_1},
        //bit_2
        {CH_1},
        //bit_3
        {CH_1},
        //bit_4
        {CH_1},
        //bit_5
        {CH_1},
        //bit_6
        {CH_1},
        //bit_7
        {CH_1},
        //bit_8
        {CH_1},
        //bit_9
        {CH_1},
        //bit_10
        {CH_1},
        //bit_11
        {CH_1},
        //bit_12
        {CH_1},
        //bit_13 Калибр. замок
        {CH_KK, CH_Ka, CH_Kl, CH_Ki, CH_Kb, CH_Kr, CH_POINT, CH_SPACE, CH_Kz, CH_Ka, CH_Km, CH_Ko, CH_Kk},
        //bit_14
        {CH_1},
        //bit_15 Опт. порт готов
        {CH_KO, CH_Kp, CH_Kt, CH_POINT, CH_SPACE, CH_Kp, CH_Ko, CH_Kr, CH_Kt, CH_SPACE, CH_Kg, CH_Ko, CH_Kt, CH_Ko, CH_Kv}
    },
    //st_2
    {
        //bit_0
        {CH_2},
        //bit_1 Обрыв цепи 2
        {CH_KO, CH_Kb, CH_Kr, CH_Ky, CH_Kv, CH_SPACE, CH_Kts, CH_Ke, CH_Kp, CH_Ki, CH_2},
        //bit_2
        {CH_2},
        //bit_3
        {CH_2},
        //bit_4
        {CH_2},
        //bit_5
        {CH_2},
        //bit_6
        {CH_2},
        //bit_7
        {CH_2},
        //bit_8
        {CH_2},
        //bit_9
        {CH_2},
        //bit_10
        {CH_2},
        //bit_11
        {CH_2},
        //bit_12
        {CH_2},
        //bit_13 Замок изготовителя
        {CH_KZ, CH_Ka, CH_Km, CH_Ko, CH_Kk, CH_SPACE, CH_Ki, CH_Kz, CH_Kg, CH_Ko, CH_Kt, CH_Ko, CH_Ki, CH_Kt, CH_Ke, CH_Kl, CH_Kya},
        //bit_14
        {CH_2},
        //bit_15 Старт модема
        {CH_KS, CH_Kt, CH_Ka, CH_Kr, CH_Kt, CH_SPACE, CH_Km, CH_Ko, CH_Kd, CH_Ke, CH_Km, CH_Ka}
    },
    //st_3
    {
        //bit_0
        {CH_3},
        //bit_1
        {CH_3},
        //bit_2
        {CH_3},
        //bit_3
        {CH_3},
        //bit_4
        {CH_3},
        //bit_5
        {CH_3},
        //bit_6
        {CH_3},
        //bit_7
        {CH_3},
        //bit_8 Граница Т окр.
        {CH_KG, CH_Kr, CH_Ka, CH_Kn, CH_Ki, CH_Kts, CH_Ka, CH_SPACE, CH_KT, CH_Ko, CH_Kk, CH_Kr, CH_POINT},
        //bit_9
        {CH_3},
        //bit_10
        {CH_3},
        //bit_11
        {CH_3},
        //bit_12
        {CH_3},
        //bit_13 Замок поставщика
        {CH_KZ, CH_Ka, CH_Km, CH_Ko, CH_Kk, CH_SPACE, CH_Kp, CH_Ko, CH_Ks, CH_Kt, CH_Ka, CH_Kv, CH_Ksh, CH_Ki, CH_Kk, CH_Ka},
        //bit_14
        {CH_3},
        //bit_15
        {CH_3}
    },
    //st_4
    {
        //bit_0 Граница Qp
        {CH_KG, CH_Kr, CH_Ka, CH_Kn, CH_Ki, CH_Kts, CH_Ka, CH_SPACE, CH_Q, CH_p},
        //bit_1
        {CH_4},
        //bit_2
        {CH_4},
        //bit_3
        {CH_4},
        //bit_4
        {CH_4},
        //bit_5 Граница Qp п
        {CH_KG, CH_Kr, CH_Ka, CH_Kn, CH_Ki, CH_Kts, CH_Ka, CH_SPACE, CH_Q, CH_p, CH_SPACE, CH_Kp},
        //bit_6
        {CH_4},
        //bit_7
        {CH_4},
        //bit_8
        {CH_4},
        //bit_9
        {CH_4},
        //bit_10
        {CH_4},
        //bit_11
        {CH_4},
        //bit_12
        {CH_4},
        //bit_13 Замок потребителя
        {CH_KZ, CH_Ka, CH_Km, CH_Ko, CH_Kk, CH_SPACE, CH_Kp, CH_Ko, CH_Kt, CH_Kr, CH_Ke, CH_Kb, CH_Ki, CH_Kt, CH_Ke, CH_Kl, CH_Kya},
        //bit_14
        {CH_4},
        //bit_15
        {CH_4}
    },
    //st_5
    {
        //bit_0 Ошибка К.Кор.
        {CH_KO, CH_Ksh, CH_Ki, CH_Kb, CH_Kk, CH_Ka, CH_SPACE, CH_KK, CH_POINT, CH_KK, CH_Ko, CH_Kr, CH_POINT},
        //bit_1 Сигнал Т
        {CH_KS, CH_Ki, CH_Kg, CH_Kn, CH_Ka, CH_Kl, CH_SPACE, CH_KT},
        //bit_2
        {CH_5},
        //bit_3
        {CH_5},
        //bit_4
        {CH_5},
        //bit_5
        {CH_5},
        //bit_6
        {CH_5},
        //bit_7
        {CH_5},
        //bit_8
        {CH_5},
        //bit_9
        {CH_5},
        //bit_10
        {CH_5},
        //bit_11
        {CH_5},
        //bit_12 Сигнал Вх1
        {CH_KS, CH_Ki, CH_Kg, CH_Kn, CH_Ka, CH_Kl, CH_SPACE, CH_KV, CH_Kh, CH_1},
        //bit_13
        {CH_5},
        //bit_14
        {CH_5},
        //bit_15
        {CH_5}
    },
    //st_6
    {
        //bit_0 Граница Т
        {CH_KG, CH_Kr, CH_Ka, CH_Kn, CH_Ki, CH_Kts, CH_Ka, CH_SPACE, CH_KT},
        //bit_1
        {CH_6},
        //bit_2
        {CH_6},
        //bit_3
        {CH_6},
        //bit_4
        {CH_6},
        //bit_5 Граница Т п
        {CH_KG, CH_Kr, CH_Ka, CH_Kn, CH_Ki, CH_Kts, CH_Ka, CH_SPACE, CH_KT, CH_SPACE, CH_Kp},
        //bit_6
        {CH_6},
        //bit_7
        {CH_6},
        //bit_8
        {CH_6},
        //bit_9
        {CH_6},
        //bit_10
        {CH_6},
        //bit_11
        {CH_6},
        //bit_12 Магнит
        {CH_KM, CH_Ka, CH_Kg, CH_Kn, CH_Ki, CH_Kt},
        //bit_13
        {CH_6},
        //bit_14
        {CH_6},
        //bit_15
        {CH_6}
    },
    //st_7
    {
        //bit_0
        {CH_7},
        //bit_1
        {CH_7},
        //bit_2
        {CH_7},
        //bit_3
        {CH_7},
        //bit_4
        {CH_7},
        //bit_5
        {CH_7},
        //bit_6
        {CH_7},
        //bit_7
        {CH_7},
        //bit_8
        {CH_7},
        //bit_9
        {CH_7},
        //bit_10
        {CH_7},
        //bit_11
        {CH_7},
        //bit_12 о Бат. отсек открыт
        {CH_KB, CH_Ka, CH_Kt, CH_POINT, CH_Ko, CH_Kt, CH_Ks, CH_Ke, CH_Kk, CH_SPACE, CH_Ko, CH_Kt, CH_Kk, CH_Kr, CH_Ky, CH_Kt},
        //bit_13
        {CH_7},
        //bit_14
        {CH_7},
        //bit_15
        {CH_7}
    },
    //st_8
    {
        //bit_0
        {CH_8},
        //bit_1
        {CH_8},
        //bit_2
        {CH_8},
        //bit_3
        {CH_8},
        //bit_4
        {CH_8},
        //bit_5
        {CH_8},
        //bit_6
        {CH_8},
        //bit_7
        {CH_8},
        //bit_8
        {CH_8},
        //bit_9
        {CH_8},
        //bit_10
        {CH_8},
        //bit_11
        {CH_8},
        //bit_12 корпус открыт
        {CH_KK, CH_Ko, CH_Kr, CH_Kp, CH_Ku, CH_Ks, CH_SPACE, CH_Ko, CH_Kt, CH_Kk, CH_Kr, CH_Ky, CH_Kt},
        //bit_13
        {CH_8},
        //bit_14 передача данных опт
        {CH_KP, CH_Ke, CH_Kr, CH_Ke, CH_Kd, CH_Ka, CH_Kch, CH_Ka, CH_SPACE, CH_Kd, CH_Ka, CH_Kn, CH_Kn, CH_Ky, CH_Kh, CH_SPACE, CH_Ko, CH_Kp, CH_Kt, CH_POINT},
        //bit_15
        {CH_8}
    },
    //st_9
    {
        //bit_0
        {CH_9},
        //bit_1
        {CH_9},
        //bit_2
        {CH_9},
        //bit_3
        {CH_9},
        //bit_4
        {CH_9},
        //bit_5
        {CH_9},
        //bit_6
        {CH_9},
        //bit_7
        {CH_9},
        //bit_8
        {CH_9},
        //bit_9
        {CH_9},
        //bit_10
        {CH_9},
        //bit_11
        {CH_9},
        //bit_12
        {CH_9},
        //bit_13
        {CH_9},
        //bit_14
        {CH_9},
        //bit_15
        {CH_9}
    }
};