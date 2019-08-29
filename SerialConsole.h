/*
 * SerialConsole.h
 *
Copyright (c) 2019 PREMERGY,INC / BLAKELY FABIANI
*/

#ifndef SERIALCONSOLE_H_
#define SERIALCONSOLE_H_

#include "config.h"

class SerialConsole {
public:
    SerialConsole();
    void loop();
    void printMenu();

protected:
    enum CONSOLE_STATE
    {
        STATE_ROOT_MENU
    };

private:
    char cmdBuffer[80];
    int ptrBuffer;
    int state;
    int loopcount;
    bool cancel;


    void init();
    void serialEvent();
    void handleConsoleCmd();
    void handleShortCmd();
    void handleConfigCmd();
};

#endif /* SERIALCONSOLE_H_ */


