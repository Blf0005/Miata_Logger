#include <Arduino.h>
#include "config.h"
#include "Logger.h"
#include "SystemIO.h"

//int DigitalInputs[4] = {DIN1, DIN2, DIN3, DIN4};
int DigitalOutputs[10] = { DOUT1, DOUT2, DOUT3, DOUT4, DOUT5, DOUT6, DOUT7, DOUT8, DOUT9, DOUT10 };

SystemIO::SystemIO()
{
    
}

void SystemIO::setup()
{
    pinMode(33, OUTPUT);
    pinMode(34, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(36, OUTPUT);
    pinMode(37, OUTPUT);
    pinMode(38, OUTPUT);
    pinMode(39, OUTPUT);
    pinMode(41, OUTPUT);
    pinMode(43, OUTPUT);
    pinMode(49, OUTPUT);
    digitalWrite(33, HIGH);
    digitalWrite(34, HIGH);
    digitalWrite(35, HIGH);
    digitalWrite(36, HIGH);
    digitalWrite(37, HIGH);
    digitalWrite(38, HIGH);
    digitalWrite(39, HIGH);
    digitalWrite(41, HIGH);
    digitalWrite(43, HIGH);
    digitalWrite(49, HIGH);
}

bool SystemIO::readInput(int pin)
{
//    if (pin < 0 || pin > 3) return false;
//    return !digitalRead(DigitalInputs[pin]);
}

void SystemIO::setOutput(int pin, OUTPUTSTATE state)
{
    if (pin < 0 || pin > 9) return;
    delayMicroseconds(10); //give mosfets some time to turn off
    if (state == ON) digitalWrite(DigitalOutputs[pin], LOW);
    if (state == OFF) digitalWrite(DigitalOutputs[pin], HIGH);
}

SystemIO systemIO;
