#ifndef params_h
#define params_h

#include <stdint.h>
#include <time.h>
#include "Events.h"
#include "time64.h"

#define DEFAULT_VAL_CALIB_POINT_T 256
#define SUM_CALIB_POINTS 10
#define SUM_CALIB_PERIODS 9
#define ALL_BAT_VOL 15000000
#define TOUR_BAT 96 /* (8 лет * 12 мес) */
#define SUM_TRIGGERS 10
// for time_t
//=================================================================================================
enum eDataClass
{
    DC_FunctS    =1, // Тест дисплея Изменения невозможны.
    DC_Function  =2, // Функции Возможна активация функции.
    DC_Const     =3, // Константы Изменения невозможны.
    DC_MessW     =4, // Измеренные значения Значения обновляются после продолжительного нажатия клавиши.
    DC_Status    =5, // Статус Значения мгновенного статуса. Изменения невозможны.
    DC_Discret   =7, // Дискретные значения Возможно изменение в пределах списка допустимых значений.
    DC_Permanent =8, // Постоянные значения Изменения возможны только через интерфейс.
    DC_Combin    =11, // Комбинации/ Возможно изменение с клавиатуры
    DC_Counter   =12, // Счётчики Изменения возможны только через интерфейс.
    DC_CompCount =15, // Счётчик потребления Изменения невозможны.
    DC_InitVal   =16, // Начальные значения Изменения невозможны.
    DC_SReg      =19, // Значения архива. Изменения невозможны.
    DC_Marker    =20  // Регистр статуса Возможен сброс значений в ноль посредством активации соответствующей функции.
};
//=================================================================================================
enum eValueRange
{
    VLR_0 = 0,
    VLR_0_23 = 1,
    VLR_5_3600_5 = 3,
    VLR_20_300 = 4,
    VLR_Cels = 5,
    VLR_Kelv = 6,
    VLR_P = 7,
    VLR_K = 8,
    VLR_IP = 9,
    VLR_20_60 = 10,
    VLR_0_900 = 11,
    VLR_20_120 = 12,
    VLR_0_6 = 13,
    VLR_20000_600000 = 14
};
//=================================================================================================
enum eDimCode
{
    DmC_No           =0,
    DmC_Analog       =0x40,
    DmC_Recip        =0x20,
    DmC_Volume       =0x80,
    DmC_Press        =0x10
};
//=================================================================================================
enum eMemType
{
    MT_RAM = 1,
    MT_ROM = 2,
    MT_RAM_ROM = 3
};
//=================================================================================================
enum eFormat
{
    F_D0         =0,
    F_D1         =1,
    F_D2         =2,
    F_D3         =3,
    F_D4         =4,
    F_D5         =5,
    F_D6         =6,
    F_D7         =7,
    F_D00        =8,
    F_D01        =9,
    F_D02       =10,
    F_D03       =11,
    F_D04       =12,
    F_D05       =13,
    F_D06       =14,
    F_D07       =15,
    F_Dynamic   =16,
    F_Quelle    =17,
    F_Ereignis  =18,
    F_DateTime  =19,
    F_Date      =20,
    F_Time      =21,
    F_Month     =23,
    F_Day       =24,
    F_Hour      =25,
    F_Minute    =26,
    F_DayMont   =27,
    F_DayHour   =28,
    F_HourMin   =29,
    F_ErrCode   =33,
    F_BCD       =34,
    F_Ascii     =35,
    F_Bit1      =36,
    F_Bits      =37,
    F_Analog    =39,
    F_GradC     =46,
    F_Beetv     =47,
    F_PointD4   =48,
    F_WeDaTime  =50,
    F_DayTime   =51
};
//=================================================================================================
enum eDataType
{
    DT_Uchar      =2,
    DT_Ushort     =3,
    DT_Ulong      =4,
    DT_Sshort     =6,
    DT_Slong      =7,
    DT_Float      =8,
    DT_COUNT6     =9,
    DT_Array2    =12,
    DT_Array4    =14,
    DT_Array6    =16,
    DT_Array8    =17,
    DT_Array12   =19,
    DT_Array16   =20,
    DT_Array24   =22,
    DT_Array32   =24,
    DT_Cont6     =30,
    DT_TCLOCK    =31,
};
//=================================================================================================
enum eStatusReg
{
    eSbat = (1<<2),
    eScap = (1<<3),
    eSmagn = (1<<4),
    eSvol = (1<<5),
    eStempr = (1<<6),
    eSgate = (1<<7),
};
//=================================================================================================
typedef unsigned long (*ChkVal) (void *value);
typedef unsigned (*WriteFmt) (unsigned i, unsigned char *p_Buff);
typedef void (*ReadFmt) (unsigned i, unsigned char *p_Buff, void *value);
typedef struct
{
    unsigned int TypCode;        //B
    uint8_t	Instans;        //C
    uint8_t	Spec;           //D
    void *AC_Value;        //E значение
    uint8_t	AC_AccRight;    //F Права доступа
    char AC_Name[8];        	//G Название
    char *AC_DimStr;      //H Ед.изм (текст)
    uint8_t	AC_DataClass;   //I Класс данных
    void  *AC_DefValue;     //J Значений по умолчанию
    uint8_t AC_ValueRange;  //K Диапазонов значений
    uint8_t	MemType;        //L Резервная копия и Защита
    uint8_t	AC_DimCode;     //M Ед.изм - Код (аналоговый, объем, давление и т.д.)
    uint8_t	AC_Format;      //N Формат (сколько знаков после точки и тп)
    uint8_t	AC_DataType;    //O Тип данных
    WriteFmt WrtFunc;       //M	Функция определяет формат перезаписи значения параметра
    ReadFmt RdFunc;
} StrDescribedParams;

// структура хранит параметры работы корректора
typedef struct
{
    uint32_t count;
    uint32_t countGlob;
    uint32_t countIntervArc;
    uint32_t countDayArc;
    uint32_t countMonthArc;
    uint32_t countEventArc;
    uint32_t countChangesArc;   
    int adrFlashIntervArc;
    int adrFlashDayArc;
    int adrFlashMonthArc;
    int adrFlashEventArc;
    int adrFlashChangesArc;
    int adrFlashParams;
    
    uint8_t dayBegin;               // Граница дня, начало газового дня
    char identDevice[13];           // Серийный номер счётчика
    char typeDevice[17];             // тип счётчика
    uint16_t verBoard;
    char Maker[33];                 // изготовитель
    char versMain[10];               // версия основного ПО
    uint32_t crcMain;               // crc основного ПО
    char versMeter[10];              // версия метрологического ПО
    uint32_t crcMeter;              // crc метрологического ПО
    short utc;                      // UTC
    double volBat1;                  // емкость батареи
    double volBat2;                  // емкость батареи 2
    uint8_t lifeBat1;               // остаточный срок службы батареи 1
    uint8_t lifeBat2;               // остаточный срок службы батареи 2
    char typeTempDevice[9];         // тип термодатчика
    uint16_t measPeriod;            // период цикла измерений
    uint16_t lcdTimeOff;            // время перед отключением дисплея
    uint16_t lcdTimeToMain;         // время переключения дисплея на основное меню
    char envirTempRange[12];        // диапазон Т окр. среды
    uint8_t koefTimer;              // Коэффициент настройки часов
    double koefImpulseHall;         //значение объема соответствующее 1 сигналу датчика
    uint8_t maxPeriodsBetweenSignals; //максимальное количество периодов цикла измерения между сигналами датчиков Холла (max 20)
    uint8_t speedUARTcode;          // Скорость передачи по оптическому интерфейсу
    double Vc;                      // Счетчик стандартного объёма Vc (невозмущенный)
    double Vp;                      // Счетчик рабочего объёма Vр (невозмущенный)
    double VcB;                     // Счётчик возмущенного стандартного объёма Vс
    double VpB;                     // Счётчик возмущенного рабочего объёма Vр
    double VcO;                     // Общий счетчик стандартного объёма Vс
    double VpO;                     // Общий счетчик рабочего объёма Vр
    double VcTI;                    // Стандартный объём за текущий интервал архивирования
    double VcOI;                    // Стандартный объём за предыдущий интервал архивирования
    double VcTC;                    // Стандартный объём за текущие сутки
    double VcOC;                    // Стандартный объём за предыдущие сутки
    double VcTM;                    // Стандартный объём за текущий месяц
    double VcOM;                    // Стандартный объём за предыдущий месяц
    double VpTI;                    // Рабочий объём за текущий интервал архивирования
    double VpOI;                    // Рабочий объём за предыдущий интервал архивирования
    double VpTC;                    // Рабочий объём за текущие сутки
    double VpOC;                    // Рабочий объём за предыдущие сутки
    double VpTM;                    // Рабочий объём за текущий месяц
    double VpOM;                    // Рабочий объём за предыдущий месяц
    double Qc;                      // Стандартный расход Qс
    double QcAvI;                   // Стандартный расход Qс (среднее за интервал)
    double QcAvD;                   // Стандартный расход Qс (среднее за сутки)
    double QcAvM;                   // Стандартный расход Qс (среднее за месяц)
    double Qp;                      // Рабочий расход Qр
    double QTek;                    // Измеренный рабочий расход QТек
    double Kkor;                    // Коэффициент коррекции
    double KkorAvI;                 // Коэффициент коррекции(среднее за интервал)
    double KkorAvD;                 // Коэффициент коррекции(среднее за сутки)
    double KkorAvM;                 // Коэффициент коррекции(среднее за месяц)
    double T;                       // Температура газа T
    double Ttek;                    // Температура газа измеренная
    int16_t TtekBin;               // Температура газа измеренная, бинарное представление
    double Tpod;                    // подстановочное значение температуры
    uint32_t Ppod;                  // Давление газа подстановочное p
    uint32_t PpodAvI;               // Давление газа подстановочное (среднее за интервал)
    uint32_t PpodAvD;               // Давление газа подстановочное (среднее за сутки)
    uint32_t PpodAvM;               // Давление газа подстановочное (среднее за месяц)
    double QpodD;                   // Нижнее подстановочное значение расхода
    double QpodU;                   // Верхнее подстановочное значение расхода
    double K;                       // Коэффициент сжимаемости подстановочный K
    double KaverI;                  // Коэффициент сжимаемости подстановочный (среднее за интервал)
    double KaverD;                  // Коэффициент сжимаемости подстановочный (среднее за сутки)
    double KaverM;                  // Коэффициент сжимаемости подстановочный (среднее за месяц)
    double Tstand;                  // Стандартная температура Tс
    uint32_t Pstand;                // Стандартное давление pс
    double TalertU;                 // Верхняя граница тревоги темп. газа T
    double TalertD;                 // Нижняя граница тревоги темп. газа T
    double TwarnU;                  // Верхняя граница предупреждения темп. газа T
    double TwarnD;                  // Нижняя граница предупреждения темп. газа T
    double ThystU;                  // Гистерезис для максимальной температуры
    double ThystD;                  // Гистерезис для минимальной температуры
    double QalertU;                 // Верхняя граница тревоги расхода Qmax
    double QalertD;                 // Нижняя граница тревоги расхода Qmin
    double QwarnU;                  // Верхняя граница предупреждения расхода Qmax
    double QwarnD;                  // Нижняя граница предупреждения расхода Qmin
    float Tenvironment;             // Температура окр. среды (на контроллере)
    float TenvWarnU;                // Верхнее значение диапазона температуры
    float TenvWarnD;                // Нижнее значение диапазона температуры
    /*
    double Tkoef1;                  // Коэффициент 1 для уравнения температуры
    double Tkoef2;                  // Коэффициент 2 для уравнения температуры
    double Tkoef3;                  // Коэффициент 3 для уравнения температуры
    double TForK1;                  // Настроечное значение 1 для температуры
    double TForK2;                  // Настроечное значение 2 для температуры
    double TForK3;                  // Настроечное значение 3 для температуры
    */
    double TrangeD;                 // Нижнее значение диапазона температуры
    double TrangeU;                 // Верхнее значение диапазона температуры
    double TaverI;                  // Ti средняя температура за интервал
    double TaverD;                  // Среднее значение температуры текущие сутки
    double TaverM;                  // Среднее значение температуры текущего месяца
    double TmaxD;                   // Максимальная температура за текущие сутки
    double TmaxM;                   // Максимальная температура за текущий месяц
    double TminD;                   // Минимальная температура за текущие сутки
    double TminM;                   // Минимальная температура за текущий месяц
    uint32_t statusReg;             // Состояние статусного регистра
    uint32_t passwS;                // Замок поставщика
    uint32_t passwU;                // Замок потребителя
    uint8_t sendErrLis200;          // Отправлять строку по LIS200 даже если CRC ERROR
    uint8_t Smodem;                 // Состояние модема
    uint32_t GSMsgnl;               // Уровень GSM-сети
    uint8_t Sopt;                   // Состояние оптического интерфейса
    char Apn[25];                   // APN
    char LoginIn[25];               // Логин выхода в интернет
    char PasswIn[25];               // Пароль выхода в интернет
    char IP[16];                    // IP-адрес сервера
    uint32_t PortS;                 // Порт сервера
    char LoginS[25];                // Логин сервера
    char PasswS[25];                // Пароль сервера
    uint32_t Npin;                  // Номер PIN
    uint8_t Spin;                   // Статус PIN
    double TservC;                  // Нижняя граница температуры для подключения к серверу
    uint32_t TclsGSM;               // Время до автоматического закрытия GSM-сеанса при отсутствии команд
    uint32_t TGSMmax;               // Максимальное время обмена по GSM
    uint32_t TclsOpt;               // Время до автоматического закрытия оптического интерфейса при отсутствии команд
    uint8_t TsyncA;                 // Режим дистанционной синхронизации времени с сервером (вкл/выкл)
    char TimSCon[13];               // Время начала передачи данных на сервер
    time_t timeData;                // время счётчика
    time_t beginTimeDataFTP;        // начальное время архивов для передачи на сервер
    uint32_t GatePas;               // Пароль для открытия клапана
    //
    StrEvents curEvents;        // текущее состояние статусов, событий
    StrEvents regEvents;        // состояние статусов, событий, которые сбрасываются только по команде
    StrEvents arcIntervSt[SUM_TRIGGERS];  // отслеживаемые события для записи в интервальный архив 10 триггеров
    StrEvents arcDaySt[SUM_TRIGGERS];     // отслеживаемые события для записи в суточный архив 10 триггеров
    StrEvents arcMonthSt[SUM_TRIGGERS];   // отслеживаемые события для записи в месячный архив 10 триггеров
    StrEvents arcEventsSt[SUM_TRIGGERS];  // отслеживаемые события для записи в архив событий 10 триггеров
    StrEvents arcChangeSt[SUM_TRIGGERS];  // отслеживаемые события для записи в архив изменений 10 триггеров
    //
    //тип шаблона ахива для совместимости со старыми версиями (agr файлы)
    uint16_t arcType_3;
    uint16_t arcType1; 
    uint16_t arcType3; 
    uint16_t arcType4; 
    uint16_t arcType5; 
    uint16_t arcType7; 
    //
    uint32_t alerts;
    struct tm oldDateTime;
    struct tm curDateTime;
    uint8_t dateTimeFlags;
    //структуры для расчета среднего за разные периоды
    struct
    {
        double sum;
        int count;
    } aTi, aTTC, aTTM,
    aKkorAvI, aKkorAvD, aKkorAvM,
    aPpodAvI, aPpodAvD, aPpodAvM,
    aKavI, aKavD, aKavM,
    aQcAvI, aQcAvD, aQcAvM;
    //
    uint32_t counterHall_A;
    uint32_t counterHall_A_old;
    uint32_t countHallImpulse[20];
    uint8_t periodIndex;
    //
    double VpCM;        // Рабочий объем за период цикла измерений
    double VcCM;        // стандартный объём цикла измерений
    double VcBCM;
    // возмущённый стандартный объём цикла измерений
    double impulses;
    double VPCP;
    double VpCMclear;
    // program update
    uint8_t LockPr;
    uint32_t PassPr;
    uint16_t RegPr;
    //
    uint32_t timeOutForOpticIntOff;     // таймаут в млСек для отключения оптики
    uint32_t timeOutForSwitch300bod;    // таймаут в млСек для переключения оптики на скорость 300
    //
    // параметры для калибровки
    float calibPoints[10];          // эталонная температура в точке калибровки
    float calibPointsSignalTS[10];  // измеренная температура в точке калибровки
    float koefForTS[9][2];          // коэффициенты для вычисления температуры в разных диапазонах
    //
    uint16_t crc16;
} StrCorrectorParams;

int GetIndex (unsigned adress, unsigned char instans, unsigned char mod);
int GetIndexIfAdressForTriggerEvents (unsigned adress, unsigned char instans);
int GetIndexIfAdressForStatuses (unsigned adress, unsigned char instans);
int GetIndexIfAdressForResetArchive (unsigned adress, unsigned char instans);
int GetIndexIfAdressForCalib (unsigned adress, unsigned char instans);
void readRangeParam(uint16_t index, char *OutBuf);
int checkValueRange(uint8_t	AC_DataType, void *value);

extern StrDescribedParams DataTable[];
extern StrCorrectorParams def;
extern StrCorrectorParams params;

void initParamsDefault(void);
void writeParamsToFlash(void);
int initParamsFromFlash(void);

#endif // params_h
