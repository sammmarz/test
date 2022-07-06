/**
 \file
Файл содержит описательную структуру для параметров.
*/
#include "Params.h"
#include "Events.h"

void RdUint (unsigned indx, unsigned char *p_Buff, void *value);
void RdInt (unsigned indx, unsigned char *p_Buff, void *value);
void RdUintHex (unsigned indx, unsigned char *p_Buff, void *value);
void RdString (unsigned indx, unsigned char *p_Buff, void *value);
void RdDateTime(unsigned indx, unsigned char *p_Buff, void *value);
void RdFTPdateTime (unsigned indx, unsigned char *p_Buff, void *value);
void RdUTC (unsigned indx, unsigned char *p_Buff, void *value);
void RdFloat (unsigned indx, unsigned char *p_Buff, void *value);
void RdTemperature (unsigned indx, unsigned char *p_Buff, void *value);
void RdStatus (unsigned indx, unsigned char *p_Buff, void *value);
void RdAccessStatus (unsigned indx, unsigned char *p_Buff, void *value);
void RdPassw (unsigned indx, unsigned char *p_Buff, void *value);
void RdTrigger (unsigned indx, unsigned char *p_Buff, void *value);
void RdStatuses (unsigned indx, unsigned char *p_Buff, void *value);
void RdAccessProgUpdateStatus (unsigned indx, unsigned char *p_Buff, void *value);
void RdTemperCalibPoint (unsigned indx, unsigned char *p_Buff, void *value);
void RdTemperCalibPointTS (unsigned indx, unsigned char *p_Buff, void *value);
void RdKoefFromTS (unsigned indx, unsigned char *p_Buff, void *value);
void RdArchivesString (unsigned indx, unsigned char *p_Buff, void *value);
unsigned int WrtUint (unsigned indx, unsigned char *p_Buff);
unsigned int WrtString (unsigned indx, unsigned char *p_Buff);
unsigned int WrtDateTime(unsigned indx, unsigned char *p_Buff);
unsigned int WrtFTPdataTime(unsigned indx, unsigned char *p_Buff);
unsigned int WrtUTC(unsigned indx, unsigned char *p_Buff);
unsigned int WrtValueForTrigger(unsigned indx, unsigned char *p_Buff);
unsigned int saveInFlash(unsigned indx, unsigned char *p_Buff);
unsigned int resetArchCount(unsigned indx, unsigned char *p_Buff);
unsigned int resetDevice(unsigned indx, unsigned char *p_Buff);
unsigned int resetAlerts(unsigned indx, unsigned char *p_Buff);
unsigned int GateControl(unsigned indx, unsigned char *p_Buff);
unsigned int WrtPeriodTime(unsigned indx, unsigned char *p_Buff);
unsigned int WrtFloat(unsigned indx, unsigned char *p_Buff);
unsigned int WrtTemperature(unsigned indx, unsigned char *p_Buff);
unsigned int stopAccess(unsigned indx, unsigned char *p_Buff);
unsigned int openAccessChangePass(unsigned indx, unsigned char *p_Buff);
unsigned int openAccessManuf (unsigned i, unsigned char *p_Buff);
unsigned int WritePassProgUpdate(unsigned i, unsigned char *p_Buff);
unsigned int StartProgUpdate(unsigned i, unsigned char *p_Buff);
unsigned int CmdResetArchive (unsigned indx, unsigned char *p_Buff);
unsigned int WrtTemperForCalibPoint(unsigned indx, unsigned char *p_Buff);
unsigned int WrtKoefForTS(unsigned indx, unsigned char *p_Buff);
unsigned int SaveTemperAtCalibPoint(unsigned indx, unsigned char *p_Buff);
unsigned int CountKoefForRange(unsigned indx, unsigned char *p_Buff);

// единицы измерений string unit of measurent
char sUmH[] = "\\xf7\0";                            // ч
char sUmA_H[] = "\\xEC\\xEA\\xF0\\xC0\\x2A\\xF7\0"; // мкрA*ч
char sUmMonth[] = "\\xEC\\xE5\\xF1.\0";             // мес.    
char sUmSec[] = "\\xF1\\xE5\\xEA.\0";               // сек.
char sUmC[] = "C\0";                                // C
char sUmMlC[] = "\\xEC\\xEB\\xD1\\xE5\\xEA\0";      // млCек
char sUmK[] = "K\0";                                // K
char sUmM3_Imp[] = "\\xEC\x33\\\\xE8\0";            // м3\и
char sUmM3[] = "\\xEC\x33\0";                       // м3
char sUmM3_H[] = "\\xEC\\x33\\\\xF7\0";             // м3\ч
char sUmM3_Pa[] = "\\xCF\\xE0\0";                   // Па
char sUmM3_Perc[] = "\\x25\0";                      // %
char sUmM3_PA[] = "Pa\0";                           // Pa

StrDescribedParams DataTable[] =
{
 {0x141, 2, 0, &params.dayBegin, 0xBF, "Pc.BP", sUmH, DC_Counter, &def.dayBegin, VLR_0_23, MT_RAM_ROM, DmC_No, F_Hour, DT_Uchar, WrtUint, RdUint}, // Граница дня, начало газового дня
 {0x180, 1, 0, params.identDevice, 0xAF, "No.KP", 0, DC_Permanent, def.identDevice, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array12, WrtString, RdString},//Серийный номер счётчика
 {0x181, 1, 0, params.typeDevice, 0xAF, "YCTP.", 0, DC_Permanent, def.typeDevice, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array16, WrtString, RdString},// тип счётчика
 {0x181, 2, 0, &params.verBoard, 0xAF, "verB", 0, DC_Permanent, &def.verBoard, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, WrtUint, RdUint},// версия платы
 {0x180, 2, 0, params.Maker, 0xAF, "Maker", 0, DC_Permanent, def.Maker, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array32, WrtString, RdString},// изготовитель
 {0x190, 2, 0, params.versMain, 0xAA, "Ver.Mn", 0, DC_Const, def.versMain, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array6, 0, RdString},// версия основного ПО
 {0x191, 2, 0, &params.crcMain, 0xAA, "CRC.Mn", 0, DC_Const, &def.crcMain, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, 0, RdUintHex},// crc основного ПО
 {0x190, 1, 0, params.versMeter, 0xAA, "Ver.Mtr", 0, DC_Const, def.versMeter, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array6, 0, RdString},// версия метрологического ПО
 {0x191, 1, 0, &params.crcMeter, 0xAA, "CRC.Mtr", 0, DC_Const, &def.crcMeter, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, 0, RdUintHex},// crc метрологического ПО
 {0x400, 1, 0, 0, 0xBF, "Time", 0, DC_Counter, 0, VLR_0, MT_RAM_ROM, DmC_No, F_DateTime, DT_TCLOCK, WrtDateTime, RdDateTime},// дата время
 {0x401, 1, 0, &params.utc, 0xBF, "UTC", 0, DC_Permanent, &def.utc, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Sshort, WrtUTC, RdUTC},// UTC
 {0x1F3, 1, 0, &params.volBat1, 0xBF, "VolBat1", sUmA_H, DC_Counter, &def.volBat1, VLR_0, MT_RAM_ROM, DmC_No, F_D1, DT_Float, 0, RdFloat},// емкость батареи
 {0x1F3, 21, 0, &params.volBat2, 0xBF, "VolBat2", sUmA_H, DC_Counter, &def.volBat2, VLR_0, MT_RAM_ROM, DmC_No, F_D1, DT_Float, 0, RdFloat},// емкость батареи 2
 {0x404, 2, 0, &params.lifeBat1, 0xAA, "LifBat1", sUmMonth, DC_Counter, &def.lifeBat1, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Uchar, 0, RdUint},// остаточный срок службы батареи 1
 {0x404, 22, 0, &params.lifeBat2, 0xAA, "LifBat2", sUmMonth, DC_Counter, &def.lifeBat2, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Uchar, 0, RdUint},// остаточный срок службы батареи 2
 {0x223, 5, 0, params.typeTempDevice, 0xAF, "Ttype", 0, DC_Permanent, def.typeTempDevice, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Array8, WrtString, RdString},// тип термодатчика
 {0x1F0, 1, 0, &params.measPeriod, 0xAF, "measPer", sUmSec, DC_Permanent, &def.measPeriod, VLR_5_3600_5, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, WrtUint, RdUint},// период цикла измерений
 {0x1A0, 2, 0, &params.lcdTimeOff, 0xBF, "LCDtOff", sUmSec, DC_Permanent, &def.lcdTimeOff, VLR_20_300, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, WrtUint, RdUint},// время перед отключением дисплея
 {0x1A0, 1, 0, &params.lcdTimeToMain, 0xBF, "LCDtoM", sUmSec, DC_Permanent, &def.lcdTimeToMain, VLR_20_300, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, WrtUint, RdUint},// время переключения дисплея на основное меню
 {0x424, 3, 0, &params.envirTempRange, 0xAF, "envirTr", sUmC, DC_Permanent, &def.envirTempRange, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array12, WrtString, RdString},// диапазон Т окр. среды
 {0x452, 1, 0, &params.koefTimer, 0xAF, "koefTim", 0, DC_Const, &def.koefTimer, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Uchar, 0, RdUint},//  Коэффициент настройки часов
 {0x131, 1, 0, 0, 0x05, "SaveMem", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, saveInFlash, 0},//  Принудительно сохранение данных в энергонезависимую память
 {0x130, 2, 0, 0, 0x05, "ResArcs", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, resetArchCount, 0},//  Сброс счётчиков и архивов
 {0x130, 1, 0, 0, 0x05, "ResDev", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, resetDevice, 0},//  Инициализация устройства (сброс до начальных настроек)
 {0x130, 4, 0, 0, 0x15, "ResAlrt", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, resetAlerts, 0},//  сброс тревог
 {0x253, 1, 0, &params.koefImpulseHall, 0xAF, "KoefHal", sUmM3_Imp, DC_Permanent, &def.koefImpulseHall, VLR_0, MT_RAM_ROM, DmC_No, F_D5, DT_Float, WrtFloat, RdFloat},//  Значение коэффициента преобразования импульсов
 {0x254, 1, 0, &params.maxPeriodsBetweenSignals, 0xBF, "maxPrds", 0, DC_Permanent, &def.maxPeriodsBetweenSignals, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Uchar, WrtUint, RdUint},//  Макс. количество циклов измерений до Q = 0
 {0x709, 1, 0, &params.speedUARTcode, 0xBF, "speedOp", 0, DC_Permanent, &def.speedUARTcode, VLR_0_6, MT_RAM_ROM, DmC_No, F_D0, DT_Uchar, WrtUint, RdUint},//  Скорость передачи по оптическому интерфейсу
 {0x300, 2, 0, &params.Vc, 0xAF, "Vc", sUmM3, DC_Counter, &def.Vc, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//  Счетчик стандартного объёма Vc (невозмущенный)
 {0x300, 4, 0, &params.Vp, 0xAF, "Vp", sUmM3, DC_Counter, &def.Vp, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Счетчик рабочего объёма Vр (невозмущенный)
 {0x301, 2, 0, &params.VcB, 0xBF, "VcB", sUmM3, DC_Counter, &def.VcB, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Счётчик возмущенного стандартного объёма Vс
 {0x301, 4, 0, &params.VpB, 0xBF, "VpB", sUmM3, DC_Counter, &def.VpB, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Счётчик возмущенного рабочего объёма Vр
 {0x302, 2, 0, &params.VcO, 0xAF, "VcO", sUmM3, DC_Counter, &def.VcO, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Общий счетчик стандартного объёма Vс
 {0x302, 4, 0, &params.VpO, 0xAF, "VpO", sUmM3, DC_Counter, &def.VpO, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Общий счетчик рабочего объёма Vр
 {0x160, 1, 0, &params.VcTI, 0xAF, "VcTI", sUmM3, DC_Counter, &def.VcTI, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный объём за текущий интервал архивирования
 {0x161, 1, 0, &params.VcOI, 0xAF, "VcOI", sUmM3, DC_Counter, &def.VcOI, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный объём за предыдущий интервал архивирования
 {0x160, 2, 0, &params.VcTC, 0xAF, "VcTC", sUmM3, DC_Counter, &def.VcTC, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный объём за текущие сутки
 {0x161, 2, 0, &params.VcOC, 0xAF, "VcOC", sUmM3, DC_Counter, &def.VcOC, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный объём за предыдущие сутки
 {0x160, 3, 0, &params.VcTM, 0xAF, "VcTM", sUmM3, DC_Counter, &def.VcTM, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный объём за текущий месяц
 {0x161, 3, 0, &params.VcOM, 0xAF, "VcOM", sUmM3, DC_Counter, &def.VcOM, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный объём за предыдущий месяц
 {0x160, 4, 0, &params.VpTI, 0xAF, "VpTI", sUmM3, DC_Counter, &def.VpTI, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Рабочий объём за текущий интервал архивирования
 {0x161, 4, 0, &params.VpOI, 0xAF, "VpOI", sUmM3, DC_Counter, &def.VpOI, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Рабочий объём за предыдущий интервал архивирования
 {0x160, 5, 0, &params.VpTC, 0xAA, "VpTC", sUmM3, DC_Counter, &def.VpTC, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Рабочий объём за текущие сутки
 {0x161, 5, 0, &params.VpOC, 0xAA, "VpOC", sUmM3, DC_Counter, &def.VpOC, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Рабочий объём за предыдущие сутки
 {0x160, 6, 0, &params.VpTM, 0xAA, "VpTM", sUmM3, DC_Counter, &def.VpTM, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Рабочий объём за текущий месяц
 {0x161, 6, 0, &params.VpOM, 0xAA, "VpOM", sUmM3, DC_Counter, &def.VpOM, VLR_0, MT_RAM_ROM, DmC_Volume, F_D4, DT_Float, WrtFloat, RdFloat},//Рабочий объём за предыдущий месяц
 {0x310, 2, 0, &params.Qc, 0xAA, "Qc", sUmM3_H, DC_MessW, &def.Qc, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный расход Qс
 {0x316, 2, 0, &params.QcAvI, 0xAA, "QcAvI", sUmM3_H, DC_MessW, &def.QcAvI, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный расход Qс (среднее за интервал)
 {0x317, 2, 0, &params.QcAvD, 0xAA, "QcAvD", sUmM3_H, DC_MessW, &def.QcAvD, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный расход Qс (среднее за сутки)
 {0x318, 2, 0, &params.QcAvM, 0xAA, "QcAvM", sUmM3_H, DC_MessW, &def.QcAvM, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Стандартный расход Qс (среднее за месяц)
 {0x310, 4, 0, &params.Qp, 0xAA, "Qp", sUmM3_H, DC_MessW, &def.Qp, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Рабочий расход Qр
 {0x210, 1, 0, &params.QTek, 0xAA, "QTek", sUmM3_H, DC_MessW, &def.QTek, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Измеренный рабочий расход QТек
 {0x310, 5, 0, &params.Kkor, 0xAA, "Kkor", 0, DC_Permanent, &def.Kkor, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Коэффициент коррекции
 {0x313, 5, 0, &params.KkorAvI, 0xAA, "KkorAvI", 0, DC_MessW, &def.KkorAvI, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Коэффициент коррекции  (среднее за интервал)
 {0x314, 5, 0, &params.KkorAvD, 0xAA, "KkorAvD", 0, DC_MessW, &def.KkorAvD, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Коэффициент коррекции  (среднее за сутки)
 {0x315, 5, 0, &params.KkorAvM, 0xAA, "KkorAvM", 0, DC_MessW, &def.KkorAvM, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Коэффициент коррекции  (среднее за месяц)
 {0x310, 6, 1, &params.T, 0xAA, "T", sUmC, DC_MessW, &def.T, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Температура газа T
 {0x310, 6, 0, &params.T, 0xAA, "T", sUmK, DC_MessW, &def.T, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Температура газа T
 {0x210, 5, 1, &params.Ttek, 0xAA, "Ttek", sUmC, DC_MessW, &def.Ttek, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Температура газа измеренная 
 {0x261, 6, 0, &params.TtekBin, 0xAA, "TtekB", 0, DC_MessW, &def.TtekBin, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Sshort, 0, RdInt},//Температура газа измеренная, бинарное представление 
 {0x311, 6, 1, &params.Tpod, 0xBF, "Tp", sUmC, DC_Permanent, &def.Tpod, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Температура газа подстановочная T. Под
 {0x311, 6, 0, &params.Tpod, 0xBF, "Tp", sUmK, DC_Permanent, &def.Tpod, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Температура газа подстановочная T. Под
 {0x311, 7, 0, &params.Ppod, 0xBF, "Ppod", sUmM3_Pa, DC_Permanent, &def.Ppod, VLR_P, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Давление газа подстановочное p
 {0x313, 7, 0, &params.PpodAvI, 0xAA, "PpodAvI", sUmM3_Pa, DC_Permanent, &def.PpodAvI, VLR_P, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Давление газа подстановочное (среднее за интервал)
 {0x314, 7, 0, &params.PpodAvD, 0xAA, "PpodAvD", sUmM3_Pa, DC_Permanent, &def.PpodAvD, VLR_P, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Давление газа подстановочное (среднее за сутки)
 {0x315, 7, 0, &params.PpodAvM, 0xAA, "PpodAvM", sUmM3_Pa, DC_Permanent, &def.PpodAvM, VLR_P, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Давление газа подстановочное (среднее за месяц)
 {0x315, 4, 0, &params.QpodD, 0xBF, "QpodD", sUmM3_H, DC_Permanent, &def.QpodD, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Нижнее подстановочное значение расхода
 {0x311, 4, 0, &params.QpodU, 0xBF, "QpodU", sUmM3_H, DC_Permanent, &def.QpodU, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Верхнее подстановочное значение расхода
 {0x311, 8, 0, &params.K, 0xBF, "K", 0, DC_Permanent, &def.K, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Коэффициент сжимаемости подстановочный K
 {0x313, 8, 0, &params.KaverI, 0xAA, "KaverI", 0, DC_Permanent, &def.KaverI, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Коэффициент сжимаемости подстановочный (среднее за интервал)
 {0x314, 8, 0, &params.KaverD, 0xAA, "KaverD", 0, DC_Permanent, &def.KaverD, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Коэффициент сжимаемости подстановочный (среднее за сутки)
 {0x315, 8, 0, &params.KaverM, 0xAA, "KaverM", 0, DC_Permanent, &def.KaverM, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Коэффициент сжимаемости подстановочный (среднее за месяц)
 {0x312, 6, 0, &params.Tstand, 0xAF, "Tstand", sUmK, DC_Permanent, &def.Tstand, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Стандартная температура Tс
 {0x312, 6, 1, &params.Tstand, 0xAF, "Tstand", sUmC, DC_Permanent, &def.Tstand, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Стандартная температура Tс
 {0x312, 7, 0, &params.Pstand, 0xAF, "Pstand", sUmM3_Pa, DC_Permanent, &def.Pstand, VLR_P, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Стандартное давление pс
 {0x3A0, 6, 1, &params.TalertU, 0xAF, "TalertU", sUmC, DC_Permanent, &def.TalertU, VLR_Cels, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Верхняя граница тревоги темп. газа T
 {0x3A8, 6, 1, &params.TalertD, 0xAF, "TalertD", sUmC, DC_Permanent, &def.TalertD, VLR_Cels, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Нижняя граница тревоги темп. газа T
 {0x3A0, 6, 0, &params.TalertU, 0xAF, "TalertU", sUmK, DC_Permanent, &def.TalertU, VLR_Kelv, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Верхняя граница тревоги темп. газа T
 {0x3A8, 6, 0, &params.TalertD, 0xAF, "TalertD", sUmK, DC_Permanent, &def.TalertU, VLR_Kelv, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Нижняя граница тревоги темп. газа T
 {0x158, 9, 1, &params.TwarnU, 0xAF, "TwarnU", sUmC, DC_Permanent, &def.TalertU, VLR_Cels, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Верхняя граница предупреждения темп. газа T
 {0x150, 9, 1, &params.TwarnD, 0xAF, "TwarnD", sUmC, DC_Permanent, &def.TalertD, VLR_Cels, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Нижняя граница предупреждения темп. газа T
 {0x158, 9, 0, &params.TwarnU, 0xAF, "TwarnU", sUmK, DC_Permanent, &def.TwarnU, VLR_Kelv, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Верхняя граница предупреждения темп. газа T
 {0x150, 9, 0, &params.TwarnD, 0xAF, "TwarnD", sUmK, DC_Permanent, &def.TwarnU, VLR_Kelv, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Нижняя граница предупреждения темп. газа
 {0x3A1, 6, 1, &params.ThystU, 0xBF, "ThystU", sUmC, DC_Permanent, &def.ThystU, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Гистерезис для максимальной температуры
 {0x3A9, 6, 1, &params.ThystD, 0xBF, "ThystD", sUmC, DC_Permanent, &def.ThystD, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Гистерезис для минимальной температуры
 {0x3A0, 4, 0, &params.QalertU, 0xBF, "QalertU", sUmM3_H, DC_Permanent, &def.QalertU, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Верхняя граница тревоги расхода Qmin
 {0x3A8, 4, 0, &params.QalertD, 0xBF, "QalertD", sUmM3_H, DC_Permanent, &def.QalertD, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Нижняя граница тревоги расхода Qmin
 {0x158, 8, 0, &params.QwarnU, 0xBF, "QwarnU", sUmM3_H, DC_Permanent, &def.QwarnU, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Верхняя граница предупреждения расхода Qmin
 {0x150, 8, 0, &params.QwarnD, 0xBF, "QwarnD", sUmM3_H, DC_Permanent, &def.QwarnD, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtFloat, RdFloat},//Нижняя граница предупреждения расхода Qmin
 
 {0x260, 1, 0, &params.calibPoints, 0xAF, "CalibP", sUmC, DC_Permanent, &def.calibPoints, VLR_Cels, MT_RAM_ROM, DmC_No, F_D3, DT_Float, WrtTemperForCalibPoint, RdTemperCalibPoint},// эталонное значение температуры в точках калибровки
 {0x227, 1, 0, &params.calibPointsSignalTS, 0xAA, "CalibPS", sUmC, DC_Permanent, &def.calibPointsSignalTS, VLR_Cels, MT_RAM_ROM, DmC_No, F_D3, DT_Float, 0, RdTemperCalibPointTS},// измеренное значение температуры в точках калибровки
 {0x280, 1, 0, &params.koefForTS, 0xAF, "Tkoef1", 0, DC_Permanent, &def.koefForTS, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, WrtKoefForTS, RdKoefFromTS},//Коэффициент 1 для уравнения температуры
 {0x262, 1, 0, 0, 0xAF, "saveT", 0, DC_Permanent, 0, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, SaveTemperAtCalibPoint, 0},// команда: "зафиксировать сигнал ДТ в точке калибровки"
 {0x259, 1, 0, 0, 0xAF, "countK", 0, DC_Permanent, 0, VLR_0, MT_RAM_ROM, DmC_No, F_D4, DT_Float, CountKoefForRange, 0},// команда: "вычислить коэфициент для диапазона"
 
 {0x224, 5, 0, &params.TrangeD, 0xAF, "TrangeD", sUmC, DC_Permanent, &def.TrangeD, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtFloat, RdFloat},//Нижнее значение диапазона температуры
 {0x225, 5, 0, &params.TrangeU, 0xAF, "TrangeU", sUmC, DC_Permanent, &def.TrangeU, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtFloat, RdFloat},//Верхнее значение диапазона температуры
 {0x160, 14, 0, &params.TaverI, 0xAA, "TaverI", sUmC, DC_MessW, &def.TaverI, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, 0, RdFloat},//Среднее значение температуры за интервал
 {0x160, 15, 0, &params.TaverD, 0xAA, "TaverD", sUmC, DC_MessW, &def.TaverD, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, 0, RdFloat},//Среднее значение температуры текущие сутки
 {0x160, 16, 0, &params.TaverM, 0xAA, "TaverM", sUmC, DC_MessW, &def.TaverM, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, 0, RdFloat},//Среднее значение температуры текущего месяца
 {0x160, 17, 0, &params.TmaxD, 0xAA, "TmaxD", sUmC, DC_MessW, &def.TmaxD, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, 0, RdFloat},//Максимальная температура за текущие сутки
 {0x160, 18, 0, &params.TmaxM, 0xAA, "TmaxM", sUmC, DC_MessW, &def.TmaxM, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, 0, RdFloat},//Максимальная температура за текущий месяц
 {0x160, 19, 0, &params.TminD, 0xAA, "TminD", sUmC, DC_MessW, &def.TminD, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, 0, RdFloat},//Минимальная температура за текущие сутки
 {0x160, 20, 0, &params.TminM, 0xAA, "TminM", sUmC, DC_MessW, &def.TminM, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, 0, RdFloat},//Минимальная температура за текущий месяц
 {0x210, 8, 1, &params.Tenvironment, 0xAA, "Tenv", sUmC, DC_MessW, &def.Tenvironment, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, 0, RdTemperature},//Температура окр. среды (на контроллере)
 {0x210, 8, 0, &params.Tenvironment, 0xAA, "Tenv", sUmK, DC_MessW, &def.Tenvironment, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, 0, RdTemperature},//Температура окр. среды (на контроллере)
 {0x228, 2, 0, &params.statusReg, 0xAA, "Sbat", 0, DC_SReg, &def.statusReg, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Ulong, 0, RdStatus},//Состояние статусного регистра
 {0x228, 3, 0, &params.statusReg, 0xAA, "Scap", 0, DC_SReg, &def.statusReg, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Ulong, 0, RdStatus},//Состояние статусного регистра
 {0x228, 4, 0, &params.statusReg, 0xAA, "Smagn", 0, DC_SReg, &def.statusReg, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Ulong, 0, RdStatus},//Состояние статусного регистра
 {0x228, 5, 0, &params.statusReg, 0xAA, "Svol", 0, DC_SReg, &def.statusReg, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Ulong, 0, RdStatus},//Состояние статусного регистра
 {0x228, 6, 0, &params.statusReg, 0xAA, "Stempr", 0, DC_SReg, &def.statusReg, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Ulong, 0, RdStatus},//Состояние статусного регистра
 {0x228, 7, 0, &params.statusReg, 0xAA, "Sgate", 0, DC_SReg, &def.statusReg, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Ulong, 0, RdStatus},//Состояние статусного регистра
 {0x170, 1, 0, 0, 0xAB, "accssV", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, stopAccess, RdAccessStatus}, //Замок поверителя: состояние / закрыть
 {0x170, 2, 0, 0, 0xAF, "accssM", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, stopAccess, RdAccessStatus}, //Замок изготовителя: состояние / закрыть
 {0x171, 2, 0, 0, 0x0F, "accssM", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, openAccessManuf, 0}, //Замок изготовителя: ввод / изменение
 {0x170, 3, 0, 0, 0xBF, "accssS", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, stopAccess, RdAccessStatus}, //Замок поставщика: состояние / закрыть
 {0x171, 3, 0, &params.passwS, 0x0F, "accssS", 0, DC_Function, &def.passwS, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, openAccessChangePass, RdPassw}, //Замок поставщика: ввод / изменение
 {0x170, 4, 0, 0, 0xEF, "accssU", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, stopAccess, RdAccessStatus}, //Замок потребителя: состояние / закрыть
 {0x171, 4, 0, &params.passwU, 0x0F, "accssU", 0, DC_Function, &def.passwU, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, openAccessChangePass, RdPassw}, //Замок потребителя: ввод / изменение
 {0x77C, 2, 0, &params.Smodem, 0xAA, "Smodem", 0, DC_Function, &def.Smodem, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Uchar, 0, RdUint}, //Состояние модема
 {0x777, 2, 0, &params.GSMsgnl, 0xAA, "GSMsgnl", sUmM3_Perc, DC_MessW, &def.GSMsgnl, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, 0, RdUint}, //Уровень GSM-сети
 {0x77C, 1, 0, &params.Sopt, 0xFF, "Sopt", 0, DC_Function, &def.Sopt, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Uchar, 0, RdUint}, //Состояние оптического интерфейса
 {0x778, 1, 0, &params.sendErrLis200, 0xBF, "sErLis", 0, DC_Function, &def.sendErrLis200, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Uchar, WrtUint, RdUint}, // Отправлять строку по LIS200 даже если CRC ERROR 
 {0xD21, 1, 0, &params.Apn, 0xBF, "Apn", 0, DC_Permanent, &def.Apn, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array24, WrtString, RdString}, //APN
 {0xD22, 1, 0, &params.LoginIn, 0xBF, "LoginIn", 0, DC_Permanent, &def.LoginIn, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array24, WrtString, RdString}, //Логин выхода в интернет
 {0xD23, 1, 0, &params.PasswIn, 0xBF, "PasswIn", 0, DC_Permanent, &def.PasswIn, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array24, WrtString, RdString}, //Пароль выхода в интернет
 {0xD27, 1, 0, &params.IP, 0xBF, "IP", 0, DC_Permanent, &def.IP, VLR_IP, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array16, WrtString, RdString}, //IP-адрес сервера
 {0xD28, 1, 0, &params.PortS, 0xBF, "PortS", 0, DC_Permanent, &def.PortS, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint}, //Порт сервера
 {0xD29, 1, 0, &params.LoginS, 0xBF, "LoginS", 0, DC_Permanent, &def.LoginS, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array24, WrtString, RdString}, //Логин сервера
 {0xD2A, 1, 0, &params.PasswS, 0xBF, "PasswS", 0, DC_Permanent, &def.PasswS, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array24, WrtString, RdString}, //Пароль сервера
 {0x772, 2, 0, &params.Npin, 0xBF, "Npin", 0, DC_Permanent, &def.Npin, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Номер PIN
 {0x77A, 2, 0, &params.Spin, 0xAA, "Spin", 0, DC_Permanent, &def.Spin, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Uchar, WrtUint, RdUint},//Статус PIN
 {0x150, 19, 1, &params.TservC, 0xBF, "TservC", sUmC, DC_Permanent, &def.TservC, VLR_0, MT_RAM_ROM, DmC_No, F_D2, DT_Float, WrtTemperature, RdTemperature},//Нижняя граница температуры для подключения к серверу
 {0x158, 5, 0, &params.TclsGSM, 0xBF, "TclsGSM", sUmSec, DC_Permanent, &def.TclsGSM, VLR_20_60, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Время до автоматического закрытия GPRS-сеанса при отсутствии команд
 {0x159, 5, 0, &params.TGSMmax, 0xBF, "TGSMmax", sUmSec, DC_Permanent, &def.TGSMmax, VLR_0_900, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Максимальное время обмена по GPRS
 {0x150, 6, 0, &params.TclsOpt, 0xBF, "TclsOpt", sUmSec, DC_Permanent, &def.TclsOpt, VLR_20_120, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Время до автоматического закрытия оптического интерфейса при отсутствии команд
 {0x150, 13, 0, &params.TsyncA, 0xBF, "TsyncA", 0, DC_Permanent, &def.TsyncA, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Uchar, WrtUint, RdUint},//Режим дистанционной синхронизации времени с сервером (вкл/выкл)
 {0x150, 5, 0, &params.TimSCon, 0xBF, "TimSCon", 0, DC_Permanent, &def.TimSCon, VLR_0, MT_RAM_ROM, DmC_No, F_Ascii, DT_Array12, WrtPeriodTime, RdString},//Время начала передачи данных на сервер
 {0x150, 29, 0, &params.beginTimeDataFTP, 0xBF, "TimSCon", 0, DC_Permanent, &def.beginTimeDataFTP, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_TCLOCK, WrtFTPdataTime, RdFTPdateTime},//время начала данных для передачи данных на сервер
 {0x229, 7, 0, &params.GatePas, 0x1F, "GatePas", 0, DC_Permanent, &def.GatePas, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, 0},//Пароль для открытия клапана
 {0x230, 7, 0, 0, 0x15, "GateC_O", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, F_Bit1, DT_Uchar, GateControl, 0},//Команда открытия\закрытия клапана (0 - закрыть, 1 - открыть)
 //типы шаблона архива для совместимости со старыми версиями (agr файлы)
 {0xA32, 1, 0, &params.arcType1, 0xAF, "ArcTyp1", 0, DC_Const, &def.arcType1, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 {0xA32, 3, 0, &params.arcType3, 0xAF, "ArcTyp3", 0, DC_Const, &def.arcType3, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 {0xA32, 4, 0, &params.arcType4, 0xAF, "ArcTyp4", 0, DC_Const, &def.arcType4, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 {0xA32, 5, 0, &params.arcType5, 0xAF, "ArcTyp5", 0, DC_Const, &def.arcType5, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 {0xA32, 7, 0, &params.arcType7, 0xAF, "ArcTyp7", 0, DC_Const, &def.arcType7, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 
 {0x01B, 1, 0, 0, 0xAF, "Arcs", 0, DC_Const, 0, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdArchivesString},
 
 {0xB03, 1, 0, &params.arcType_3, 0xAF, "ArcTyp3", 0, DC_Const, &def.arcType_3, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 {0xB03, 3, 0, &params.arcType_3, 0xAF, "ArcTyp3", 0, DC_Const, &def.arcType_3, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 {0xB03, 4, 0, &params.arcType_3, 0xAF, "ArcTyp3", 0, DC_Const, &def.arcType_3, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 {0xB03, 5, 0, &params.arcType_3, 0xAF, "ArcTyp3", 0, DC_Const, &def.arcType_3, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 {0xB03, 7, 0, &params.arcType_3, 0xAF, "ArcTyp3", 0, DC_Const, &def.arcType_3, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUint},
 //
 {0xA60, 1, 0, 0, 0xBF, "TrigEv", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, WrtValueForTrigger, RdTrigger},//функции записи чтения треггеров событий
 {0x111, 1, 0, 0, 0xBF, "Statuses", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, 0, RdStatuses},//функция чтения статуса событий
 
 {0x170, 5, 0, &params.LockPr, 0xAA, "accssP", 0, DC_Permanent, &def.LockPr, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, 0, RdAccessProgUpdateStatus}, //замок обновления прошива
 {0x171, 5, 0, &params.PassPr, 0x55, "PassPr", 0, DC_Function, &def.PassPr, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, WritePassProgUpdate, 0}, //комбинация обновления прошива 
 {0x196, 2, 0, &params.RegPr, 0xAA, "RegPr", 0, DC_Permanent, &def.RegPr, VLR_0, MT_RAM_ROM, DmC_No, F_D0, DT_Ushort, 0, RdUintHex}, //регистр статуса обновления прошива 
 {0x195, 1, 0, 0, 0x55, "StartPr", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, StartProgUpdate, 0}, //команда запуска обновления прошива
 
 {0xA52, 1, 0, 0, 0x05, "ResArch", 0, DC_Function, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, CmdResetArchive, 0}, //сброс архива 
 {0x1E8, 5, 0, &params.timeOutForOpticIntOff, 0xBF, "tOutOpt", sUmMlC, DC_Permanent, &def.timeOutForOpticIntOff, VLR_20000_600000, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Время таймаута для отключения оптического интерфейса
 {0x1E9, 5, 0, &params.timeOutForSwitch300bod, 0xBF, "tOut300", sUmMlC, DC_Permanent, &def.timeOutForSwitch300bod, VLR_20000_600000, MT_RAM_ROM, DmC_No, F_D0, DT_Ulong, WrtUint, RdUint},//Время таймаута для переключения на скорость 300
  // формальные параметры нужны только для установки соединения
 {0x310, 7, 0, 0, 0xBF, "test", sUmM3_PA, DC_Permanent, 0, VLR_0, MT_RAM_ROM, DmC_No, 0, 0, 0, RdStatuses},//функция чтения статуса событий
};

/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция возвращает индекс параметра в массиве описательных структур
*/
int GetIndex (unsigned adress, unsigned char instans, unsigned char mod)
{
    int i;
    int arraySize = sizeof(DataTable) / sizeof(StrDescribedParams);

    for (i=0; i < arraySize ; i++)
        if ((DataTable[i].TypCode == adress) && (DataTable[i].Instans == instans) &&
                (DataTable[i].Spec == mod))
        {
            return (i) ;
        }

    return (-1);
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция определяет является ли адрес для работы с триггерами и если да, то сохраняет
адрес запроса и выдает индекс универсального параметра для всех триггеров, всех архивов.
Функции чтения и записи универсального параметра исходя из адреса реализуют обработку запроса.
*/
int GetIndexIfAdressForTriggerEvents (unsigned adress, unsigned char instans)
{
    if(((adress>=0xa60) && (adress<=0xa69)) &&
            ((instans==1) || (instans==3) || (instans==4) || (instans==5) || (instans==7)))
    {
        // индекс универсального параметра для всех триггеров всех архивов
        return GetIndex (0xA60, 1, 0);
    }

    return (-1);
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция определяет является ли адрес для работы со статусами и если да, то сохраняет
адрес запроса и выдает индекс универсального параметра для всех статусов.
Функция чтения универсального параметра исходя из адреса реализуют обработку запроса.
*/
int GetIndexIfAdressForStatuses (unsigned adress, unsigned char instans)
{
    if(((adress == 0x111) || (adress == 0x110) || (adress == 0x100) || (adress == 0x101)) &&
            ((instans > 0) && (instans<10)))
    {
        // индекс универсального параметра для всех триггеров всех архивов
        return GetIndex (0x111, 1, 0);
    }

    return (-1);
}
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция определяет индекс параметра при команде сброса какого-то из архивов.
*/
int GetIndexIfAdressForResetArchive (unsigned adress, unsigned char instans)
{
    if(adress == 0xA52)
    {
        switch(instans)
        {
            case 1:
            case 3:
            case 4:
            case 5:
            case 7:     
            instansLis200 = instans;
            // индекс универсального параметра для всех триггеров всех архивов
            return GetIndex (0xA52, 1, 0);            
            break;
        }
    }
    return (-1);
}
//===============================================================================================================================================================
/**
 \ingroup mdParams
 \author Shuvalov Sergey
 \brief Функция определяет является ли адрес для параметров калибровки если да, то сохраняет
адрес запроса и выдает универсальный индекс.
Функция чтения\записи универсального параметра исходя из адреса реализуют обработку запроса.
*/
int GetIndexIfAdressForCalib (unsigned adress, unsigned char instans)
{
    // эталонное значение температуры в точках калибровки
    if((adress == 0x260) && ((instans > 0) && (instans <= SUM_CALIB_POINTS)))
    {
        return GetIndex (0x260, 1, 0);
    }
    // измеренное значение температуры в точках калибровки
    if((adress == 0x227) && ((instans > 0) && (instans <= SUM_CALIB_POINTS)))
    {
        return GetIndex (0x227, 1, 0);
    }
    //Коэффициент 1 для уравнения температуры
    if(((adress == 0x280) || (adress == 0x281)) && ((instans > 0) && (instans <= SUM_CALIB_PERIODS)))
    {
        return GetIndex (0x280, 1, 0);
    }
    // команда: "зафиксировать сигнал ДТ в точке калибровки"
    if((adress == 0x262) && ((instans > 0) && (instans <= SUM_CALIB_POINTS)))
    {
        return GetIndex (0x262, 1, 0);
    }
    // команда: "вычислить коэфициент для диапазона"
    if((adress == 0x259) && ((instans > 0) && (instans <= SUM_CALIB_PERIODS)))
    {
        return GetIndex (0x259, 1, 0);
    }
    return (-1);
}