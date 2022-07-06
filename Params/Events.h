#ifndef Events_h
#define Events_h
#include <stdint.h>
#include <stdbool.h>

#define SUM_STATUS_EVENTS 10
enum {SYS_ST, ST_1, ST_2, ST_3, ST_4, ST_5, ST_6, ST_7, ST_8, ST_9};
enum
{
    //вне системы
    EV_NEW_INTERVAL_ARC = 0x8104,
    //по системе (протокол передачи данных)
    EV_BORDER_ALERT_QP = 0x2004,
    EV_ERROR_KKOR = 0x2005,
    EV_BORDER_ALERT_T = 0x2006,
    EV_ERROR_HALL1 = 0x2101,
    EV_ERROR_REPROGRAMM = 0x2601,
    EV_ERROR_HALL2 = 0x2102,
    EV_SIGNAL_T = 0x2105,
    //EV_BORDER_WARN_QC = 0x2502,
    EV_BORDER_WARN_QP = 0x2504,
    EV_BORDER_WARN_T_GAS = 0x2506,
    EV_BORDER_WARN_T_ENV = 0x2803,
    EV_MAGNIT = 0x2C06,
    EV_BAT_OPEN = 0x2C07,
    EV_BAT1_LOW = 0x3802,
    EV_BAT2_LOW = 0x3302,
    EV_BOX_OPEN = 0x2C08,
    EV_VALVE_CLOSE = 0x3502,
    EV_LOCK_CALIB_OPEN = 0x2D01,
    EV_LOCK_MANUF_OPEN = 0x2D02,
    EV_LOCK_SUPL_OPEN = 0x2D03,
    EV_LOCK_CONS_OPEN = 0x2D04,
    EV_OPT_READY = 0x2F01,
    EV_MODEM_START = 0x2F02,
    EV_OPT_DATA_START = 0x2E08,
    EV_TIMER_NOT_SET = 0x3A02,
    EV_RESET = 0x3002,
    EV_REPROGRAMM = 0x3102,
    EV_DATA_UPDATE = 0x3202
};
/*
Каждый член - это отдельный статус. Каждый бит отдельное событие
*/
typedef struct
{
    uint16_t events[SUM_STATUS_EVENTS];
    uint16_t trigger; // здесь храним значения событий для конкретного триггера
} StrEvents;

extern uint16_t adressLis200;
extern uint16_t instansLis200;
//
extern uint16_t adressLis200;
extern uint16_t instansLis200;

bool CheckEventInRegisterStatus(uint16_t event);
bool CheckEventInStatus(uint16_t event);
int setEvent(uint16_t event);
int resetEvent(uint16_t event);
void controlBoxOpen(int countForSignal);
void controlBatBoxOpen(int countForSignal);
void controlMagnitActivity(int countForSignal);
#endif