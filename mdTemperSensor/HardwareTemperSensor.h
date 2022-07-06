#ifndef HardwareTemperSensor_h
#define HardwareTemperSensor_h
//registers
#define     TMP117_TemperatureRegister     0x00
#define     TMP117_ConfigurationRegister   0x01
#define     TMP117_TemperatureHighLimit    0x02
#define     TMP117_TemperatureLowLimit     0x03

#define     TMP117_EEPROM_Uclock           0x04
#define     TMP117_EEPROM1                 0x05
#define     TMP117_EEPROM2                 0x06
#define     TMP117_EEPROM3                 0x08

#define     TMP117_Temperature_Offset      0x07
#define     TMP117_ID_Register             0x0F
//
#define EEPROM_UNLOCK_BIT 0x8000
#define EEPROM_BUSY_BIT 0x4000
#define TMP117_DATA_READY 0x2000
int sendToTemperSensor(uint8_t register_, uint16_t data);
uint16_t receiveFromTemperSensor(uint8_t register_, uint16_t *dataTemp);
#endif