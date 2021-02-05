//
// AEM_Temp_Sensor.h
// Created by Blakely Fabiani on 5/5/20.

#ifndef _AEM_Temp_Sensor_h_
#define _AEM_Temp_Sensor_h_

#include "config.h"

class AEM_Temp_Sensor
{
public:
	AEM_Temp_Sensor();
	void setup();
	uint8_t getTemp(int pin);
	
private:

};


#endif