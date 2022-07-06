#ifndef DisplayHardware
#define DisplayHardware

#include <stdbool.h>
#include "stm32l1xx.h"


enum {DISPLAY_DATA = 0, DISPLAY_CMD = 1};
void sendByteToDisplay(uint8_t data, bool cmd);
void disableDisplayPower(void);

#endif