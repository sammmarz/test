#ifndef driver_GL865_h
#define driver_GL865_h

#include "../mdHardware/mdHardware.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "../mdLIS200/mdLis200.h"                                               // for StrArchive

void GL865_initPins();
void GL865_powerOn();
void GL865_powerOff();
void GL865_reset();
void SkipRecieveBuff();

int GL865_check();
bool GL865_GprsEnable();
void GL865_GprsStop();
bool GL865_SwitchToATMode();
int GL865_FtpClient(long *bytesSent);
int GetRecSum(StrArchive *archiveModem, time_t beg, time_t end);
int ReadArchiveModem(StrArchive *archiveModem, unsigned char *bufTX, int *count);

void createIntervalArchiveString(char *bufTX, StrIntervalArchive *archive, int *count);
void createDayArchiveString(char *bufTX, StrDayArchive *archive, int *count);
void createMonthArchiveString(char *bufTX, StrMonthArchive *archive, int *count);
void createChangesArchiveString(char *bufTX, StrChangesArchive *archive, int *count);
void createEventsArchiveString(char *bufTX, StrEventsArchive *archive, int *count);

bool WaitATAnswer(char *rightAns, portTickType waitForAnswer);
bool RecieveATAnswer(portTickType waitForAnswer);

#define SendATCommand(AT_COMM) sendDataUsart3((uint8_t *)AT_COMM, sizeof(AT_COMM) - 1);
#define WaitOKAnswer(WAIT) WaitATAnswer("OK", WAIT)
#define BYTE_TIMEOUT 100

enum GPRS_resultCodes
{
    GPRS_start1 = 1,                                                              // 1 try
    //GPRS_start = 2,                                                             // 2 try
    //GPRS_start = 3,                                                             // 3 try
    GPRS_stoppedByUser = 10,
    GPRS_noCTS = 21,
    GPRS_ATcommandError = 31,
    GPRS_noPDP = 32,                                                              // AT+CGDCONT error
    GPRS_noSGACT = 33,                                                            // AT#SGACT error
    GPRS_noFTP = 34                                                               // AT#FTPOPEN
};

#endif