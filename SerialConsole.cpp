/*
 * SerialConsole.cpp
 *
 Copyright (c) 2019 PREMERGY,INC / BLAKELY FABIANI
 */

#include <due_wire.h>
#include <Wire_EEPROM.h>
#include "SerialConsole.h"
#include "Logger.h"

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } //Lets us stream SerialUSB

extern EEPROMSettings settings;

SerialConsole::SerialConsole() {
    init();
}

void SerialConsole::init() {
    //State variables for serial console
    ptrBuffer = 0;
    state = STATE_ROOT_MENU;
    loopcount=0;
    cancel=false;
}

void SerialConsole::loop() {  
    if (SERIALCONSOLE.available()) {
        serialEvent();
    }
}

void SerialConsole::printMenu() {   
    Logger::console("\n*************SYSTEM MENU *****************");
    Logger::console("Enable line endings of some sort (LF, CR, CRLF)");
    Logger::console("Most commands case sensitive\n");
    Logger::console("GENERAL SYSTEM CONFIGURATION\n");

    Logger::console("   LOGLEVEL=%i - set log level (0=debug, 1=info, 2=warn, 3=error, 4=off)", Logger::getLogLevel());
    Logger::console("   CANSPEED=%i - set first CAN bus speed", settings.canSpeed);

    Logger::console("\nSWITCHBOX CONFIGURATION CONTROLS\n");
    Logger::console("PIN=%i - Toggle contactor on and off (0, 1, 2, 3, 4, 5, 6, 7, 8, 9)");

}

/*	There is a help menu (press H or h or ?)

    Commands are submitted by sending line ending (LF, CR, or both)
 */
void SerialConsole::serialEvent() {
    int incoming;
    incoming = SERIALCONSOLE.read();
    if (incoming == -1) { //false alarm....
        return;
    }

    if (incoming == 10 || incoming == 13) { //command done. Parse it.
        handleConsoleCmd();
        ptrBuffer = 0; //reset line counter once the line has been processed
    } else {
        cmdBuffer[ptrBuffer++] = (unsigned char) incoming;
        if (ptrBuffer > 79)
            ptrBuffer = 79;
    }
}

void SerialConsole::handleConsoleCmd() {

    if (state == STATE_ROOT_MENU) {
        if (ptrBuffer == 1) { //command is a single ascii character
            handleShortCmd();
        } else { //if cmd over 1 char then assume (for now) that it is a config line
            handleConfigCmd();
        }
    }
}

/*For simplicity the configuration setting code uses four characters for each configuration choice. This makes things easier for
 comparison purposes.
 */
void SerialConsole::handleConfigCmd() {
    int i;
    int newValue;
    float newFloat;
    bool needEEPROMWrite = false;

    //Logger::debug("Cmd size: %i", ptrBuffer);
    if (ptrBuffer < 6)
        return; //4 digit command, =, value is at least 6 characters
    cmdBuffer[ptrBuffer] = 0; //make sure to null terminate
    String cmdString = String();
    unsigned char whichEntry = '0';
    i = 0;

    while (cmdBuffer[i] != '=' && i < ptrBuffer) {
        cmdString.concat(String(cmdBuffer[i++]));
    }
    i++; //skip the =
    if (i >= ptrBuffer)
    {
        Logger::console("Command needs a value..ie TORQ=3000");
        Logger::console("");
        return; //or, we could use this to display the parameter instead of setting
    }

    // strtol() is able to parse also hex values (e.g. a string "0xCAFE"), useful for enable/disable by device id
    newValue = strtol((char *) (cmdBuffer + i), NULL, 0);
    newFloat = strtof((char *) (cmdBuffer + i), NULL);

    cmdString.toUpperCase();

    if (cmdString == String("CANSPEED")) {
        if (newValue >= 33000 && newValue <= 1000000) {
            settings.canSpeed = newValue;
            Logger::console("Setting CAN speed to %i", newValue);
            needEEPROMWrite = true;
        }
        else Logger::console("Invalid speed. Enter a value between 33000 and 1000000");
    } else if (cmdString == String("LOGLEVEL")) {
        switch (newValue) {
        case 0:
            Logger::setLoglevel(Logger::Debug);
            settings.logLevel = 0;
            Logger::console("setting loglevel to 'debug'");
            break;
        case 1:
            Logger::setLoglevel(Logger::Info);
            settings.logLevel = 1;
            Logger::console("setting loglevel to 'info'");
            break;
        case 2:
            Logger::console("setting loglevel to 'warning'");
            settings.logLevel = 2;
            Logger::setLoglevel(Logger::Warn);
            break;
        case 3:
            Logger::console("setting loglevel to 'error'");
            settings.logLevel = 3;
            Logger::setLoglevel(Logger::Error);
            break;
        case 4:
            Logger::console("setting loglevel to 'off'");
            settings.logLevel = 4;
            Logger::setLoglevel(Logger::Off);
            break;
        } 
        needEEPROMWrite = true;
    } else if (cmdString == String("PIN")) {
        switch (newValue) {
          case 33: 
                  digitalWrite(33, LOW);
                  delay(750);
                  digitalWrite(33, HIGH);
                  break;
          case 34:
                  digitalWrite(34, LOW);
                  delay(750);
                  digitalWrite(34, HIGH);
                  break;
          case 35:
                  digitalWrite(35, LOW);
                  delay(750);
                  digitalWrite(35, HIGH);
                  break;
          case 36:
                  digitalWrite(36, LOW);
                  delay(750);
                  digitalWrite(36, HIGH);
                  break;
          case 37:
                  digitalWrite(37, LOW);
                  delay(750);
                  digitalWrite(37, HIGH);
                  break;
          case 38:
                  digitalWrite(38, LOW);
                  delay(750);
                  digitalWrite(38, HIGH);
                  break;
          case 39:
                  digitalWrite(39, LOW);
                  delay(750);
                  digitalWrite(39, HIGH);
                  break;
          case 41:
                  digitalWrite(41, LOW);
                  delay(750);
                  digitalWrite(41, HIGH);
                  break;
          case 43:
                  digitalWrite(43, LOW);
                  delay(750);
                  digitalWrite(43, HIGH);
                  break;
          case 49:
                  digitalWrite(49, LOW);
                  delay(750);
                  digitalWrite(49, HIGH);
                  break;
          default: 
                  break;
          
        }       
    } else {
        Logger::console("Unknown command");
    }
    if (needEEPROMWrite)
    {
        EEPROM.write(EEPROM_PAGE, settings);
    }
}

void SerialConsole::handleShortCmd() {
    uint8_t val;

    switch (cmdBuffer[0]) {
    case 'h':
    case '?':
    case 'H':
        printMenu();
        break;
    default:
        Logger::console("Unknown command");
        break;
    }
}

