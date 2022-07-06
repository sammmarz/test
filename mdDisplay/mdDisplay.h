#ifndef mdDisplay
#define mdDisplay

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

enum {CENTER_BTN = 1<<0, LEFT_BTN = 1<<1, RIGHT_BTN = 1<<2, UPDATE_DISPLAY = 0x8000};

typedef struct
{
    int codeEventForDisplay;
    int status; //1 - нажата 0 - отпустили
} StrEventForDisplay;

extern QueueHandle_t xBtnsQueue;
extern TaskHandle_t xTaskDisplayDriver;
extern bool displayActive;

void DisplayDriver(void *arg);
void initUserButtons(void);
void initDisplay(void);
void enableDisplayPower(void);

#endif
