#pragma once

#include <Arduino.h>

//Set to the proper port for your USB connection - SerialUSB on Due (Native) or Serial for Due (Programming) or Teensy
#define SERIALCONSOLE   SerialUSB

//Define this to be the serial port the Tesla BMS modules are connected to.
//On the Due you need to use a USART port (Serial1, Serial2, Serial3) and update the call to serialSpecialInit if not Serial1
#define SERIAL  Serial1

#define EEPROM_VERSION      0x01    //update any time EEPROM struct below is changed.
#define EEPROM_PAGE         1

#define DOUT1            33
#define DOUT2            34
#define DOUT3            35
#define DOUT4            36

#define DIN1            22
#define DIN2            23
#define DIN3            24
#define DIN4            25


#define AIN1           A0
#define AIN2           A1
#define AIN3           A2
#define AIN4           A3
#define AIN5           A4
#define AIN6           A5
#define AIN7           A6
#define AIN8           A7
#define AIN9           A8
#define AIN10          A9
#define AIN11          A10
#define AIN12          A11


typedef struct {
    uint8_t version;
    uint32_t canSpeed;
    uint8_t logLevel;
} EEPROMSettings;
