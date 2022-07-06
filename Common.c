/**
 \file
Файл содержит общие элементы для всей прошивки.
*/
#include "Common.h"

typedef struct  
{
    char ver_meter[10];//номер версии метрологической части ПО
    char ver_ne_meter[10];//номер версии неметрологической части ПО
    char id[10];//идентификатор прибора
    unsigned short hard_ver;//версия платы
    unsigned short crc16_meter;//контрольная сумма метрологической части
    unsigned short crc16_ne_meter;//контрольная сумма неметрологической части
    unsigned int size_meter;//размер метрологической части
    
    unsigned int size_ne_meter;//размер неметрологической части
    unsigned int size_file;//размер файла прошива
    char rez1;
    char rez2;
    char rez3;
    char rez4;
    char rez5;
    char rez6;
    unsigned short crc16_str;//контрольная сумма структуры 
} StrIDData; 


#pragma location = "ID_REGION"
__root const StrIDData str_id = {
  .ver_meter = VER_SOFT_METR,
    .ver_ne_meter = VER_SOFT_MAIN,
    .id = "BK-G_TC",
    .hard_ver = 0x2,
    .crc16_meter = 0xf1f2,
    .crc16_ne_meter = 0xc60,
    .size_meter = csizeofmeter,
    .size_ne_meter = 0x7f,
    .size_file = 0xFFFFFFFF,
    .rez1 = 0x9f,
    .rez2 = 0xAF,
    .rez3 = 0xBF,
    .rez4 = 0xCF,
    .rez5 = 0xDF,
    .rez6 = 0xEF,
    .crc16_str = 0xff
}; 