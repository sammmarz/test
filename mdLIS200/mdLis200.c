/**
\file
Файл содержит функции для реализации протокола LIS200.
*/

/** \defgroup mdLis200Group mdLis200
Специализация модуля - реализации протокола LIS200.
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "../Params/Params.h"
#include "mdHardware/mdHardware.h"
#include "mdDataManager/CheckProgramData.h"
#include "mdOpticalInterface/mdOpticalInterface.h"
#include "mdLis200.h"
#include "../mdDisplay/mdDisplay.h"
#include "mdDataManager/mdDataManager.h"
#include "mdFlashMemory/mdFlashMemory.h"
#include "mdModes/mdModes.h"

char stringBuf[25];
int exchangeRegim;
// programming
#define bufCRCsize 128
volatile int timeOUTprogramming;
int sumBytesInBlock;
uint32_t sumBlockProgramm = 0;
uint32_t lastBlockProgramSize = 0;
uint16_t crcMetrInProg;
uint16_t crcMainInProg;
int counterBytes = 0;
int numBlock = 1;
int sizeMeter;
uint16_t crcMetr;
uint16_t crcMain;
uint16_t countMain;
uint16_t countMetr;
uint8_t bufMetr[bufCRCsize];
uint8_t bufMain[bufCRCsize];
int numProgramByte;
uint16_t adressLis200;
uint16_t instansLis200;
//
extern uint8_t accessStatus;
extern SemaphoreHandle_t xMutexParams;
extern QueueHandle_t xBtnsQueue;
extern StrDataUART optData;

bool exchangeRegim0(StrDataUART *optData);
bool exchangeRegim1(StrDataUART *optData);
void exchangeRegim2(StrDataUART *dataUART);
bool exchangeRegim3(StrDataUART *dataUART);
unsigned int PrepAnswer(StrDataUART *dataUART);
unsigned int WriteValue(StrDataUART *dataUART);
unsigned char CheckLock(unsigned char pRights, unsigned char pRW);
void startControlTimeOut(void);
void stopControlTimeOut(void);

void prepareToCheckProgramData(void);
int checkProgramDataStruct(StrBufProgram * strProg);
void countCRCforProgramData(uint8_t * data, int length, bool lastData);
int GetArchiveHead(char *bufTX, unsigned char Attribute);
int GetArchiveRecSum(char *bufTX,  unsigned char *p_Order);
void ReadArchive(char *bufTX);
int getAttribute(char *str, uint8_t *attribute);
int getTypeSelection(char *str, uint8_t *typeSelection);
int getSumPagesInBlock(char *str, uint16_t *sumPagesBlock);
int GetBcc (unsigned char *p_msg);
int CheckBcc (unsigned char *p_msg);

extern const unsigned short CRCInit;
unsigned short GetCRC16(unsigned char *p_Buff, unsigned int p_Length, unsigned short CRCInit1);

/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция проверяет пришедшие байты на начало обмена.
*/
bool checkBeginExchange(StrDataUART *dataUART)
{
    bool begExchange = false;
    char *ptr = 0;
    
    for(int i = 0; i < BUF_UART_MAX; ++i)
    {
        if(dataUART->bufRX[i] == '/')
        {
            memset(stringBuf, 0, sizeof(stringBuf));
            sprintf(stringBuf, "/?%s\x0D\x0A", params.identDevice);
            ptr = strstr(&dataUART->bufRX[i], stringBuf);
            
            if(ptr)
            {
                clearRXBuf(dataUART);
                memcpy(dataUART->bufRX, stringBuf, strlen(stringBuf));
                begExchange = true;
                break;
            }
            
            memset(stringBuf, 0, sizeof(stringBuf));
            sprintf(stringBuf, "/?!\x0D\x0A");
            ptr = strstr(&dataUART->bufRX[i], stringBuf);
            
            if(ptr)
            {
                clearRXBuf(dataUART);
                memcpy(dataUART->bufRX, stringBuf, strlen(stringBuf));
                begExchange = true;
                break;
            }
        }
    }
    
    return begExchange;
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция анализирует пришедший байт в соответствии с протоколом Lis200.
*/
bool analyseReceivedByteUARTasLis200(char ch, StrDataUART *dataUART)
{
    bool needSendTXbuf = false;
    //прописываем пришедший символ в строку
    addByteInBufRX(ch, dataUART);
    
    if((exchangeRegim != 3) && (checkBeginExchange(dataUART)))
    {
        exchangeRegim = 0;
    }
    
    switch (exchangeRegim)
    {
        case 0:
        stopControlTimeOut();
        if(strstr(dataUART->bufRX, "\x0D\x0A"))
        {
            needSendTXbuf = exchangeRegim0(dataUART);
            clearRXBuf(dataUART);
        }
        
        break;
        
        case 1:
        startControlTimeOut();
        if((dataUART->indx - 2) >= 0)
        {
            if(dataUART->bufRX[dataUART->indx - 2] == ETX)
            {
                needSendTXbuf = exchangeRegim1(dataUART);
                clearRXBuf(dataUART);
                
                if(exchangeRegim == 2)
                {
                    needSendTXbuf = false;
                    exchangeRegim2(dataUART);
                }
            }
        }
        
        break;
        
        case 2:
        startControlTimeOut();
        if(dataUART->bufRX[dataUART->indx - 1] == ACK)
        {
            clearRXBuf(dataUART);
            archive.curRecInBlock = 0;
            
            //АСК на ЕТХ
            if(archive.curPage == archive.sumPages)
            {
                exchangeRegim = 1;
                return 0;
            }
        }
        
        if(dataUART->bufRX[dataUART->indx - 1] == NAK)
        {
            clearRXBuf(dataUART);
            // повторяем выдачу последнего блока
            archive.curPage -= archive.curRecInBlock;
        }
        
        exchangeRegim2(dataUART);
        break;
        case 3:
        startControlTimeOut();
        needSendTXbuf = exchangeRegim3(dataUART);
        break;
    }
    
    return needSendTXbuf;
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция реализации идентификации и установки скорости при начале обмена.
*/
bool exchangeRegim0(StrDataUART *dataUART)
{
    //идентификация устройства
    sprintf(stringBuf, "/?%s\x0D\x0A", params.identDevice);
    
    if(strstr(dataUART->bufRX, stringBuf))
    {
        //sprintf(dataUART->bufTX, "/Els%d\\BKET\x0D\x0A", params.speedUARTcode);
        sprintf(dataUART->bufTX, "/ELS%d%s\x0D\x0A", params.speedUARTcode, params.typeDevice);
        return true;
    }
    
    sprintf(stringBuf, "/?!\x0D\x0A");
    
    if(strstr(dataUART->bufRX, stringBuf))
    {
        sprintf(dataUART->bufTX, "/Els%d\\%s\x0D\x0A", params.speedUARTcode, params.typeDevice);
        return true;
    }
    
    //подтверждение от сервера (АСК) установка скорости UART
    char *ch = strchr(dataUART->bufRX, ACK);
    int z;
    int speed = 300;
    
    if(ch)
    {
        sscanf(&ch[2], "%1d", (int *)&z);
        
        if(z == params.speedUARTcode)
        {
            switch(z)
            {
                case 0:
                speed = 300;
                break;
                
                case 1:
                speed = 600;
                break;
                
                case 2:
                speed = 1200;
                break;
                
                case 3:
                speed = 2400;
                break;
                
                case 4:
                speed = 4800;
                break;
                
                case 5:
                speed = 9600;
                break;
                
                case 6:
                speed = 19200;
                break;
            }
        }
        else
        {
            speed = 300;
        }
        
        initUSART_1(speed, USART_Parity_Even);
        // задержка для переконфигурирования USART
        vTaskDelay(200);
        //отсылаем псевдокоманду пароля
        strcpy(dataUART->bufTX, "\x01P0\x02(1234567)\x03P\x00");
        // обновляем информацию о статусе в регистре
        StrEventForDisplay cmdUpdate;
        cmdUpdate.status = 1;
        cmdUpdate.codeEventForDisplay = UPDATE_DISPLAY;
        xQueueSendToBack(xBtnsQueue, &cmdUpdate, NULL);
        dataUART->BCCisZERO = GetBcc((unsigned char *)dataUART->bufTX);
        startControlTimeOut();
        exchangeRegim = 1;
        return true;
    }
    
    return false;
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция обмена данными по протоколу Lis200.
*/
bool exchangeRegim1(StrDataUART *dataUART)
{
    unsigned char *p_RxBuf = 0;
    unsigned int error = 0;
    
    char *ptrBeg = 0;
    char *ptrEnd = 0;
    //получена команда завершениЯ свЯзи
    if (strstr(dataUART->bufRX, "B0\x03q"))
    {
        exchangeRegim = 0;
        //disableOptPower();
        initOpticalInterface();
        clearTXBuf(dataUART);
        clearRXBuf(dataUART);
        xQueueReset(xOptBufQueue);
        enableOptPower();
        resetEvent(EV_OPT_DATA_START);
        firstOptInterfByteExchange = false;
        //printf("\ninfo got B0 - exchange end");
        return false;
    }
    
    clearTXBuf(dataUART);
    p_RxBuf = (unsigned char *) strstr((char *)dataUART->bufRX, "\x01R");
    
    if (!p_RxBuf)
    {
        p_RxBuf = (unsigned char *) strstr((char *)dataUART->bufRX, "\x01W");
    }
    if (!p_RxBuf)
    {
        p_RxBuf = (unsigned char *) strstr((char *)dataUART->bufRX, "\x01P");
    }
    
    if (!p_RxBuf)
    {
        error = Err_Telegramma;    //не найдено начало телегр
    }
    
    //проверим контрольную сумму
    if (!error)
    {
        p_RxBuf++;
        error = CheckBcc(p_RxBuf);
        char* ptrBCC = strrchr((char const*)p_RxBuf, ETX) + 1; 
        *ptrBCC = 0;
    }
    
    //получим вид команды (R или W или P, 1 или 3)
    if (!error)
    {
        switch (*p_RxBuf)
        {
            case 'R'  :
            error = PrepAnswer(dataUART);
            break;
            
            case 'W'  :
            xSemaphoreTake (xMutexParams, portMAX_DELAY);
            error = WriteValue(dataUART);
            xSemaphoreGive(xMutexParams);
            taskYIELD();
            
            if (!error)
            {
                dataUART->bufTX[0] = ACK;
                dataUART->bufTX[1] = 0;
                writeParamsToFlash();
                return true;
            }
            
            break;
            case 'P'  :
            // проверка на вход в режим перепрограммирования
            
            ptrBeg = (char *) strchr((const char *)p_RxBuf, '(');
            ptrEnd = (char *) strchr((const char *)p_RxBuf, ')');
            *ptrEnd = 0;
            sumBytesInBlock = (int)strtol((const char *)ptrBeg+1, 0, 0);
            if((sumBytesInBlock >= 128) && (sumBytesInBlock <= 512))
            {
                exchangeRegim = 3;
                // сбрасывваем регистр обновления
                xSemaphoreTake (xMutexParams, portMAX_DELAY);
                params.RegPr = 0; 
                xSemaphoreGive(xMutexParams);
                //
                clearRXBuf(dataUART);
                dataUART->bufTX[0] = ACK;
                dataUART->bufTX[1] = 0;
                // подготавливаем счетчики
                sumBlockProgramm = 0;
                lastBlockProgramSize = 0;
                counterBytes = 0;
                numProgramByte = 0;
                numBlock = 1;  
                countMain = 0;
                countMetr = 0;
                sizeMeter = 256; // как минимум, после получения структуры прошива пересчитается
                indxBufProgram = 0;
                crcMetr = ~CRCInit;
                crcMain = ~CRCInit;                
                // начинаем писать с начала отведенной памяти
                bufProgramInfo.adress = bufProgramInfo.adressFlashBegin;
                bufProgramInfo.flags |= CONTROL_CRC;
                resetCrcFlashError();
            }
            else
            {
                dataUART->bufTX[0] = NAK;
                dataUART->bufTX[1] = 0;
            }
            return true;    
            break;
            
            default:
            error = Err_Telegramma;
        }
    }
    
    if (error) //есть ошибки - возвращает код ошибки
    {
        clearTXBuf(dataUART);
        // if (error==1)
        // _NOP();
        sprintf((char *)dataUART->bufTX, "\x02(#%4.4d)\x03\x00", error);
        dataUART->BCCisZERO = GetBcc((unsigned char *)dataUART->bufTX);
        return true;
    }
    
    dataUART->BCCisZERO = GetBcc((unsigned char *)dataUART->bufTX);
    return true;
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция обмена данными в режиме передачи архива.
*/
void exchangeRegim2(StrDataUART *dataUART)
{
    //получена команда завершениЯ свЯзи
    if (strstr(dataUART->bufRX, "B0\x03q"))
    {
        /*
        resetEvent(EV_OPT_DATA_START);
        firstOptInterfByteExchange = false;
        exchangeRegim = 0;
        */
        exchangeRegim = 0;
        //disableOptPower();
        initOpticalInterface();
        clearTXBuf(dataUART);
        clearRXBuf(dataUART);
        xQueueReset(xOptBufQueue);
        enableOptPower();
        resetEvent(EV_OPT_DATA_START);
        firstOptInterfByteExchange = false;
        //printf("\ninfo got B0 - exchange end");
        return;
    }
    
    clearTXBuf(dataUART);
    ReadArchive(dataUART->bufTX);
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция обмена данными в режиме перепрограммирования.
*/
bool exchangeRegim3(StrDataUART *dataUART)
{
    int i = 0;
    bool needSendData = false; 
           
    counterBytes++;
    // 3: STX, EOT, BCC начинаем разбор пришедшего блока
    int sumBytes = sumBytesInBlock + 3;
    // блок пришел 
    if((counterBytes == sumBytes) ||
       (lastBlockProgramSize && (sumBlockProgramm == numBlock) && (counterBytes == lastBlockProgramSize + 3)))
    {           
        int indxBCC = sumBytes-1;
        // если последний блок не полный, то BCC смещается
        if(lastBlockProgramSize && (sumBlockProgramm == numBlock))
        {
            // 2: STX, EOT,
            indxBCC = lastBlockProgramSize + 2;        
        }        
        // подсчитываем BCC
        unsigned char BccBlock = 0;
        for(i=1; i<indxBCC; ++i)
        {
            BccBlock = BccBlock ^ dataUART->bufRX[i];
        }    
        if(BccBlock == (unsigned char)dataUART->bufRX[indxBCC])
        {
            dataUART->bufTX[0] = ACK;
            dataUART->bufTX[1] = 0;
            needSendData = true;
        }
        else
        {
            dataUART->bufTX[0] = NAK;
            dataUART->bufTX[1] = 0; 
            return true;
        }  
        
        // копирование данных в буфер для флеш (-1 : исключаем EOT или ETX
        int indx = 0;
        for(i = 1; i < indxBCC-1; i+=2)
        {       
            indx = indxBufProgram % 256;
            bufProgram[indx] = dataUART->bufRX[i] & 0xf;
            bufProgram[indx] |= (dataUART->bufRX[i+1] << 4) & 0xf0;
            indxBufProgram++;
            if(indx == 255)
            {
                // структура лежит в начале  3-й страницы
                if(indxBufProgram / 256 == 3)
                {
                    if(!checkProgramDataStruct((StrBufProgram *)bufProgram))
                    {
                        sumBlockProgramm = 0;
                        lastBlockProgramSize = 0;
                        counterBytes = 0;
                        numBlock = 0;            
                        // переходим в штатный режим на той же скорости
                        exchangeRegim = 1;  
                        return false;
                    }
                    sumBlockProgramm = ((StrBufProgram *)bufProgram)->allSize * 2 / sumBytesInBlock;
                    lastBlockProgramSize = (((StrBufProgram *)bufProgram)->allSize * 2) % sumBlockProgramm;
                    if(lastBlockProgramSize)
                    {
                        sumBlockProgramm++;
                    }
                    crcMetrInProg = ((StrBufProgram *)bufProgram)->crcMetr;
                    crcMainInProg = ((StrBufProgram *)bufProgram)->crcMain;
                    sizeMeter = ((StrBufProgram *)bufProgram)->sizeMetr;                    
                }
                // записываем данные во флэш
                writeDataToFlashQueue((uint8_t *)bufProgram, 256, &bufProgramInfo);
                countCRCforProgramData((uint8_t *)bufProgram, 256, false);                
            }
        }
        // последний блок - подчищаем после окончания
        if(sumBlockProgramm == numBlock)
        {            
            // если последний блок прописываем остатки во флэш
            if(lastBlockProgramSize)
            {
                writeDataToFlashQueue((uint8_t *)bufProgram, indxBufProgram % 256, &bufProgramInfo);
                countCRCforProgramData((uint8_t *)bufProgram, indxBufProgram % 256, true);
                indxBufProgram = 0;
            }
            xSemaphoreTake (xMutexParams, portMAX_DELAY);
            params.RegPr |= PROG_ANALYSE_PROCESS | PROG_LEN_OK; 
            xSemaphoreGive(xMutexParams);
            
            // проверка CRC
            if(crcMetrInProg == crcMetr)
            {
                xSemaphoreTake (xMutexParams, portMAX_DELAY);
                params.RegPr |= PROG_CRCMETR_OK; 
                xSemaphoreGive(xMutexParams);
            }
            if(crcMainInProg == crcMain)
            {
                xSemaphoreTake (xMutexParams, portMAX_DELAY);
                params.RegPr |= PROG_CRCMAIN_OK;
                xSemaphoreGive(xMutexParams);
            }
            if((params.RegPr & PROG_CRCMAIN_OK) && (params.RegPr & PROG_CRCMETR_OK))
            {
                xSemaphoreTake (xMutexParams, portMAX_DELAY);
                params.RegPr |= PROG_CRC_OK;
                xSemaphoreGive(xMutexParams);
            }
            
            sumBlockProgramm = 0;
            lastBlockProgramSize = 0;
            counterBytes = 0;
            numBlock = 0;            
            // переходим в штатный режим на той же скорости
            exchangeRegim = 1;
            startControlTimeOut();
            //помещаем информацию о режиме в очередь
            StrEventMode mode;
            mode.status = 1;
            mode.modeCode = DATA_M;
            xQueueSendToBackFromISR(xModesQueue, &mode, NULL);
            // информация для DataManager
            StrDataManager forDataManager;
            forDataManager.cmd = DM_CHECK_FLASH_CRC_ERROR;
            xQueueSendToBackFromISR(xDataManagerQueue, &forDataManager, NULL);            
        }
        clearRXBuf(dataUART);
        counterBytes = 0;
        numBlock++;
    }
    return needSendData;
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция проверяет структуру файла прошива.
*/
int checkProgramDataStruct(StrBufProgram * strProg)
{
    int result = 1;
    // проверка CRC структуры
    uint16_t crcStruct = GetCRC16(bufProgram, sizeof(StrBufProgram) - 2, CRCInit);
    if(crcStruct == strProg->crc16)
    {
        xSemaphoreTake (xMutexParams, portMAX_DELAY);
        params.RegPr |= PROG_CRCSTR_OK;
        xSemaphoreGive(xMutexParams);
    } 
    else
    {
        xSemaphoreTake (xMutexParams, portMAX_DELAY);
        params.RegPr &= ~PROG_CRCSTR_OK;
        params.RegPr &= ~PROG_ANALYSE_PROCESS;
        xSemaphoreGive(xMutexParams);
        result = 0;
    }
    
    if(strProg->verBoard == params.verBoard)
    {
        xSemaphoreTake (xMutexParams, portMAX_DELAY);
        params.RegPr |= PROG_VERBOARD_OK; 
        xSemaphoreGive(xMutexParams);
    }
    else
    {
        xSemaphoreTake (xMutexParams, portMAX_DELAY);
        params.RegPr &= ~PROG_ANALYSE_PROCESS;
        xSemaphoreGive(xMutexParams);
        result = 0;
    }
    if(!strncmp((char*)strProg->verDevice, "BK-G_TC", 10))
    {
        xSemaphoreTake (xMutexParams, portMAX_DELAY);
        params.RegPr |= PROG_DEVICE_OK;
        xSemaphoreGive(xMutexParams);
    } 
    else
    {
        result = 0;
    }
    return result; 
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция проверяет структуру.
*/
void countCRCforProgramData(uint8_t * data, int length, bool lastData)
{    
    int i = 0;
    int shAdrStr = 0x200;
    int shAdrMetr = 0x238;   
    while(length--)
    {        
        if((shAdrMetr <= numProgramByte) && (numProgramByte < 0x238+sizeMeter))
        {
            bufMetr[countMetr++] = data[i];
            if((countMetr == bufCRCsize) && (!lastData))
            {
                crcMetr = GetCRC16(bufMetr, bufCRCsize, ~crcMetr);
                countMetr = 0;
            }  
        }
        else if((shAdrStr <= numProgramByte) && (numProgramByte < shAdrMetr))
        {
            // пропускаем идентификационную структуру
        }
        else 
        {
            bufMain[countMain++] = data[i];
            if((countMain == bufCRCsize) && (!lastData))
            {
                crcMain = GetCRC16(bufMain, bufCRCsize, ~crcMain);
                countMain = 0;
            }   
        }   
        numProgramByte++;
        i++;
    }
    // досчитываем остатки
    if(lastData)
    {
        crcMetr = GetCRC16(bufMetr, countMetr, ~crcMetr);
        crcMain = GetCRC16(bufMain, countMain, ~crcMain);
    }
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция вызывается в обработчике таймера.
\param [in] mlSec период таймера для расчета таймаута
*/
void controlTimeOut(int mlSec)
{
    timeOUTprogramming += mlSec;
    // 7 секунд - выходим из режима перепрограммирования
    if(timeOUTprogramming > params.timeOutForSwitch300bod+1)
    {
        exchangeRegim = 0;
        //прекращаем отслеживать таймауты
        stopControlTimeOut();
        //
        initOpticalInterface();
        clearTXBuf(&optData);
        clearRXBuf(&optData);
        xQueueReset(xOptBufQueue);
        enableOptPower();
        resetEvent(EV_OPT_DATA_START);
        firstOptInterfByteExchange = false;       
    }
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция инициирует контроль таймаута для переключения на скорость 300
*/
void startControlTimeOut(void)
{
    timeOUTprogramming = 1;
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция прекращает контроль таймаута
*/
void stopControlTimeOut(void)
{
    timeOUTprogramming = 0;
}
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция реализации подготовки данных при чтении.
*/
unsigned int PrepAnswer(StrDataUART *dataUART)
{
    unsigned error = 0;
    unsigned char *buf_ptr, *p_loc;
    int index;
    unsigned instans, mod, attribute, adress, cnt;
    uint8_t Attribute;
    ReadFmt PrintValue;
    buf_ptr = (unsigned char *) strchr((char *)dataUART->bufRX, STX);
    
    if (!buf_ptr)
    {
        error = Err_Telegramma;    // кривая строка
    }
    
    if (!error)
    {
        buf_ptr++;
        cnt = sscanf((char *) buf_ptr, "%u", &instans );
        instansLis200 = instans;
        if (cnt != 1 )
        {
            error = Err_Telegramma;
        }
    }
    
    if (!error)
    {
        buf_ptr = (unsigned char *) strchr((char *)dataUART->bufRX, ':');
        p_loc = (unsigned char *) strchr((char *)dataUART->bufRX, 'V');
        
        if  (p_loc)	//Archive data request
        {
            // получаем атрибут
            if(!getAttribute((char *)p_loc, &Attribute))
            {
                return Err_Telegramma;
            }
            
            // определяем номер архива
            if ((instans > 7) || (instans < 1))
            {
                return Err_Telegramma;
            }
            
            archive.num = instans;
            
            // определяем тип выборки (время \ счётчик \ глоб.счётчик)
            if((Attribute == 0) || (Attribute == 10))
                if(!getTypeSelection((char *)p_loc, &archive.typeSelection))
                {
                    return Err_Telegramma;
                }
            
            switch (Attribute)
            {
                case 0:
                
                // определяем количество записей в блоке
                if(!getSumPagesInBlock((char *)p_loc, &archive.sumPagesBlock))
                {
                    return Err_Telegramma;
                }
                
                error = GetArchiveRecSum(dataUART->bufTX, p_loc+1);
                
                if (!error) //limits are found and request is correct
                {
                    exchangeRegim = 2; //режим передачи архивов
                    return 0;
                }
                
                break;
                
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                error = GetArchiveHead(dataUART->bufTX, Attribute);
                break;
                
                case 0x0A:
                error = GetArchiveRecSum(dataUART->bufTX, p_loc+1);
                break;
                
                default :
                strcpy((char *)dataUART->bufTX, "\x02(#0004)\x03");
            }
            
            return error;
        }
        else
        {
            cnt = sscanf((char *) ++buf_ptr, "%x", &adress);
            attribute = (adress / 0x1000);
            adress = adress % 0x1000;
            adressLis200 = adress;
            if (cnt != 1 )
            {
                error = Err_Telegramma;
            }
        }
    }
    
    if (!error)
    {
        buf_ptr = (unsigned char *) strchr((char *)dataUART->bufRX, '_');
        
        if (!buf_ptr)
        {
            mod = 0;
        }
        else
        {
            cnt = sscanf((char *) ++buf_ptr, "%u", &mod);
        }
        
        index = GetIndexIfAdressForTriggerEvents (adress, instans);
        
        if(index < 0)
        {
            index = GetIndexIfAdressForStatuses(adress, instans);
        }
        
        if(index < 0)
        {
            index = GetIndexIfAdressForCalib(adress, instans);
        }
        
        if(index < 0)
        {
            index = GetIndex(adress, instans, mod);
        }
        
        if (index < 0)
        {
            error = EC_AbsentValue;
        }
    }
    
    // Wrong error code; EC_19 -  no access rights for write
    if (!error)
    {
        error = CheckLock(DataTable[index].AC_AccRight,
                          Acc_Read);                                                                                           //      EC_18 - No access rights for write
    }
    
    char *OutBuf = dataUART->bufTX;
    
    if (!error)
    {
        PrintValue = DataTable[index].RdFunc;
        
        if (PrintValue == 0)
        {
            return Err_FmtFunc;
        }
        
        // заголовок ответа
        cnt = sprintf((char *)OutBuf, "\x02%1.1u:%3.3X", instans, adress + attribute*0x1000);
        OutBuf += cnt;
        
        if (mod)
        {
            cnt = sprintf((char *)OutBuf, "_%1.1X", mod);
            OutBuf += cnt;
        };
        
        // "Тело"
        switch(attribute)
        {
            case 0:  // - value
            PrintValue(index, (unsigned char *)OutBuf, DataTable[index].AC_Value);
            break;
            
            case 1:  // Acc code
            sprintf((char *)OutBuf, "(0x%2.2X)\x03", DataTable[index].AC_AccRight);
            break;
            
            case 2: // Name
            sprintf((char *)OutBuf, "(%s)\x03", DataTable[index].AC_Name);
            break;
            
            case 3:
            if (DataTable[index].AC_DimStr)        //if pointer on the dimesion string not equal 0
            {
                sprintf((char *)OutBuf, "(%s)\x03", (char *)DataTable[index].AC_DimStr);
            }
            else // pointer on the dimension string = 0
            {
                sprintf((char *)OutBuf, "()\x03");
            }
            
            break;
            
            case 4:
            sprintf((char *)OutBuf, "(0x%X)\x03", DataTable[index].AC_DataClass);
            break;
            
            case 5	:	//Default value
            if (DataTable[index].AC_DefValue)
            {
                (*DataTable[index].RdFunc)(index, (unsigned char *)OutBuf, DataTable[index].AC_DefValue);
            }
            else
            {
                sprintf((char *)OutBuf, "()\x03");
            }
            
            break;
            
            case 6	:	//Range
            readRangeParam(index, OutBuf);
            break;
            
            case 7	:	//Backup and Security
            sprintf((char *)OutBuf, "(%d)\x03", DataTable[index].MemType);
            break;
            
            case 8	:	//Dimension Code
            sprintf((char *)OutBuf, "(0x%.2X)\x03", DataTable[index].AC_DimCode);
            break;
            
            case 9	:	//Format
            sprintf((char *)OutBuf, "(0x%.2X)\x03", DataTable[index].AC_Format);
            break;
            
            case 10 :	//Data type
            sprintf((char *)OutBuf, "(0x%.2X)\x03", DataTable[index].AC_DataType);
            break;
            
            default:
            error = EC_WrongValueRange;
            break;
        }  // end swith
    }
    
    return error;
}//void PrepAnswer(unsigned char* InBuf, unsigned char* OutBuf)
/**
\ingroup mdLis200Group
\author Shuvalov Sergey
\brief Функция записи данных.
*/
unsigned int WriteValue(StrDataUART *dataUART)
{
    unsigned char *buf_ptr;
    int index, adress, cnt, instans, mod, type;
    buf_ptr = (unsigned char *)strchr((char *)dataUART->bufRX, STX);
    
    if (!buf_ptr)
    {
        return Err_Telegramma;
    }
    
    buf_ptr++;
    cnt = sscanf((char *)buf_ptr, "%u", &instans);
    instansLis200 = instans;
    
    if (cnt != 1)
    {
        return Err_Telegramma;
    }
    
    buf_ptr = (unsigned char *)strchr((char *)dataUART->bufRX, ':');
    mod = 0;
    adress = 0;
    cnt = sscanf((char *) ++buf_ptr, "%x_%u", &adress, &mod);
    type = (adress / 0x1000);
    adress = adress % 0x1000;
    adressLis200 = adress;
    
    if (!adress)
    {
        return Err_Telegramma;
    }
    
    index = GetIndexIfAdressForTriggerEvents (adress, instans);
    
    if(index < 0)
    {
        index = GetIndexIfAdressForStatuses(adress, instans);
    }
    
    if(index < 0)
    {
        index = GetIndexIfAdressForResetArchive(adress, instans);
    }
    
    if(index < 0)
    {
        index = GetIndexIfAdressForCalib(adress, instans);
    }

    if(index < 0)
    {
        index = GetIndex(adress, instans, mod);
    }
    
    if (index < 0) //запрошенный индекс отсутсвует в таблице данных
    {
        return EC_AbsentValue;
    }
    
    buf_ptr = (unsigned char *)strchr((char *)dataUART->bufRX, ')');
    
    if (!buf_ptr)
    {
        return Err_Telegramma;
    }
    else
    {
        //заменим ненужную скобку концом строки длЯ удобства получениЯ значениЯ
        *buf_ptr = 0x00;
    }
    
    buf_ptr = (unsigned char *)strchr((char *)dataUART->bufRX, '(');
    
    if (!buf_ptr)
    {
        return Err_Telegramma;
    }
    else
    {
        buf_ptr++;
    }
    
    if (DataTable[index].TypCode != 0x171)
    {
        if (CheckLock(DataTable[index].AC_AccRight, Acc_Write))
        {
            return EC_19;    // lin_Tmp;
        }
    }
    
    switch (type)
    {
        uint8_t accessRights;
        
        case 0:
        switch (DataTable[index].AC_DataClass)
        {
            case DC_FunctS:
            case DC_Const :
            case DC_MessW :
            case DC_Status :
            case DC_CompCount :
            case DC_Marker :
            return EC_MessW;
            
            case DC_InitVal :
            return EC_19;
        } //switch (DataTable[i].AC_DataClass)
        
        if (DataTable[index].WrtFunc)
        {
            unsigned result =0;
            
            // сохраняем старое значение для архива изменений
            if(DataTable[index].RdFunc)
            {
                memset(changesArchive.oldParamValue, 0, 64);
                (*(DataTable[index].RdFunc))(index, (unsigned char *)changesArchive.oldParamValue, DataTable[index].AC_Value);
                char *ptr = (char *) strchr((const char *)changesArchive.oldParamValue, ETX);
                if(ptr)
                    *ptr = 0;
            }
            else
            {
                memset(changesArchive.oldParamValue, 0, 64);
            }
            
            //
            // прописываем новое значение
            result = (*DataTable[index].WrtFunc)(index, buf_ptr);
            
            //
            if(!result)
            {
                // сохраняем новое значение для архива изменений
                if(DataTable[index].RdFunc)
                {
                    memset(changesArchive.newParamValue, 0, 64);
                    (*DataTable[index].RdFunc)(index, (unsigned char *)changesArchive.newParamValue, DataTable[index].AC_Value);
                    char *ptr = (char *) strchr((const char *)changesArchive.newParamValue, ETX);
                    if(ptr)
                        *ptr = 0;
                }
                else
                {
                    memset(changesArchive.newParamValue, 0, 64);
                }
                // скрываем пароли в архиве изменений
                if((adress == 0x171) && ((instans==3)||(instans==4)))
                {
                    memset(changesArchive.oldParamValue, 0, 64);
                    memset(changesArchive.oldParamValue, '*', 3);
                    memset(changesArchive.newParamValue, 0, 64);
                    memset(changesArchive.newParamValue, '*', 3);
                }
                snprintf((char *)changesArchive.paramCode, 10, "%u:%x", instans, adress);
                changesArchive.stCalibLock = (bool)(accessStatus & 0x5);
                changesArchive.stSupplierLock = (bool)(accessStatus & 0x10);
                changesArchive.stConsumerLock = (bool)(accessStatus & 0x40);
                // инициируем режим работы с данными
                StrEventMode mode;
                mode.status = 1;
                mode.modeCode = DATA_M;
                xQueueSendToBack(xModesQueue, &mode, 0);
                // инициируем формирование\запись архива изменений
                StrDataManager forDataManager;
                forDataManager.event = 0;
                forDataManager.cmd = DM_CHANGE;
                xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
                // инициируем запись измененных параметров во флэш
                forDataManager.event = 0;
                forDataManager.cmd = DM_SAVE_PARAMS_IN_FLASH;
                xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
                // ожидаем пока параметры пропишутся во флэш                
                while(uxQueueSpacesAvailable(xFlashWriteQueue) != QUEUE_FLASH_SIZE)
                {
                    vTaskDelay(50);
                }
            }
            return result;
        }
        else
        {
            return NAK;
        }
        
        case 1: //Access Rights
        errno = 0;
        accessRights = (uint8_t)strtol((const char *)buf_ptr, 0, 0);
        
        if((!errno) || (!accessRights))
        {
            if(!(accessStatus & 0x15) &&
               ((DataTable[index].AC_AccRight & 0x3F) != (accessRights & 0x3F)))
            {
                //не даём потребителю изменить уровень доступа выше своего
                return EC_19;
            }
            
            // для поверителя и производителя доступны все возможности ( | 0xf )
            DataTable[index].AC_AccRight = accessRights | 0xf;
            return 0;
        }
        else
        {
            return EC_WrongValueRange;
        }
        
        case 2: //Name
        strncpy((char *)DataTable[index].AC_Name, (char *)buf_ptr, 8);
        // инициируем режим работы с данными
        StrEventMode mode;
        mode.status = 1;
        mode.modeCode = DATA_M;
        xQueueSendToBack(xModesQueue, &mode, 0);
        // инициируем запись измененных параметров во флэш
        StrDataManager forDataManager;
        forDataManager.event = 0;
        forDataManager.cmd = DM_SAVE_PARAMS_IN_FLASH;
        xQueueSendToBack(xDataManagerQueue, &forDataManager, NULL);
        // ожидаем пока параметры пропишутся во флэш                
        while(uxQueueSpacesAvailable(xFlashWriteQueue) != QUEUE_FLASH_SIZE)
        {
            vTaskDelay(50);
        }
        return 0;
        
        default :
        return EC_ROValue;
    }//switch (p_Attr)
}

