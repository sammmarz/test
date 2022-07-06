/**
 \file
Файл содержит массивы индексов символов шрифтов
*/
#include <stdint.h>
#include "Fonts5x8.h"
#include "Fonts12x16.h"

const CHAR_TYPE sTekusheePokazanie[17] = {CH_KT, CH_Ke, CH_Kk, CH_Ku, CH_Kshch, CH_Ke, CH_Ke, CH_SPACE, CH_Kp, CH_Ko, CH_Kk, CH_Ka, CH_Kz, CH_Ka, CH_Kn, CH_Ki, CH_Ke};
const CHAR_TYPE sM3[2] = {CH_Km, CH_3};
const CHAR_TYPE sM3_Ch[4] = {CH_Km, CH_3, CH_SLASH, CH_Kch};
const CHAR_TYPE sUTC[3] = {CH_U, CH_T, CH_C};
const CHAR_TYPE sVolumes[5] = {CH_KO, CH_Kb, CH_Khard, CH_Ke, CH_Km};
const CHAR_TYPE sVco[4] = {CH_V, CH_c, CH_o, CH_DPOINT};
const CHAR_TYPE sVpo[4] = {CH_V, CH_p, CH_o, CH_DPOINT};
const CHAR_TYPE sVc[3] = {CH_V, CH_c, CH_DPOINT};
const CHAR_TYPE sVp[3] = {CH_V, CH_p, CH_DPOINT};

const CHAR_TYPE sTrevogi[7] = {CH_KT, CH_Kr, CH_Ke, CH_Kv, CH_Ko, CH_Kg, CH_Ki};

//const CHAR_TYPE sPodst_koef[11] = {CH_KP, CH_Ko, CH_Kd, CH_Ks, CH_Kt, CH_POINT, CH_Kk, CH_Ko, CH_Kea, CH_Kf, CH_POINT};
const CHAR_TYPE sParams[9] = {CH_KP, CH_Ka, CH_Kr, CH_Ka, CH_Km, CH_Ke, CH_Kt, CH_Kr, CH_Ky};
const CHAR_TYPE sT_izmer[9] = {CH_KT, CH_SPACE, CH_Ki, CH_Kz, CH_Km, CH_Ke, CH_Kr, CH_POINT, CH_DPOINT};
const CHAR_TYPE sP_podst[9] = {CH_P, CH_SPACE, CH_Kp, CH_Ko, CH_Kd, CH_Ks, CH_Kt, CH_POINT, CH_DPOINT};
const CHAR_TYPE sT_podst[9] = {CH_KT, CH_SPACE, CH_Kp, CH_Ko, CH_Kd, CH_Ks, CH_Kt, CH_POINT, CH_DPOINT};
const CHAR_TYPE sKszh[7] = {CH_KK, CH_SPACE, CH_Ks, CH_Kzh, CH_POINT, CH_SPACE, CH_DPOINT};
const CHAR_TYPE sKkor[7] = {CH_KK, CH_SPACE, CH_Kk, CH_Ko, CH_Kr, CH_POINT, CH_DPOINT};
const CHAR_TYPE sPa[2] = {CH_KP, CH_Ka};
const CHAR_TYPE sDegr[2] = {CH_DEGR, CH_C};

const CHAR_TYPE sRashod[6] = {CH_KR, CH_Ka, CH_Ks, CH_Kh, CH_Ko, CH_Kd};
const CHAR_TYPE sQmax[6] = {CH_Q, CH_m, CH_a, CH_x, CH_SPACE, CH_DPOINT};
const CHAR_TYPE sQmin[6] = {CH_Q, CH_m, CH_i, CH_n, CH_SPACE, CH_DPOINT};
const CHAR_TYPE sQpPn[6] = {CH_Q, CH_p, CH_KP, CH_KN, CH_SPACE, CH_DPOINT};
const CHAR_TYPE sQpPv[6] = {CH_Q, CH_p, CH_KP, CH_KV, CH_SPACE, CH_DPOINT};

const CHAR_TYPE sPO[2] = {CH_KP, CH_KO};
const CHAR_TYPE sVersM[7] = {CH_KV, CH_Ke, CH_Kr, CH_Ks, CH_KM, CH_POINT, CH_DPOINT};
const CHAR_TYPE sVers[6] = {CH_KV, CH_Ke, CH_Kr, CH_Ks, CH_POINT, CH_DPOINT};
const CHAR_TYPE sCRCM[5] = {CH_C, CH_R, CH_C, CH_M, CH_DPOINT};
const CHAR_TYPE sCRC[4] = {CH_C, CH_R, CH_C, CH_DPOINT};

const CHAR_TYPE sBatteries[7] = {CH_KB, CH_Ka, CH_Kt, CH_Ka, CH_Kr, CH_Ke, CH_Ki};
const CHAR_TYPE sBattery_1[9] = {CH_KB, CH_Ka, CH_Kt, CH_Ka, CH_Kr, CH_Ke, CH_Kya, CH_SPACE, CH_1};
const CHAR_TYPE sBattery_2[9] = {CH_KB, CH_Ka, CH_Kt, CH_Ka, CH_Kr, CH_Ke, CH_Kya, CH_SPACE, CH_2};
const CHAR_TYPE sOstatSrok[11] = {CH_KO, CH_Ks, CH_Kt, CH_Ka, CH_Kt, CH_POINT, CH_Ks, CH_Kr, CH_Ko, CH_Kk, CH_DPOINT};
const CHAR_TYPE sMes[4] = {CH_Km, CH_Ke, CH_Ks, CH_POINT};

const CHAR_TYPE sOptPort[15] = {CH_KO, CH_Kp, CH_Kt, CH_Ki, CH_Kch, CH_Ke, CH_Ks, CH_Kk, CH_Ki, CH_Kiy, CH_SPACE, CH_Kp, CH_Ko, CH_Kr, CH_Kt};
const CHAR_TYPE sStatus[7] = {CH_KS, CH_Kt, CH_Ka, CH_Kt, CH_Ku, CH_Ks, CH_DPOINT};
const CHAR_TYPE sVikl[5] = {CH_KV, CH_Ky, CH_Kk, CH_Kl, CH_POINT};
const CHAR_TYPE sVkl[4] = {CH_KV, CH_Kk, CH_Kl, CH_POINT};
const CHAR_TYPE sCircle3[5] = {CH_CIRCLEB, CH_CIRCLEE, CH_SPACE, CH_3, CH_SPACE};
const CHAR_TYPE sSecForVkl[15] = {CH_Ks, CH_Ke, CH_Kk, CH_POINT, CH_SPACE, CH_SPACE, CH_SPACE, CH_Kd, CH_Kl, CH_Kya, CH_SPACE, CH_KV, CH_Kk, CH_Kl, CH_POINT};
const CHAR_TYPE sSecForVikl[16] = {CH_Ks, CH_Ke, CH_Kk, CH_POINT, CH_SPACE, CH_SPACE, CH_SPACE, CH_Kd, CH_Kl, CH_Kya, CH_SPACE, CH_KV, CH_Ky, CH_Kk, CH_Kl, CH_POINT};
const CHAR_TYPE sChangeBat[17] = {CH_Ks, CH_Ke, CH_Kk, CH_POINT, CH_SPACE, CH_Kz, CH_Ka, CH_Km, CH_Ke, CH_Kn, CH_Ka, CH_SPACE, CH_Kb, CH_Ka, CH_Kt, CH_POINT};
const CHAR_TYPE sAcceptNewBat[21] = {CH_KP, CH_Ko, CH_Kd, CH_Kt, CH_Kv, CH_Ke, CH_Kr, CH_Kd, CH_Ki, CH_Kt, CH_Ksoft, CH_SPACE, CH_Ku, CH_Ks, CH_Kt, CH_Ka, CH_Kn, CH_Ko, CH_Kv, CH_Kk, CH_Ku};
const CHAR_TYPE sAcceptNewBat2[14] = {CH_Kn, CH_Ko, CH_Kv, CH_Ko, CH_Kiy, CH_SPACE, CH_Kb, CH_Ka, CH_Kt, CH_Ka, CH_Kr, CH_Ke, CH_Ki, CH_QUEST};
const CHAR_TYPE sGPRS[4] = {CH_G, CH_P, CH_R, CH_S};
const CHAR_TYPE sSettingsGPRS[14] = {CH_KN, CH_Ka, CH_Ks, CH_Kt, CH_Kr, CH_Ko, CH_Kiy, CH_Kk, CH_Ki, CH_SPACE, CH_G, CH_P, CH_R, CH_S};
const CHAR_TYPE sRSSI[5] = {CH_R, CH_S, CH_S, CH_I, CH_DPOINT};
const CHAR_TYPE sAPN[4] = {CH_A, CH_P, CH_N, CH_DPOINT};
const CHAR_TYPE sIP[3] = {CH_I, CH_P, CH_DPOINT};
