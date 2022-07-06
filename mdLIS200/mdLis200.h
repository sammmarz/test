#ifndef lis200_h
#define lis200_h

#include <stdbool.h>
#include <stdint.h>

#define BUF_UART_MAX 1024
#define Acc_Read	0x01
#define Acc_Write  	0x02

enum eCharsCode
{
    SOH = 1,
    STX = 2,
    ETX = 3,
    EOT = 4,
    ACK = 6,
    NAK = 0x15
};
enum eErrors
{
    EC_Restore = 1,
    EC_AbsentValue = 4,
    EC_WrongFArgument = 5,
    EC_ROValue = 6,
    EC_Measurement = 7,
    EC_WrongChr = 8,
    EC_MessW = 9,	//? - EK260 returns if write to measured value
    EC_Lock0 = 11,
    EC_Lock1 = 12,
    EC_Lock2 = 13,
    EC_Lock3 = 14,

    EC_17 = 17,	//? - EK260 returns if incorrect lock value
    EC_18 = 18,  // Нет прав на чтение
    EC_19 = 19,  // Нет прав на запись
    EC_20 = 20,  // Action is prohibited

    EC_WrongValueFromSet = 101,
    EC_WrongValueRange = 102
};
//*****************************
//секция Ошибки коммуникации
//*****************************
enum eErrors2
{
    Err_Telegramma  = 1, //ошибка формата телеграммы
    Err_Address     = 2, //адрес параметра не существует
    Err_Attrib      = 3, //несуществующий номер аттрибута параметра
    Err_Modif       = 4, //несуществующий модификатор параметра
    Err_Value       = 5, //недопустимое значение параметра
    Err_Range       = 6, //устанавливаемое значение вне допустимого диапазона
    Err_Const       = 7, //устанавливаемое значение константа
    Err_AcCode      = 8, //неверный код доступа
    Err_AcRight     = 9, //отсутствуют права на выполнение операции
    Err_ArcEmpty    = 10,//архив пуст
    Err_Data        = 11,//ошибка при обращении к данным
    Err_EepRom      = 12,//ошибка при обращении к EEPROM
    Err_BCC         = 13,//ошибка при проверке контрольной суммы телеграммы
    Err_FmtFunc     = 14,//у параметра нет функции форматированиЯ ответа
    Err_WrtFunc     = 15//у параметра нет функции записи значениЯ
};

typedef struct
{
    char bufRX[BUF_UART_MAX];
    char bufTX[BUF_UART_MAX];
    int indx;  //индекс для bufRX
    int BCCisZERO; // флаг, что BCC ровно 0 и нужно передать еще байт
} StrDataUART;

extern StrDataUART uartData;

typedef struct
{
    int num; // номер запрашиваемого архива
    bool mode; // true - передача архива false - передача параметра
    uint8_t typeSelection;   // тип выборки данных (время \ счётчик \ глобальный счётчик)
    uint32_t adrBegin; // начальная страница адреса во флеш запрашиваемых данных
    uint32_t adrEnd;   // конечная страница адреса во флеш запрашиваемых данных
    uint32_t adressFlashBegin; // начало выделенной памяти для архива
    uint32_t adressFlashEnd;    // конец выделенной памяти для архива
    uint8_t size; // размер структуры архива
    uint16_t sizeCRC; // размер части архива, учавствующей в подсчете КС
    uint16_t sumPages; // сумма страниц во флеш запрашиваемых данных
    uint16_t curPage; // текущая страница во флеш запрашиваемых данных, которую нужно передать
    uint16_t curRecInBlock; // текущая передаваемая запись в блоке
    uint16_t sumPagesBlock; // сумма страниц в блоке данных
    uint8_t bccBlock;   // контрольная сумма блока данных
} StrArchive;

extern StrArchive archive;

void clearRXBuf(StrDataUART *data);
void clearTXBuf(StrDataUART *data);
void addByteInBufRX(char ch, StrDataUART *data);
bool analyseReceivedByteUARTasLis200(char ch, StrDataUART *dataUART);
#endif