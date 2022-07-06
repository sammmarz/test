/**
 \file
Файл содержит структуры для заполнения данными, которые необходимо хранить в
энергонезависимой памяти. Также описывает месторасположения данных во флеш.
*/
#include "DataForFlash.h"
#include "../Params/Params.h"

//глобальный счётчик для всех структур данных, записываемых во флеш
uint32_t globCountFlash;
//
StrChangesArchive changesArchive;
StrFlashInfoData changesArchiveInfo;
//
StrIntervalArchive intervalArchive;
StrFlashInfoData intervalArchiveInfo;
//
StrEventsArchive eventsArchive;
StrFlashInfoData eventsArchiveInfo;
//
StrPeriodData periodData;
StrFlashInfoData periodDataInfo;
//
StrMonthArchive monthArchive;
StrFlashInfoData monthArchiveInfo;
//
StrDayArchive dayArchive;
StrFlashInfoData dayArchiveInfo;

uint8_t bufProgram[256];
uint32_t indxBufProgram;
StrFlashInfoData bufProgramInfo;

StrFlashInfoData paramsInfo;
/**
 \ingroup mdFlashMemoryGroup
 \author Shuvalov Sergey
 \brief Функция описывает расположения данных во флеш-памяти.

 Общий объём памяти 0х7fff ff - 0 (65535 страниц по 256 байт).
 Данные необходимо располагать кратно 4 КБайт 0х1000 (0-0xfff).
 Данные во флеш должны быть выравнены по страницам, т.е. если
 структура занимает 530 байт для нее должно быть отведено 3 страницы
 (256 + 256 + 18 следующая структура пишется начиная с 4-й страницы).
*/
void initInfoStructDataForFlash(void)
{
    // интервальный архив Глубина интервального архива – не менее 5000 записей.
    //  0x00 00 - 0x223f ff (8767 страниц (8760(24*365) + выравнивание) по 256 байт)
    intervalArchiveInfo.adress = 0x0000;
    intervalArchiveInfo.adressFlashBegin = 0x0000;
    intervalArchiveInfo.adressFlashEnd = 0x223fff;
    //
    // архив изменений Глубина архива изменений – не менее 200 записей.
    // 0x2240 00 - 0x262f ff (1008 страниц по 256 байт)
    changesArchiveInfo.adress = 0x224000 ;
    changesArchiveInfo.adressFlashBegin = 0x224000;
    changesArchiveInfo.adressFlashEnd = 0x262fff;
    //
    // периодические данные сохранять раз в час (на случай перезагрузки)
    // 0x2630 00 - 0x2a1f ff (1008 страниц по 256 байт)
    periodDataInfo.adress = 0x263000;
    periodDataInfo.adressFlashBegin = 0x263000;
    periodDataInfo.adressFlashEnd = 0x2a1fff;
    //
    // архив событий Глубина архива событий – не менее 2000 записей.
    // 0x2a20 00 - 0x54ef ff (10960 страниц по 256 байт)
    eventsArchiveInfo.adress = 0x2a2000;
    eventsArchiveInfo.adressFlashBegin = 0x2a2000;
    eventsArchiveInfo.adressFlashEnd = 0x54efff;
    //
    // месячный архив Глубина месячного архива – не менее 120 записей.
    // 0x54f0 00 - 0x557f ff (144 (10 лет * 12 + выравнивание) страниц по 256 байт)
    monthArchiveInfo.adress = 0x54f000;
    monthArchiveInfo.adressFlashBegin = 0x54f000;
    monthArchiveInfo.adressFlashEnd = 0x557fff;
    //
    // суточный архив Глубина суточного архива – не менее 730 записей.
    // 0x5580 00 - 0x585f ff (736 (365 суток * 2 года + выравнивание) страниц по 256 байт)
    dayArchiveInfo.adress = 0x558000;
    dayArchiveInfo.adressFlashBegin = 0x558000;
    dayArchiveInfo.adressFlashEnd = 0x5859ff;
    //
    // для хранения параметров устройства 16 блоков по 4 КБ
    // 0x5860 00 - 0x595f ff
    paramsInfo.adress = 0x586000;
    paramsInfo.adressFlashBegin = 0x586000;
    paramsInfo.adressFlashEnd = 0x595FFF;
    paramsInfo.flags = BLOCK_4KB;
    paramsInfo.size = sizeof(StrCorrectorParams);
    //
    // для хранения прошива выделяе 512 000
    // 0x7830 00 - 0x7fff ff
    bufProgramInfo.adress = 0x783000; // при смене поменять и в mdLis200
    bufProgramInfo.adressFlashBegin = 0x783000;
    bufProgramInfo.adressFlashEnd = 0x7FFFFF;
    //
    // резерв 0x5960 00 - 0x7830 ff (7888 страниц)
    //
}