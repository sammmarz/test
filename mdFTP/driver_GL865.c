/**
\file
Includes funcions for FTP processing via modem TELIT GL865-DUAL V3.1.
*/

/** \defgroup mdFTPGroup mdFTP
Includes funcions for FTP processing via modem TELIT GL865-DUAL V3.1.
*/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32l1xx_gpio.h"
#include "../mdHardware/mdHardware.h"                                           // for pins
#include "driver_GL865.h"                                                       // for Result codes
#include "string.h"                                                             // for memset()
#include "time.h"
#include "stdio.h"
#include <stdint.h>                                                             // For uint32
#include "mdRTC/mdRTC.h"                                                        // For getRtcDateTime
#include "../mdFlashMemory/mdFlashMemory.h"

#include "../mdFlashMemory/DataForFlash.h"

extern int GetBcc(unsigned char *p_msg);
extern char *FmtStatus(unsigned pSt, char *p_Buff, int *count);

extern bool gprsStatus;                                                         // GPRS global status
extern SemaphoreHandle_t xMutexParams;
extern const unsigned short CRCInit;

MODEM_BUF Modem_buf;                                                            // RX, TX buffers
static uint8_t bufArc[256];
unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);
unsigned char countBcc (unsigned char *p_msg);
static const char intervHeader[64] = {"\r\nMeasuring period archive from %19s to %19s\r\n"};
static const char dayHeader[64] = {"\r\nDay archive from %19s to %19s\r\n"};
static const char monthHeader[64] = {"\r\nMonthly archive from %19s to %19s\r\n"};
static const char eventsHeader[64] = {"\r\nLog book from %19s to %19s\r\n"};
static const char changesHeader[64] = {"\r\nAudit Trial archive from %19s to %19s\r\n"};


/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief TELIT GL865 DUAL V3.1. [Init pins]
*/
void GL865_initPins()
{
    initGpioForModemRESET();                                                      // Init modem RESET, LOW
    initGpioForModemPOWER();                                                      // Init modem POWER, LOW
    initGpioForModemTX();
    initGpioForModemRX();
    initGpioForModemCTS();
    initGpioForModemRTS();
    initUSART_3(115200, USART_Parity_No);                                         // 115200 8N1
    initModemInterrupt();
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief TELIT GL865 DUAL V3.1. [Power ON]
*/
void GL865_powerOn()
{
    GPIO_ResetBits(MODEM_GPIO_PORT, MODEM_RESET_PIN);                             // RESET OFF (HI)
    vTaskDelay(10);
    GPIO_SetBits(GL865_POWER_PORT, GL865_POWER_PIN);                              // POWER ON (HI)
    vTaskDelay(1000);                                                             // Start up pause
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief TELIT GL865 DUAL V3.1. [Power OFF]
*/
void GL865_powerOff()
{
    GPIO_ResetBits(GL865_POWER_PORT, GL865_POWER_PIN);                            // POWER OFF (LOW)
    //?????? SHOULD SET to IN????
    vTaskDelay(10);
    GPIO_ResetBits(MODEM_GPIO_PORT, MODEM_RESET_PIN);                             // RESET deInit (HI)
    deInitGpioForModem();                                                         // USART port set to IN for lower consumption
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief TELIT GL865 DUAL V3.1. [reset modem]
*/
void GL865_reset()
{
    GPIO_SetBits(MODEM_GPIO_PORT, MODEM_RESET_PIN);                               // RESET ON (CPU PIN HI, MODEM PIN LOW)
    vTaskDelay(250);
    GPIO_ResetBits(MODEM_GPIO_PORT, MODEM_RESET_PIN);                             // RESET OFF (CPU PIN LOW, MODEM PIN HI)
    vTaskDelay(1000);
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief TELIT GL865 DUAL V3.1. [Check modem]
*/
int GL865_check()
{
    int result = 0;                                                               // Init result code indicator
    
    for (int trying = 0; trying < 3; trying ++)
    {
        if (!gprsStatus)                                                            // Stopped by user
        {
            return GPRS_stoppedByUser;
        }
        
        if (trying == 1)                                                               // The second try - wait
        {
            vTaskDelay(2000);
        }
        
        if (trying == 2)                                                               // The third try - reset modem
        {
            GL865_reset();
        }
        
        result = 0;                                                                 // Reset result code
        // [00]
        vTaskDelay(1000);                                                           // Wait DUMMY
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("AT\r\n");                                                    // Test command, no answer is checked
        vTaskDelay(200);                                                            // Wait
        // [01]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("ATE0\r\n");                                                  // Echo off, no answer is checked
        vTaskDelay(200);                                                            // Wait
        // [0X]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("AT+CREG=0\r\n");                                             // Turn OFF registrition message
        vTaskDelay(200);                                                            // Wait
        // [02]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("AT\r\n");                                                    // Test command
        
        if (!WaitOKAnswer(2000))
        {
            result = GPRS_ATcommandError;                                             // Set result code
            continue;                                                                 // Try again, from the beginning
        }
        
        // [03]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("AT&W\r\n");                                                  // Save Settings (ATE0) to TELIT EEPROM
        
        if (!WaitOKAnswer(2000))
        {
            result = GPRS_ATcommandError;                                             // Set result code
            continue;                                                                 // Try again, from the beginning
        }
        
        // [04]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("AT+IFC=2,2\r\n");                                            // Use flow control (CTS, RTS)
        
        if (!WaitOKAnswer(200))
        {
            result = GPRS_ATcommandError;                                             // Set result code
            continue;                                                                 // Try again, from the beginning
        }
        
        // [05]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("AT+IFC=2,2\r\n");                                            // Use flow control (CTS, RTS)
        
        if (!WaitOKAnswer(200))
        {
            result = GPRS_ATcommandError;                                             // Set result code
            continue;                                                                 // Try again, from the beginning
        }
        
        // [06]
        if (!isCts_usart3())                                                        // Check CTS
        {
            result = GPRS_noCTS;                                                      // Set result
            continue;                                                                 // Try again, from the beginning
        }
        
        // [07]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("ATX0\r\n");                                                  // Set result code format
        
        if (!WaitOKAnswer(200))
        {
            result = GPRS_ATcommandError;                                             // Set result code
            continue;                                                                 // Try again, from the beginning
        }
        
        // [08]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("AT+CMEE=0\r\n");                                             // Set error format code. Return only "ERROR" string
        
        if (!WaitOKAnswer(200))
        {
            result = GPRS_ATcommandError;                                             // Set result code
            continue;                                                                 // Try again, from the beginning
        }
        
        // [09]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("AT+CIMI\r\n");                                               // SIM detection
        RecieveATAnswer(2000);                                                      // Skip IMEI number
        
        if (!WaitOKAnswer(5000))                                                    // Wait for OK
        {
            result = GPRS_ATcommandError;                                             // Set result code
            continue;                                                                 // Try again, from the beginning
        }
        
        // [07]
        //    SendATCommand("AT#FTPTO=200\r\n");                                          // Set FTP timeout
        //    if (!WaitOKAnswer(200))
        //    {
        //      result = GPRS_ATcommandError;                                             // Set result code
        //      continue;                                                                 // Try again, from the beginning
        //    }
        // [10]
        SkipRecieveBuff(Modem_buf.RX);                                              // Purge and clear bufer
        SendATCommand("AT+WS46=12\r\n");                                            // Use ONLY 2G network connection
        
        if (!WaitOKAnswer(200))
        {
            result = GPRS_ATcommandError;                                             // Set result code
            continue;                                                                 // Try again, from the beginning
        }
        
        break;                                                                      // Exit
    }
    
    return result;
    // 1. Check CTS
    //  portTickType end_of_wait = xTaskGetTickCount() + (portTickType)(5000);
    //
    //  while (true)
    //  {
    //    if (!gprsStatus)                                                            // Stopped by user
    //    {
    //      return GPRS_stoppedByUser;
    //    }
    //
    //    if (isCts_usart3())                                                         // Check CTS
    //    {                                                                           // CTS found, continue
    //      break;
    //    }
    //
    //    if (xTaskGetTickCount() >= end_of_wait)
    //    {
    //      return GPRS_noCTS;                                                        // No CTS - return error
    //    }
    //
    //    vTaskDelay((portTickType)(1000));
    //  }
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief TELIT GL865 DUAL V3.1. [Connect to GPRS service]
*/
bool GL865_GprsEnable()
{
    // Get data from device
    char apn[32];
    memset(apn, 0x00, 32);
    char login[32];
    memset(login, 0x00, 32);
    char pass[32];
    memset(pass, 0x00, 32);
    xSemaphoreTake(xMutexParams, portMAX_DELAY);                                  // Protect by semaphore
    memcpy(apn, params.Apn, strlen(params.Apn));                                // Get APN
    memcpy(login, params.LoginIn, strlen(params.LoginIn));                      // Get login internet
    memcpy(pass, params.PasswIn, strlen(params.PasswIn));                       // Get  pass internet
    xSemaphoreGive(xMutexParams);
    int result = 0;
    vTaskDelay(2000);
    // [01] Deactivate PDP context
    SkipRecieveBuff(Modem_buf.RX);
    SendATCommand("AT#SGACT=1,0\r\n");                                            // Deactivate 1 PDP context
    RecieveATAnswer(2000);
    // [02] Socket configuration for 1 PDP context
    SkipRecieveBuff(Modem_buf.RX);
    SendATCommand("AT#SCFG=1,1,1024,0,600,1\r\n");                                // PDP , conid , Packet size,socket inactivity timeout, conn timeout, data sending rimeout
    RecieveATAnswer(2000);
    // [03] Define PDP context #1 as IP
    SkipRecieveBuff(Modem_buf.RX);
    memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);                                   // Define PDP context #1 as IP
    int len = sprintf((char *)Modem_buf.TX, "AT+CGDCONT=1,\"IP\",\"%s\",\"0.0.0.0\",0,0\r\n", apn);
    sendDataUsart3((uint8_t *)Modem_buf.TX, len);
    
    if (!WaitOKAnswer(3000))                                                      // Wait for OK
    {
        result = GPRS_noPDP;                                                        // PDP IP context definition error
        return result;                                                              // Try again, from the beginning
    }
    
    // [04] Open GPRS - context activation
    vTaskDelay(5000);
    SkipRecieveBuff(Modem_buf.RX);                                                // Purge and clear bufer
    memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
    len = sprintf((char *)Modem_buf.TX, "AT#SGACT=1,1,\"%s\",\"%s\"\r\n", login, pass);
    sendDataUsart3((uint8_t *)Modem_buf.TX, len);
    
    if (!RecieveATAnswer(60000) || strcmp("ERROR", (const char *)Modem_buf.RX) == 0)
    {
        result = GPRS_noSGACT;                                                      // Context activation error
        return result;
    }
    
    // GPRS CONNECTED
    return result;
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief TELIT GL865 DUAL V3.1. [Disconnect from GPRS service]
*/
void GL865_GprsStop()
{
    GL865_SwitchToATMode();                                                       // Try to switch from COMMAND mode to AT mode
    // [0] Close socket
    SkipRecieveBuff(Modem_buf.RX);                                                // Purge and clear bufer
    SendATCommand("AT#SH=1\r\n");
    RecieveATAnswer(5000);
    // [1] Deactivate context
    SkipRecieveBuff(Modem_buf.RX);                                                // Purge and clear bufer
    SendATCommand("AT#SGACT=1,0\r\n");
    RecieveATAnswer(5000);
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim\Shuvalov Sergey
\brief TELIT GL865 DUAL V3.1. [Header send function]
*/
long sendHeader(const char * const header,  unsigned short *FTP_CRC, time_t dateFrom, time_t dateTo)
{
  long len = sprintf((char *)Modem_buf.TX, header, dateFrom, dateTo);
  *FTP_CRC = GetCRC16(Modem_buf.TX, len, (*FTP_CRC)^0xFFFF);
  sendDataUsart3((uint8_t *)Modem_buf.TX, len);
  return len;
}
/**
\ingroup mdFTPGroup
\author Borodov Maksim\Shuvalov Sergey
\brief TELIT GL865 DUAL V3.1. [Set struct archive fields]
*/
void setArchiveModem(StrArchive *archiveModem, int archNum, StrFlashInfoData *archInfo, int archSize)
{
  archiveModem->sumPagesBlock = 1;                                             // 1 record in block
  archiveModem->typeSelection = 3;                                             // Selected by time
  archiveModem->num = archNum;                                                       // Interval archive
  archiveModem->adressFlashBegin = archInfo->adressFlashBegin;
  archiveModem->adressFlashEnd = archInfo->adressFlashEnd;
  archiveModem->size = archSize; 
}
/**
\ingroup mdFTPGroup
\author Borodov Maksim\Shuvalov Sergey
\brief TELIT GL865 DUAL V3.1. [Read data from flash  and send]
*/
int readAndSendArchiveModem(StrArchive *archiveModem, time_t dateFrom_t, time_t dateTo_t, unsigned short *FTP_CRC, long *bytesSent)
{
  int len = 0;
  if (GetRecSum(archiveModem, dateFrom_t, dateTo_t))
  {
      int RecordCounter = 0;                                                    // Extra check
      
      while ((archiveModem->curPage != archiveModem->sumPages)&& (RecordCounter<10000))
      {
          RecordCounter++;
          
          if (ReadArchiveModem(archiveModem, Modem_buf.TX, &len))                // If bad record - don't send it
          {
              continue;                                                             // Read next one
          }
          
          *FTP_CRC = GetCRC16(Modem_buf.TX, len, (*FTP_CRC)^0xFFFF);                  // Update CRC
          
          if (noCTS_usart3(12000))
          {
              return 0;
          }
          
          sendDataUsart3((uint8_t *)Modem_buf.TX, len);
          *bytesSent += len;
      }
  }
  else
  {
      memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
      len = sprintf((char *)Modem_buf.TX, "\x02(#0103)\x03");
      GetBcc(Modem_buf.TX);
      len++;
      *FTP_CRC = GetCRC16(Modem_buf.TX, len, (*FTP_CRC)^0xFFFF);                    
      if (noCTS_usart3(12000))
      {
          return 0;
      }
      sendDataUsart3((uint8_t *)Modem_buf.TX, len);
      *bytesSent += (len);
  }
  return 1;
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief Connect to FTP server, send data, make CRC, check SIZE
INPUT:
dateFROM
FileName
FTP_login
FTP_pass
*/
int GL865_FtpClient(long *bytesSent)
{
    struct tm dateTo_tm = {0};                                                    // get current date/time
    getRtcDateTime(&dateTo_tm);                                                   // TM format
    char dateTo[20];
    memset(dateTo, 0x00, sizeof(dateTo));
    sprintf(dateTo, "%04u-%02u-%02u,%02u:%02u:%02u\0",                            // String format
            dateTo_tm.tm_year + 1900,
            dateTo_tm.tm_mon+1,
            dateTo_tm.tm_mday,
            dateTo_tm.tm_hour,
            dateTo_tm.tm_min,
            dateTo_tm.tm_sec
                );
    time_t dateTo_t;
    dateTo_t = mktime(&dateTo_tm);
    // Get data from device
    char serial[32];
    memset(serial, 0x00, 32);
    char type[32];
    memset(type, 0x00, 32);
    char vers[32];
    memset(vers, 0x00, 32);
    char FTPlogin[32];
    memset(FTPlogin, 0x00, 32);
    char FTPpass[32];
    memset(FTPpass, 0x00, 32);
    char FTPserver[32];
    memset(FTPserver, 0x00, 32);
    uint32_t FTPport = 0;
    time_t dateFrom_t;
    xSemaphoreTake(xMutexParams, portMAX_DELAY);                                  // Protect by semaphore
    memcpy(serial, params.identDevice, sizeof(params.identDevice));             // Get serial
    memcpy(FTPlogin, params.LoginS, strlen(params.LoginS));                     // Get FTP login
    memcpy(FTPpass, params.PasswS, strlen(params.PasswS));                      // Get FTP pass
    memcpy(FTPserver, params.IP, strlen(params.IP));                            // Get IP-address of server
    FTPport = params.PortS;                                                     // Get port of server
    memcpy(type, params.typeDevice, strlen(params.typeDevice));                 // Get device type
    memcpy(vers, params.versMain, strlen(params.versMain));                     // Get version
    dateFrom_t = params.beginTimeDataFTP;
    xSemaphoreGive(xMutexParams);
    char dateFrom[20];
    memset(dateFrom, 0x00, sizeof(dateFrom));
    struct tm dateFrom_tm;
    struct tm *dateFrom_tmPtr = &dateFrom_tm;
    dateFrom_tmPtr = gmtime ( &dateFrom_t );
    sprintf(dateFrom, "%04u-%02u-%02u,%02u:%02u:%02u\0",                          // String format
            dateFrom_tmPtr->tm_year + 1900,
            dateFrom_tmPtr->tm_mon+1,
            dateFrom_tmPtr->tm_mday,
            dateFrom_tmPtr->tm_hour,
            dateFrom_tmPtr->tm_min,
            dateFrom_tmPtr->tm_sec
                );
    int result = 0;
    char fileName[64];                                                            // BK-G_TC_YYMMDDhhmmss_XXXXXXXXXX.txt
    long oldFileSize = 0;                                                         // The size of file on the server before uploading
    long newFileSize = 0;                                                         // The size of file on the server after uploading
    long FileSize = 0;
    unsigned short FTP_CRC = CRCInit;                                             // Init CRC
    // [00] Connect to FTP
    memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
    int len = sprintf(
                      (char *)Modem_buf.TX, "AT#FTPOPEN=\"%s:%Lu\",\"%s\",\"%s\",1\r\n",
                      FTPserver,
                      FTPport,
                      FTPlogin,
                      FTPpass);
    SkipRecieveBuff(Modem_buf.RX);                                                // Purge and clear bufer
    sendDataUsart3((uint8_t *)Modem_buf.TX, len);
    
    if (!WaitOKAnswer(20000))                                                     // Wait for OK
    {
        result = GPRS_noFTP;                                                        // Can't connect to FTP
        return result;                                                              // Try again, from the beginning
    }
    
    // [01] SET RTS, as FLOW CONTROL is ON
    rtsUp_usart3();                                                               // Set RTS as MODEM uses flow control IFC=2,2
    // [02] Create or attach to file
    memset(fileName, 0x00, sizeof(fileName));
    sprintf(fileName, "%s_%02i%02i%02i%02i%02i%02i_%s.txt\0",                     // Make filename BK-G_TC_YYMMDDhhmmss_XXXXXXXXXX.txt
            type,
            (dateTo_tm.tm_year+1900)%100,                                         // year in 2 digits
            dateTo_tm.tm_mon+1,
            dateTo_tm.tm_mday,
            dateTo_tm.tm_hour,
            dateTo_tm.tm_min,
            dateTo_tm.tm_sec,
            serial
                );
    SkipRecieveBuff(Modem_buf.RX);
    memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
    len = sprintf((char *)Modem_buf.TX, "AT#FTPFSIZE=\"%s\"\r\n\0", fileName);
    sendDataUsart3((uint8_t *)Modem_buf.TX, len);
    RecieveATAnswer(20000);
    
    if (strstr((const char *)Modem_buf.RX, "#FTPFSIZE:"))
    {
        sscanf((char *)Modem_buf.RX, "%*11s %Li", &oldFileSize);                    // CRLF#FTPFSIZE: XXX
        SkipRecieveBuff(Modem_buf.RX);
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
        sprintf((char *)Modem_buf.TX, "AT#FTPAPP=\"%s\"\r\n\0", fileName);
    }
    else
    {
        oldFileSize = 0;
        SkipRecieveBuff(Modem_buf.RX);
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
        sprintf((char *)Modem_buf.TX, "AT#FTPPUT=\"%s\"\r\n\0", fileName);
    }
    
    SendATCommand(Modem_buf.TX);                                                  // [Create or attach to file]
    
    if (WaitATAnswer("CONNECT", 30000))                                           // Connected to FTP server
    {
        // [03] Send data
        *bytesSent = 0;                                                             // Init file send size
        // A - header: string
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);                                 // Clear TX buffer
        len = sprintf((char *)Modem_buf.TX, "\r\nDevice data\r\n(%s)(%s)(%s)(%s)\r\n\0", // Make string
                      serial,
                      type,
                      vers,
                      dateTo
                          );
        FTP_CRC = GetCRC16(Modem_buf.TX, len, FTP_CRC);                             // Init CRC
        //    if (noCTS_usart3(12000))
        //      break;
        sendDataUsart3((uint8_t *)Modem_buf.TX, len);                               // Send data to server
        *bytesSent += len;                                                          // Increment bytes sent counter
        
        StrArchive archiveModem;
        //
        // интервальный архив
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
        *bytesSent += sendHeader(intervHeader, &FTP_CRC, dateFrom_t, dateTo_t);          
        setArchiveModem(&archiveModem, 3, &intervalArchiveInfo, sizeof(StrIntervalArchive));
        archiveModem.sizeCRC = (char *)&intervalArchive.cs - (char *)&intervalArchive;       
        if(!readAndSendArchiveModem(&archiveModem, dateFrom_t, dateTo_t, &FTP_CRC, bytesSent))
        {
            return GPRS_noCTS;
        }
        // суточный архив
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
        *bytesSent += sendHeader(dayHeader, &FTP_CRC, dateFrom_t, dateTo_t); 
        setArchiveModem(&archiveModem, 7, &dayArchiveInfo, sizeof(StrDayArchive));
        archiveModem.sizeCRC = (char *)&dayArchive.cs - (char *)&dayArchive;
        if(!readAndSendArchiveModem(&archiveModem, dateFrom_t, dateTo_t, &FTP_CRC, bytesSent))
        {
            return GPRS_noCTS;
        }
        // месячный архив
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
        *bytesSent += sendHeader(monthHeader, &FTP_CRC, dateFrom_t, dateTo_t); 
        setArchiveModem(&archiveModem, 1, &monthArchiveInfo, sizeof(StrMonthArchive));
        archiveModem.sizeCRC = (char *)&monthArchive.cs - (char *)&monthArchive;
        if(!readAndSendArchiveModem(&archiveModem, dateFrom_t, dateTo_t, &FTP_CRC, bytesSent))
        {
            return GPRS_noCTS;
        }
        // архив событий
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
        *bytesSent += sendHeader(eventsHeader, &FTP_CRC, dateFrom_t, dateTo_t); 
        setArchiveModem(&archiveModem, 4, &eventsArchiveInfo, sizeof(StrEventsArchive));
        archiveModem.sizeCRC = (char *)&eventsArchive.cs - (char *)&eventsArchive;
        if(!readAndSendArchiveModem(&archiveModem, dateFrom_t, dateTo_t, &FTP_CRC, bytesSent))
        {
            return GPRS_noCTS;
        }
        // архив изменений
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
        *bytesSent += sendHeader(changesHeader, &FTP_CRC, dateFrom_t, dateTo_t); 
        setArchiveModem(&archiveModem, 5, &changesArchiveInfo, sizeof(StrChangesArchive));
        archiveModem.sizeCRC = (char *)&changesArchive.cs - (char *)&changesArchive;
        if(!readAndSendArchiveModem(&archiveModem, dateFrom_t, dateTo_t, &FTP_CRC, bytesSent))
        {
            return GPRS_noCTS;
        }
        //        
        // Add CRLF
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);                                 // Clear TX buffer
        len = sprintf((char *)Modem_buf.TX, "\r\n");                                // Make string
        FTP_CRC = GetCRC16(Modem_buf.TX, len, FTP_CRC^0xFFFF);                      // Update CRC
        if (noCTS_usart3(12000))
        {
            return GPRS_noCTS;
        }
        sendDataUsart3((uint8_t *)Modem_buf.TX, len);                               // Send data to server
        *bytesSent += len;                                                          // Increment bytes sent counter
        // D - add CRC
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);                                 // Clear TX buffer
        len = sprintf((char *)Modem_buf.TX, "CRC %5u", FTP_CRC);                    // Make string
        //    if (noCTS_usart3(12000))
        //      break;
        sendDataUsart3((uint8_t *)Modem_buf.TX, len);                               // Send data to server
        *bytesSent += len;                                                          // Increment bytes sent counter
        
        //    memset(Modem_buf.TX,0x00, MODEM_BUF_SIZE);
        //    len = sprintf((char*)Modem_buf.TX, "%i Hello world from BK-G_TC\r\n",i);
        //    sendDataUsart3((uint8_t*)Modem_buf.TX, len);
        //    *bytesSent += len;
        
        //?????vTaskDelay(TICK_MS(7000));
        // Prevent from bad +++
        if (noCTS_usart3(12000))
        {
            return GPRS_noCTS;
        }
        
        // [04] Exit from data mode
        vTaskDelay((2000));
        SkipRecieveBuff(Modem_buf.RX);
        SendATCommand("+++");
        vTaskDelay(1000);
        
        if (!WaitATAnswer("NO CARRIER", 20000))
        {
            return result;
        }
        
        // [05] Get file size
        SkipRecieveBuff(Modem_buf.RX);
        memset(Modem_buf.TX, 0x00, MODEM_BUF_SIZE);
        sprintf((char *)Modem_buf.TX, "AT#FTPFSIZE=\"%s\"\r\n\0", fileName);
        SendATCommand(Modem_buf.TX);                                                // [Get file size]
        RecieveATAnswer(20000);
        
        if (strstr((const char *)Modem_buf.RX, "#FTPFSIZE:"))                       // CRLF#FTPFSIZE: XXX
        {
            sscanf((char *)Modem_buf.RX, "%*11s %Li", &newFileSize);
            
            if (oldFileSize>0)
            {
                FileSize = newFileSize - oldFileSize;
            }
            else
            {
                FileSize = newFileSize;
            }
            
            if (*bytesSent == FileSize)
            {
                result = 0;    // SUCCESS
            }
            else
            {
                result = 1;    // FAILURE
            }
        }
    }
    else
    {
        // No connection or some error
        vTaskDelay(2000);
        SkipRecieveBuff(Modem_buf.RX);
        SendATCommand("+++");
        
        if (!WaitATAnswer("NO CARRIER", 20000))
        {
            return result;
        }
    }
    
    // [06] Close connection
    SkipRecieveBuff(Modem_buf.RX);
    SendATCommand("AT#FTPCLOSE\r\n");
    WaitOKAnswer(20000);
    
    if (result == 0)                                                              // Successfull transmission
    {
        xSemaphoreTake(xMutexParams, portMAX_DELAY);                                // Protect by semaphore
        params.beginTimeDataFTP = dateTo_t;                                       // uPDATE lAST SUCCESSFUL READOUT DATE IN DEVICE
        xSemaphoreGive(xMutexParams);
    }
    
    return result;
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief TELIT GL865 DUAL V3.1. [Clear buffer]
*/
void SkipRecieveBuff(unsigned char *buf)                                        // Get data from modem
{
    int len = 0;
    
    while (recv_usart3((uint8_t *)(buf+len), 20))                                 // Read data from RX IRQ queue, if any
    {
        len++;
    };
    
    memset(Modem_buf.RX, 0x00, MODEM_BUF_SIZE);                                   // Clear buffer
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief Recieve answer from MODEM, compare it with RightAnswer
*/
bool WaitATAnswer(char *rightAns, portTickType waitForAnswer)
{
    memset(Modem_buf.RX, 0x00, MODEM_BUF_SIZE);                                   // Clear modem RX buffer
    
    if (RecieveATAnswer(waitForAnswer))
    {
        if (strcmp((char const *)Modem_buf.RX, rightAns) == 0)
        {
            return true;
        }
    }
    
    return false;
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief
Function gets AT command answer in format "\r\nANSWER\r\n"
Makes answer in format "ANSWER\0" and saves it in Modem_buf.RX
Waits for 'waitForAnswer' ms for any answer

Returns:
TRUE if answer got
FALSE if no answer got or format error
*/
bool RecieveATAnswer(portTickType waitForAnswer)
{
    unsigned char byte;
    unsigned int RX_start_byte_cnt = 0;                                           // RX bytes counter in the start of message (skip \r\n)
    uint8_t *buffForAns = (uint8_t *) Modem_buf.RX;
    
    if (!recv_usart3(&byte, waitForAnswer))                                       // Get the first byte in accordance with timeout
    {
        return false;    // No data - return false
    }
    
    if (byte != '\r')                                                             // Skip \r char (CR)
    {
        *buffForAns++ = byte;
        RX_start_byte_cnt++;
    }
    
    if (!recv_usart3(&byte, BYTE_TIMEOUT))                                        // Get the second byte
    {
        return false;
    }
    
    if (byte != '\n')                                                             // Skip \n char (LF)
    {
        *buffForAns++ = byte;                                                       // No data - format error - return false
        RX_start_byte_cnt ++;
    }
    
    // Got \r\n or 2 other bytes
    // Get the full answer
    // The length is limited by MODEM_BUF_SIZE chars
    for (int i = 0; i < (MODEM_BUF_SIZE-RX_start_byte_cnt); i++)
    {
        if (!recv_usart3(&byte, BYTE_TIMEOUT))
        {
            *buffForAns = '\0';                                                       // make null-terminated string
            return false;                                                             // No data - format error - return false
        }
        
        if (
            (byte == '\n') &&                                                       // got byte \n (LF)
                ((int)(buffForAns-1) >= (int)(&Modem_buf.RX)) &&                        // check buffer left limit
                    (*(buffForAns-1) == '\r')                                               // previous byte was \r (CR)
                        )
        {
            *(buffForAns-1) = '\0';                                                   // remove \r from the answer and terminate string with 0x00
            return true;                                                              // got the last /r/n
        }
        
        *buffForAns++ = byte;                                                       // Save byte in buffer
    }
    
    return false;
}

/**
\ingroup mdFTPGroup
\author Borodov Maksim
\brief Switch from DATA mode to COMMAND mode
*/
bool GL865_SwitchToATMode()
{
    vTaskDelay(1105);
    SendATCommand("+++");
    vTaskDelay(1105);
    return WaitOKAnswer(100);
}

/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция формирует интервальный архив.
*/
int ReadArchiveModem(StrArchive *archiveModem, unsigned char *bufTX, int *count)
{
    int result = 0;
    memset(bufTX, 0, MODEM_BUF_SIZE);
    uint32_t adress = 0;
    int oldPages = 0; //уже выданные записи - используется при переполнении
    //int count = 0;
    
    while(archiveModem->curPage < archiveModem->sumPages)
    {
        *count = 0;
        adress = archiveModem->adrBegin + archiveModem->curPage*256;
        
        //если переполнение новые данные в начале адресного пространства флеш
        if(adress > archiveModem->adressFlashEnd)
        {
            oldPages = (archiveModem->adressFlashEnd + 1 - archiveModem->adrBegin) >> 8;
            adress = archiveModem->adressFlashBegin + (archiveModem->curPage - oldPages) *256;
        }
        
        readDataFromFlash(bufArc, archiveModem->size, adress);
        
        if(archiveModem->curPage % archiveModem->sumPagesBlock == 0)
        {
            bufTX[*count] = STX;
            *count += 1;
        }
        
        archiveModem->curRecInBlock++;
        archiveModem->curPage++;
        
        switch(archiveModem->num)
        {
            //интервальный архив
            case 3:
            createIntervalArchiveString((char *)bufTX, (StrIntervalArchive *)bufArc, count);
            break;
            
            //суточный архив
            case 7:
            createDayArchiveString((char *)bufTX, (StrDayArchive *)bufArc, count);
            break;
            
            //месячный архив
            case 1:
            createMonthArchiveString((char *)bufTX, (StrMonthArchive *)bufArc, count);
            break;
            
            //архив событий
            case 4:
            createEventsArchiveString((char *)bufTX, (StrEventsArchive *)bufArc, count);
            break;
            
            //архив изменений
            case 5:
            createChangesArchiveString((char *)bufTX, (StrChangesArchive *)bufArc, count);
            break;
        }
        
        // проверка CRC записи
        uint16_t crc = 0;
        crc = GetCRC16(bufArc, archiveModem->sizeCRC, CRCInit);
        
        if(crc == *((uint16_t *)(bufArc + archiveModem->sizeCRC)))
        {
            *count += sprintf((char *)bufTX + *count, "(CRC Ok)\x00");
        }
        else
        {
            if(params.sendErrLis200)
            {
                count += sprintf((char *)bufTX + *count, "(CRC Error)\x00");
            }
            else
            {
                if (archiveModem->sumPages > 0)                                       // Decrement whole number of pages
                {
                    archiveModem->sumPages--;
                }
                
                if (archiveModem->curPage > 0)                                        // Decrement number os sent pages, as this page was not snt
                {
                    archiveModem->curPage--;
                }
                
                result = 1;                                                           // Don't sent the answer
                break;
            }
        }
        
        // последняя запись из диапазона (вставить ETX)
        if(archiveModem->curPage == archiveModem->sumPages)
        {
            bufTX[*count] = ETX;
            *count += 1;
            archiveModem->bccBlock ^= countBcc (bufTX);
            bufTX[*count] = archiveModem->bccBlock;
            *count += 1;
        }
        // последняя страница в блоке (вставить EOT)
        else if(archiveModem->curPage % archiveModem->sumPagesBlock == 0)
        {
            bufTX[*count] = EOT;
            *count += 1;
            archiveModem->bccBlock ^= countBcc (bufTX);
            bufTX[*count] = archiveModem->bccBlock;
            *count += 1;
        }
        else
        {
            bufTX[*count] = 0x0D;
            bufTX[*count +1] = 0x0A;
            *count += 2;
        }
        
        //        archiveModem->bccBlock ^= countBcc (bufTX);                           // 1 record per block!!!
        //        sendDataUsart3((uint8_t *)bufTX, count+1);
        //        memset(bufTX, 0, MODEM_BUF_SIZE);
        
        //выдали блок - ждём АСК
        if(archiveModem->curPage % archiveModem->sumPagesBlock == 0)
        {
            break;
        }
    }
    
    return result;
}

/**
\ingroup mdLis200Group
\author Borodov Maksim
\brief Local+ Global numbers.
*/
void createIntervalArchiveStringModem(char *bufTX, StrIntervalArchive *archive, int *count)
{
    struct tm *t = localtime(&archive->timeDate);
    // формируем строку
    *count += sprintf(bufTX + *count,
                      "(%u)(%u)(%.2u-%.2u-%.2u,%.2u:%.2u:%.2u)(%0.4f)(%0.4f)(%0.4f)(%0.4f)(%0.2f)(%0.4f)(%0.4f)",
                      archive->count, archive->count, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,
                      archive->Vc, archive->VcO, archive->Vp, archive->VpO, archive->averageTemper, archive->PpodAvI, archive->averageKkor
                          );
    bufTX = bufTX + *count;
    bufTX = FmtStatus(archive->st5, bufTX, count);
    bufTX = FmtStatus(archive->st6, bufTX, count);
    bufTX = FmtStatus(archive->sysSt, bufTX, count);
    *count += sprintf(bufTX, "(0x%.4X)", archive->eventCode);
}