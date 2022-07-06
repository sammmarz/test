#ifndef DataForFlash
#define DataForFlash

#include <stdint.h>
#include <time.h>

enum {INTERVAL_HOUR = 0, INTERVAL_DAY, INTERVAL_MONTH};
enum {BLOCK_4KB = (1<<0), CRC_END_BLOCK = (1<<1), CONTROL_CRC = (1<<2)};

//информация о записываемых данных
typedef struct
{
    int adress; //адрес флеш-памяти для текущей записи
    int adressFlashBegin; //адрес начала блока флеш-памяти для этого типа данных
    int adressFlashEnd;  //адрес конца блока флеш-памяти для этого типа данных при переполнении переход на adressFlashBegin
    uint16_t flags;
    uint32_t size; // для 4 КБ
} StrFlashInfoData;

typedef struct
{
    uint8_t numVerMetr[10];
    uint8_t numVerMain[10];
    uint8_t verDevice[10];
    uint16_t verBoard;
    uint16_t crcMetr;
    uint16_t crcMain;
    uint32_t sizeMetr;
    uint32_t sizeMain;
    uint32_t allSize;
    uint8_t reserv[6];
    uint16_t crc16;    
} StrBufProgram;

typedef struct
{
    uint32_t count;
    uint32_t globCount;
    time_t timeDate;
    double Vc;   //стандартный объем;
    double VcO; //стандартный объем, общий;
    double Vp;  //рабочий объем;
    double VpO; //рабочий объем, общий;
    double averageTemper; //среднюю температуру газа за интервал
    double PpodAvI; //среднее подстановочное значение давления за интервал
    double KaverI; //среднее подстановочное значение коэффициента сжимаемости за интервал
    double averageKkor;   //средний коэффициент коррекции за интервал
    double QcAver;//Среднее значение  стандартного расхода газа за интервал (без учета циклов измерений с 0-м расходом)
    uint16_t sysSt;
    uint16_t st1;
    uint16_t st2;
    uint16_t st3;
    uint16_t st4;
    uint16_t st5;
    uint16_t st6;
    uint16_t st7;
    uint16_t st8;
    uint16_t st9;
    uint16_t eventCode;
    uint32_t cs; //контрольная сумма
} StrIntervalArchive;

//интервальный архив для выставки на основе ТС
/*typedef struct
{
    uint32_t count;
    uint32_t globCount;
    time_t timeDate;
    double Vc;   //стандартный объем;
    double VcO; //стандартный объем, общий;
    double Vp;  //рабочий объем;
    double VpO; //рабочий объем, общий;
    double averageTemper; //среднюю температуру газа за интервал
    double PpodAvI; //среднее подстановочное значение давления за интервал
    double averageKkor;   //средний коэффициент коррекции за интервал
    uint16_t st5;
    uint16_t st6;
    uint16_t sysSt;
    uint16_t eventCode;
    uint16_t cs; //контрольная сумма
} StrIntervalArchive;
*/

typedef struct
{
    uint32_t count;
    uint32_t globCount;
    time_t timeDate;
    double Vc;   //стандартный объем;
    double VcO; //стандартный объем, общий;
    double Vp;  //рабочий объем;
    double VpO; //рабочий объем, общий;
    double averageTemper; //среднюю температуру газа за интервал
    double PpodAvI; //среднее подстановочное значение давления за интервал
    double KaverI; //среднее подстановочное значение коэффициента сжимаемости за интервал
    double averageKkor;   //средний коэффициент коррекции за интервал
    double QcAver;//Среднее значение  стандартного расхода газа за интервал (без учета циклов измерений с 0-м расходом)
    uint16_t sysSt;
    uint16_t st1;
    uint16_t st2;
    uint16_t st3;
    uint16_t st4;
    uint16_t st5;
    uint16_t st6;
    uint16_t st7;
    uint16_t st8;
    uint16_t st9;
    uint16_t eventCode;
    uint32_t cs; //контрольная сумма
} StrMonthArchive;

typedef struct
{
    uint32_t count;
    uint32_t globCount;
    time_t timeDate;
    double Vc;   //стандартный объем;
    double VcO; //стандартный объем, общий;
    double Vp;  //рабочий объем;
    double VpO; //рабочий объем, общий;
    double averageTemper; //среднюю температуру газа за интервал
    double PpodAvI; //среднее подстановочное значение давления за интервал
    double KaverI; //среднее подстановочное значение коэффициента сжимаемости за интервал
    double averageKkor;   //средний коэффициент коррекции за интервал
    double QcAver;//Среднее значение  стандартного расхода газа за интервал (без учета циклов измерений с 0-м расходом)
    uint16_t sysSt;
    uint16_t st1;
    uint16_t st2;
    uint16_t st3;
    uint16_t st4;
    uint16_t st5;
    uint16_t st6;
    uint16_t st7;
    uint16_t st8;
    uint16_t st9;
    uint16_t eventCode;
    uint32_t cs; //контрольная сумма
} StrDayArchive;

typedef struct
{
    uint32_t count;
    uint32_t globCount;
    time_t timeDate;
    uint8_t paramCode[10]; // адрес параметра
    uint8_t oldParamValue[64]; //параметр мог быть строкой, поэтому массив байт
    uint8_t newParamValue[64];
    uint8_t stCalibLock;   //Статус калибровочного замка
    uint8_t stSupplierLock;   //Статус замка поставщика
    uint8_t stConsumerLock;   //Статус замка потребителя
    uint32_t cs; //контрольная сумма
} StrChangesArchive;

typedef struct
{
    uint32_t count;
    uint32_t globCount;
    time_t timeDate;
    uint16_t eventCode; // код события
    double Vc;   //стандартный объем;
    double VcO; //стандартный объем, общий;
    double Vp;  //рабочий объем;
    double VpO; //рабочий объем, общий;
    double PpodAvI; //среднее подстановочное значение давления за месяц
    double KaverI; //среднее подстановочное значение коэффициента сжимаемости за месяц
    double averageKkor;   //средний коэффициент коррекции за месяц
    double T; // Мгновенное значение температуры газа на момент записи в архив событий
    double Q; // Мгновенное значения расхода газа на момент записи в архив событий

    uint32_t cs; //контрольная сумма
} StrEventsArchive;

typedef struct
{
    uint32_t count;
    uint32_t globCount;
    time_t timeDate;
    double Ttek;        //Измеренная температура °C	-50...100	Цикл измерения
    double T;           // Ttek или T подстановочная (при нарушении границ тревоги)//Температура газа °C	-50...100	Цикл измерения
    double Ti;          //Среднее значение температуры за час	°C	-50...100	Цикл измерения
    // чч:мм:сс 00:00:20 - 01:00:00
    // чч:мм:сс 00:00:05 - 01:00:19
    double TTC;         //Среднее значение температуры за текущие сутки	°C	-50...100	Цикл измерения/ интервал архивирования
    //чч - начало газовых суток    чч:мм:сс 10:00:20 - 10:00:00
    double TTM;         //Среднее значение температуры текущего месяца	°C	-50...100	Цикл измерения/ раз в сутки
    double TTMmax;      //Максимальная температура в текущем месяце	°C	-50...100	Цикл измерения
    double TTMmin;      //Минимальная температура в текущем месяце	°C	-50...100	Цикл измерения
    // если самое начало интервала\месяца то брать значения из предыдущего месяца или только 1-2 значения текущего?   ! брать первое значение

    double VpCM;        // Рабочий объем за период цикла измерений
    double Q1tek;       /// Измеренный рабочий расход VpCM/T (20сек  / 3600)	м3/ч
    double Qp;          /// Рабочий расход газа Q1tek или подстановочное значение м3/ч

    double Vp;          // Рабочий объем общий м3	0 - 1999999,9999
    double VpB;         // Возмущенный рабочий объем	м3
    double VpO;         // Vp + VpB Общий рабочий объем	м3

    double VcCM;        // стандартный объём цикла измерений
    double VcBCM;       // возмущённый стандартный объём цикла измерений
    double Vc;          // Счетчик стандартного объема	м3	0 - 1999999,9999
    double Qc;          // Стандартный расход газа	м3/ч	0-1000	Цикл измерения

    double VcB;         // ! = Qподстановочное * Kkor * T (по 20 сек.) или =Vp * P*Тс/Pc*Tпод*K //Счетчик возмущенного стандартного объема	м3	0 - 1999999,9999	Цикл измерения
    double VcO;         // Vc+VcB //Общий стандартный объем	м3	0 - 1999999,9999	Цикл измерения

    double VcI;         //Общий Стандартный объём за час	м3	0 - 1000	Цикл измерения
    double VcTC;        //Общий Стандартный объём за текущие сутки	м3	0 - 1999999,9999	Цикл измерения
    double VcTM;        //Общий Стандартный объём за текущий месяц	м3	0 - 1999999,9999	Цикл измерения
    double VcOI;        //Общий Стандартный объём за предыдущий час	м3	0 - 1000	Интевал архивирования
    double VcOC;        //Общий Стандартный объём за предыдущие сутки	м3	0 - 1999999,9999	Раз в сутки
    double VcOM;        //Общий Стандартный объём за предыдущий месяц	м3	0 - 1999999,9999	Раз в месяц

    double VpI;         //Общий рабочий объём за текущий час	м3	0 - 1000	Цикл измерения
    double VpTC;        //Общий Рабочий объём за текущие сутки	м3	0 - 1999999,9999	Цикл измерения
    double VpTM;        //Общий Рабочий объём за текущий месяц	м3	0 - 1999999,9999	Цикл измерения
    double VpOI;        //Общий Рабочий объём за предыдущий час	м3	0 - 1000	Интевал архивирования
    double VpOC;        //Общий Рабочий объём за предыдущие сутки	м3	0 - 1999999,9999	Раз в сутки
    double VpOM;        //Общий Рабочий объём за предыдущий месяц	м3	0 - 1999999,9999	Раз в месяц

    double Kkor;       // P*Тс/Pc*T*K //Коэффициент коррекции – Цикл измерения

    uint32_t alerts;    // действующие тревоги
    uint32_t alertsReg; // сюда добавлются все тревоги как и в status, сбрасываются принудительно пользователем
    uint32_t cs; //контрольная сумма
} StrPeriodData;

extern uint32_t globCountFlash;
extern StrChangesArchive changesArchive;
extern StrIntervalArchive intervalArchive;
extern StrMonthArchive monthArchive;
extern StrDayArchive dayArchive;
extern StrEventsArchive eventsArchive;
extern StrPeriodData periodData;

extern StrFlashInfoData changesArchiveInfo;
extern StrFlashInfoData intervalArchiveInfo;
extern StrFlashInfoData eventsArchiveInfo;
extern StrFlashInfoData periodDataInfo;
extern StrFlashInfoData monthArchiveInfo;
extern StrFlashInfoData dayArchiveInfo;
extern StrFlashInfoData paramsInfo;

extern uint8_t bufProgram[256];
extern uint32_t indxBufProgram;
extern StrFlashInfoData bufProgramInfo;

void initInfoStructDataForFlash(void);
#endif