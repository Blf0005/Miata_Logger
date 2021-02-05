#include <Arduino.h>
#include "config.h"
#include "Logger.h"
#include "SystemIO.h"

int DigitalInputs[4] = {DIN1, DIN2, DIN3, DIN4};
int DigitalOutputs[4] = { DOUT1, DOUT2, DOUT3, DOUT4};
int AnalogInputs[12] = {AIN1, AIN2, AIN3, AIN4, AIN5, AIN6, AIN7, AIN8, AIN9, AIN10, AIN11, AIN12};

SystemIO::SystemIO()
{
    
}

void SystemIO::setup()
{

  //Setup Digintal Outputs
    pinMode(33, OUTPUT);
    pinMode(34, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(36, OUTPUT);

    digitalWrite(33, HIGH);
    digitalWrite(34, HIGH);
    digitalWrite(35, HIGH);
    digitalWrite(36, HIGH);

   //Setup Digital Inputs
    pinMode(22, INPUT);
    pinMode(23, INPUT);
    pinMode(24, INPUT);
    pinMode(25, INPUT);

   //Setup Analog Inputs
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);
    pinMode(A4, INPUT);
    pinMode(A5, INPUT);
    pinMode(A6, INPUT);
    pinMode(A7, INPUT);
    pinMode(A8, INPUT);
    pinMode(A9, INPUT);
    pinMode(A10, INPUT);
    pinMode(A11, INPUT);



}

bool SystemIO::readInput(int pin)
{
    if (pin < 0 || pin > 3) return false;
    return !digitalRead(DigitalInputs[pin]);
}

void SystemIO::setOutput(int pin, OUTPUTSTATE state)
{
    if (pin < 0 || pin > 3) return;
    delayMicroseconds(10); //give mosfets some time to turn off
    if (state == ON) digitalWrite(DigitalOutputs[pin], LOW);
    if (state == OFF) digitalWrite(DigitalOutputs[pin], HIGH);
}

int SystemIO::readAnalogInput(int pin)
{
   if (pin < 1 || pin > 12) return 0;
   analogReadResolution(12);
    return analogRead(AnalogInputs[pin-1]);
}

SystemIO systemIO;
