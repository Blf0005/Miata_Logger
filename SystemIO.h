#pragma once

enum OUTPUTSTATE {
    FLOATING = 0,
    ON = 1,
    OFF = 2
};

class SystemIO
{
public:
    SystemIO();
    void setup();
    bool readInput(int pin);
    void setOutput(int pin, OUTPUTSTATE state);
    int readAnalogInput(int pin);
    
private:

};

extern SystemIO systemIO;
