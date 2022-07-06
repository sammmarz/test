#ifndef mdRTC
#define mdRTC

#include "mdHardware/mdHardware.h"
#include "mdFlashMemory/mdFlashMemory.h"
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

/*
32768 / 30 Гц = 1092;  т.к. делитель 2 PERIOD_WKUP_TIMER 546
*/
#define PERIOD_WKUP_TIMER 546

void InitHSIClock(void);
void initRTC(StrCorrectorParams *strCorParams);
ErrorStatus setRtcDateTime(struct tm *dateTime);
void getRtcDateTime(struct tm *dateTime);
double getTimeWithSubSecAfterPoint(void);
void initWatchdog(void);

#endif
