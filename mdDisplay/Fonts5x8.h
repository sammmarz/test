#ifndef Fonts5x8_h
#define Fonts5x8_h

#include <stdint.h>

typedef uint8_t CHAR_TYPE;

extern char Font5x8 [][5];

enum
{
    POSITION_ZERO_IN_FONT5x8 = 16,
    POSITION_SPACE_IN_FONT5x8 = 0
};

enum CH
{
    CH_SPACE = 0, // (space)
    CH_ALERT = 1, // !
    CH_DQUOT = 2, // "
    CH_SHARP = 3, // #
    CH_DOLLAR = 4, // $
    CH_PERCENT = 5, // %
    CH_AMPER = 6, // &
    CH_QUOT = 7, // '
    CH_BRACKET_B = 8, // (
    CH_BRACKET_E = 9, // )
    CH_MULTIPLY = 10, // *
    CH_PlUS = 11, // +
    CH_COMMA = 12, // ,
    CH_DASH = 13, // -
    CH_POINT = 14, // .
    CH_SLASH = 15, // /
    CH_0 = 16, // 0
    CH_1 = 17, // 1
    CH_2 = 18, // 2
    CH_3 = 19, // 3
    CH_4 = 20, // 4
    CH_5 = 21, // 5
    CH_6 = 22, // 6
    CH_7 = 23, // 7
    CH_8 = 24, // 8
    CH_9 = 25, // 9
    CH_DPOINT = 26, // :
    CH_POINT_COMMA = 27, // ;
    CH_LESS = 28, // <
    CH_EQUALLY = 29, // =
    CH_MORE = 30, // >
    CH_QUEST = 31, // ?
    CH_DOG = 32, // @
    CH_A = 33, // A
    CH_B = 34, // B
    CH_C = 35, // C
    CH_D = 36, // D
    CH_E = 37, // E
    CH_F = 38, // F
    CH_G = 39, // G
    CH_H = 40, // H
    CH_I = 41, // I
    CH_J = 42, // J
    CH_K = 43, // K
    CH_L = 44, // L
    CH_M = 45, // M
    CH_N = 46, // N
    CH_O = 47, // O
    CH_P = 48, // P
    CH_Q = 49, // Q
    CH_R = 50, // R
    CH_S = 51, // S
    CH_T = 52, // T
    CH_U = 53, // U
    CH_V = 54, // V
    CH_W = 55, // W
    CH_X = 56, // X
    CH_Y = 57, // Y
    CH_Z = 58, // Z
    CH_BRACKET_B_SQ = 59, // [
    CH_SLASH_REV = 60, // "\"
    CH_BRACKET_E_SQ = 61, // ]
    CH_TRIANG = 62, // ^
    CH_UNDERLINE = 63, // _
    CH_QUOT2 = 64, // `
    CH_a = 65, // a
    CH_b = 66, // b
    CH_c = 67, // c
    CH_d = 68, // d
    CH_e = 69, // e
    CH_f = 70, // f
    CH_g = 71, // g
    CH_h = 72, // h
    CH_i = 73, // i
    CH_j = 74, // j
    CH_k = 75, // k
    CH_l = 76, // l
    CH_m = 77, // m
    CH_n = 78, // n
    CH_o = 79, // o
    CH_p = 80, // p
    CH_q = 81, // q
    CH_r = 82, // r
    CH_s = 83, // s
    CH_t = 84, // t
    CH_u = 85, // u
    CH_v = 86, // v
    CH_w = 87, // w
    CH_x = 88, // x
    CH_y = 89, // y
    CH_z = 90, // z
    CH_BRACKET_B_FIG = 91, // {
    CH_OR = 92, // |
    CH_BRACKET_E_FIG = 93, // }
    CH_RIGHT = 94, // ->
    CH_LEFT = 95, // <-
    //kiril
    CH_KA = 96, //A	0x80
    CH_KB = 97, //Б	0x81
    CH_KV = 98, //В	0x82
    CH_KG = 99, //Г	0x83
    CH_KD = 100, //Д	0x84
    CH_KE = 101, //E	0x85
    CH_KZH = 102, //Ж	0x86
    CH_KZ = 103, //З	0x87
    CH_KI = 104, //И	0x88
    CH_KIY = 105, //Й	0x89
    CH_KK = 106, //K	0x8A
    CH_KL = 107, //Л	0x8B
    CH_KM = 108, //M	0x8C
    CH_KN = 109, //H	0x8D
    CH_KO = 110, //O	0x8E
    CH_KP = 111, //П	0x8F
    CH_KR = 112, //P	0x90
    CH_KS = 113, //C	0x91
    CH_KT = 114, //T	0x92
    CH_KU = 115, //У	0x93
    CH_KF = 116, //Ф	0x94
    CH_KH = 117, //X	0x95
    CH_KTS = 118, //Ц	0x96
    CH_KCH = 119, //Ч	0x97
    CH_KSH = 120, //Ш	0x98
    CH_KSHCH = 121, //Щ	0x99
    CH_KHARD = 122, //Ъ	0x9A
    CH_KY = 123, //Ы	0x9B
    CH_KEA = 124, //Э	0x9C
    CH_KSOFT = 125, //Ь	0x9D
    CH_KYU = 126, //Ю	0x9E
    CH_KYA = 127, //Я	0x9F
    // маленькие буквы
    CH_Ka = 128, //a	0xA0
    CH_Kb = 129, //б	0xA1
    CH_Kv = 130, //в	0xA2
    CH_Kg = 131, //г	0xA3
    CH_Kd = 132, //д	0xA4
    CH_Ke = 133, //e	0xA5
    CH_Kzh = 134, //ж	0xA6
    CH_Kz = 135, //з	0xA7
    CH_Ki = 136, //и	0xA8
    CH_Kiy = 137, //й	0xA9
    CH_Kk = 138, //к	0xAA
    CH_Kl = 139, //л	0xAB
    CH_Km = 140, //м	0xAC
    CH_Kn = 141, //н	0xAD
    CH_Ko = 142, //o	0xAE
    CH_Kp = 143, //п	0xAF
    CH_Kr = 144, //p 	0xB0
    CH_Ks = 145, //c	0xB1
    CH_Kt = 146, //т	0xB2
    CH_Ku = 147, //у	0xB3
    CH_Kf = 148, //ф	0xB4
    CH_Kh = 149, //x	0xB5
    CH_Kts = 150, //ц	0xB6
    CH_Kch = 151, //ч	0xB7
    CH_Ksh = 152, //ш	0xB8
    CH_Kshch = 153, //щ	0xB9
    CH_Khard = 154, //ъ	0xBA
    CH_Ky = 155, //ы	0xBB
    CH_Ksoft = 156, //ь	0xBC
    CH_Kea = 157, //э	0xBD
    CH_Kyu = 158, //ю	0xBE
    CH_Kya = 159, //я	0xBF
    CH_CIRCLEB = 160, //круг начало
    CH_CIRCLEE = 161, //круг конец
    CH_DEGR = 162 //градус
};
#endif