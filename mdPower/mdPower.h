#ifndef mdPower
#define mdPower

#include "stm32l1xx.h"
#include "stm32l1xx_lcd.h"

void LowPowerON(void);
void LowPowerOFF(void);
void controlPower(void);
void setMaskForLPM(GPIO_TypeDef const * const GPIOx, uint32_t pin);
void unsetMaskForLPM(GPIO_TypeDef const * const GPIOx, uint32_t pin);

void initRequestBat(void);
void setRequestBat(void);
void resetRequestBat(void);
void setNewBat(void);

enum {BAT_A, BAT_B};
float getVoltageBat(int numBat);

extern uint8_t activeBat;
#endif