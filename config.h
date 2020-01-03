#pragma once

#include <Arduino.h>

//Set to the proper port for your USB connection - SerialUSB on Due (Native) or Serial for Due (Programming) or Teensy
#define SERIALCONSOLE   SerialUSB

//Define this to be the serial port the Tesla BMS modules are connected to.
//On the Due you need to use a USART port (Serial1, Serial2, Serial3) and update the call to serialSpecialInit if not Serial1
#define SERIAL  Serial1

#define EEPROM_VERSION      0x03    //update any time EEPROM struct below is changed.
#define EEPROM_PAGE         1

#define DOUT1            33
#define DOUT2            34
#define DOUT3            35
#define DOUT4            36
#define DOUT5            37
#define DOUT6            38
#define DOUT7            39
#define DOUT8            41
#define DOUT9            43
#define DOUT10           49
#define DOUT11           40
#define DOUT12           48


typedef struct {
    uint8_t version;
    uint8_t checksum;
    uint32_t canSpeed;
    uint8_t logLevel;
    uint16_t preDelay;
    uint16_t switchDelay;
} EEPROMSettings;
