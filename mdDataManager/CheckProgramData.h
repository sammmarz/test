#ifndef CheckProgramData_h
#define CheckProgramData_h

#define PROG_LEN_OK (1<<0)              /*Размер файла прошива в допуске */
#define PROG_IN_FLASH_OK (1<<1)         /* Файл прошива записан/не записан во флеш */
#define PROG_CRC_OK (1<<2)              /* Целостность файла обеспечена/не обеспечена */ 
#define PROG_DEVICE_OK (1<<3)           /* ID счетчика соответствует/ не соответствует устройству */
#define PROG_VERBOARD_OK (1<<4)         /* Активен процесс проверки прошива  */
#define PROG_ANALYSE_PROCESS (1<<5)     /* Активен процесс проверки прошива  */
#define PROG_CRCSTR_OK (1<<6)           /* Ошибка CRC структуры */
#define PROG_CRCMETR_OK (1<<7)          /* Ошибка CRC метрологической части */
#define PROG_CRCMAIN_OK (1<<8)          /* Ошибка CRC неметрологической части */

#endif