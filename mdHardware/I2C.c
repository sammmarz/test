/**
 \file
Файл настройки I2C.
*/

#include "mdHardware.h"
/**
 \ingroup mdHardwareGroup
 \author Shuvalov Sergey
 \brief Функция инициализации I2C_1 (связь с термодатчиком)
*/
void initI2cForTemperSensor(void)
{
    I2C_InitTypeDef i2c;
    // Включаем тактирование нужных модулей
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    i2c.I2C_ClockSpeed = 100000;//400000;//
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    // random adress
    i2c.I2C_OwnAddress1 = 0x10;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &i2c);
    //
    I2C_Cmd(I2C1, ENABLE);
}
