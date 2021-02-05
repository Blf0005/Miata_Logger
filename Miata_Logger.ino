#if defined (__arm__) && defined (__SAM3X8E__)
#include <chip.h>
#endif

#include <Arduino.h>
#include "Logger.h"
#include "SerialConsole.h"
#include "SystemIO.h"
#include <due_can.h>
#include <due_wire.h>
#include <Wire_EEPROM.h>


#define CONTROLLER_BAUD  617647
EEPROMSettings settings;
SerialConsole console;
uint32_t lastUpdate;
uint32_t lastTick;
uint32_t tick;
int stepcount=0;

uint32_t slowTimer;
uint32_t mediumTimer;
uint32_t fastTimer;
#define aref_voltage 3.3  
#define MAXbrakePressure 0

#define coolantTempSensor 1
#define oilTempSensor 2
#define oilPressureSensor 3
#define fuelPressureSensor 4
#define brakePressureSensor 5
#define airFuelRatioSensor 6
#define throttlePositionSensor 11

uint32_t rpmTimer;

int tempReading;


//This is a character buffer that will store the data from the Serial3 port
char rxData[20];
char rxIndex=0;


uint8_t coolantTemp = 0;
uint8_t oilTemp = 0;
uint8_t oilPressure = 0;
uint8_t fuelPressure = 0;
uint16_t brakePressure = 0;
uint8_t brakePercentage = 0;
uint16_t afrVoltage = 0;
uint16_t AFR = 0;
uint8_t TPS = 0;




float tempTable[39][2] = {         
{ 4064.666667 , 0 } ,
{ 4052.928082 , 5 } ,
{ 4037.323944 , 10  } ,
{ 4016.751592 , 15  } ,
{ 3990  , 20  } ,
{ 3955.397727 , 25  } ,
{ 3912.459138 , 30  } ,
{ 3858.294798 , 35  } ,
{ 3791.666667 , 40  } ,
{ 3711.09375  , 45  } ,
{ 3613.235294 , 50  } ,
{ 3501.521739 , 55  } ,
{ 3372.352941 , 60  } ,
{ 3223.723404 , 65  } ,
{ 3062.647059 , 70  } ,
{ 2890.588235 , 75  } ,
{ 2698.977273 , 80  } ,
{ 2499.545455 , 85  } ,
{ 2314.565217 , 90  } ,
{ 2113.548387 , 95  } ,
{ 1939.736842 , 100 } ,
{ 1732.5  , 105 } ,
{ 1587.857143 , 110 } ,
{ 1365  , 115 } ,
{ 1238.023256 , 120 } ,
{ 1098.658537 , 125 } ,
{ 996.6393443 , 130 } ,
{ 884.0747517 , 135 } ,
{ 783.6792453 , 140 } ,
{ 694.7785774 , 145 } ,
{ 615.815633  , 150 } ,
{ 546.4731369 , 155 } ,
{ 484.9500441 , 160 } ,
{ 430.0238663 , 165 } ,
{ 382.3980054 , 170 } ,
{ 340.4156479 , 175 } ,
{ 304.5032397 , 180 } ,
{ 271.4705882 , 185 } ,
{ 242.6952023 , 190 } 
};         

//This code only applicable to Due to fixup lack of functionality in the arduino core.
#if defined (__arm__) && defined (__SAM3X8E__)
void serialSpecialInit(Usart *pUsart, uint32_t baudRate)
{
  // Reset and disable receiver and transmitter
  pUsart->US_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;

  // Configure mode
  pUsart->US_MR =  US_MR_CHRL_8_BIT | US_MR_NBSTOP_1_BIT | UART_MR_PAR_NO | US_MR_USART_MODE_NORMAL | 
                   US_MR_USCLKS_MCK | US_MR_CHMODE_NORMAL | US_MR_OVER; // | US_MR_INVDATA;

  //Get the integer divisor that can provide the baud rate
  int divisor = SystemCoreClock / baudRate;
  int error1 = abs(baudRate - (SystemCoreClock / divisor)); //find out how close that is to the real baud
  int error2 = abs(baudRate - (SystemCoreClock / (divisor + 1))); //see if bumping up one on the divisor makes it a better match

  if (error2 < error1) divisor++;   //If bumping by one yielded a closer rate then use that instead

  // Configure baudrate including the optional fractional divisor possible on USART
  pUsart->US_BRGR = (divisor >> 3) | ((divisor & 7) << 16);

  // Enable receiver and transmitter
  pUsart->US_CR = UART_CR_RXEN | UART_CR_TXEN;
}
#endif

void loadSettings()
{
    EEPROM.read(EEPROM_PAGE, settings);

    if (settings.version != EEPROM_VERSION) //if settings are not the current version then erase them and set defaults
    {
        Logger::console("Resetting to factory defaults");
        settings.version = EEPROM_VERSION;
        settings.canSpeed = 1000000;
        settings.logLevel = 2;
        EEPROM.write(EEPROM_PAGE, settings);
    }
    else {
        Logger::console("Using stored values from EEPROM");
    }
        
    Logger::setLoglevel((Logger::LogLevel)settings.logLevel);
}

void initializeCAN()
{
    uint32_t id;
    if(Can0.begin(1000000)){
      Logger::console("CAN0 initialization Successful.");
    }
    else{
      Logger::console("CAN0 initialization Failed.");
    }
   
    
    Logger::console("CAN initialization complete.");
}


      
void setup() 
{
    delay(4000);  //just for easy debugging. It takes a few seconds for USB to come up properly on most OS's
    SERIALCONSOLE.begin(115200);
    SERIALCONSOLE.println("Starting up!");
    SERIAL.begin(CONTROLLER_BAUD);
    Serial3.begin(10400);
#if defined (__arm__) && defined (__SAM3X8E__)
    serialSpecialInit(USART0, CONTROLLER_BAUD); //required for Due based boards as the stock core files don't support 612500 baud.
#endif
    initializeCAN();
    loadSettings();
    systemIO.setup(); 
    slowTimer = 0;
    mediumTimer = 0;
    fastTimer = 0;
    rpmTimer = 0;
}

void loop() 
{   
    console.loop();

    if (millis() - mediumTimer > 40){ //25Hz
      getMediumData();
      sendDataPacket2();
      mediumTimer = millis();
    }

    if (millis() - fastTimer > 20){ //50Hz
      getFastData();
      sendDataPacket1();
      fastTimer = millis();
    }

    if (millis() - slowTimer > 1000){ //1Hz
      getSlowData();
      slowTimer = millis();
      if(settings.logLevel < 2){
        printData();
      }
    }
    
    
}

float lookupTemp(float table[][2], float ADC_Reading){
  int i;
  float m;
  i = 0;
  while((i < 39) && (ADC_Reading < table[i][0]) ){  //find the two points in the table to use
    i++;
  }
  if ( i == 39 ){   //make sure the point isn't past the end of the table
    return table[i-1][1];
  }

  if ( i == 0 ){  //make sure the point isn't before the beginning of the table
    return table[i][1];
  }


  m = (float)(table[i][1] - table[i-1][1]) / (float)( table[i][0] - table[i-1][0]); //calculate the slope
  return m * (ADC_Reading - table[i][0]) + table[i][1]; //this is the solution to the point slope formula
}

uint8_t getTemp(int sensor){
  analogReadResolution(12);
  int tempReading = systemIO.readAnalogInput(sensor);
  Logger::debug("RAW Temp Reading: %i\n", tempReading);
  float temp = lookupTemp(tempTable,(float)tempReading);
  return temp;
}

uint8_t getPressure(int sensor){
  analogReadResolution(12);
  int pressureReading = systemIO.readAnalogInput(sensor);
 // Logger::debug("RAW Pressure Reading: %i\n", pressureReading);
  //float pressureVoltage = (pressureReading* 5.0)/4095.0;
  return map(pressureReading, 409, 3686, 0, 100);

}

uint8_t getTPS(int sensor){
  analogReadResolution(12);
  int tpsReading = systemIO.readAnalogInput(sensor);
  //Logger::debug("RAW Pressure Reading: %i\n", tpsReading);
  return map(tpsReading, 500, 2900, 0, 100);

}

uint16_t getBrakePressure(int sensor){
  analogReadResolution(12);
  int pressureReading = systemIO.readAnalogInput(sensor);
  //Logger::debug("RAW Pressure Reading: %i\n", pressureReading);
  return map(pressureReading, 400, 3685, 0, 2000);
}

uint16_t getAFR(int sensor){
  analogReadResolution(12);
  int afrReading = systemIO.readAnalogInput(sensor);
  //Logger::debug("RAW AFR Reading: %i\n", afrReading);
  //float afrVoltage = (afrReading* 5.0)/4095.0;
  return map(afrReading, 0, 4095, 735, 2239);
}

void sendDataPacket1()
{
  CAN_FRAME outgoing;
  outgoing.length = 8;
  outgoing.id = 0x250;
  outgoing.extended = 0; //standard frame
  outgoing.rtr = 0;
  outgoing.fid = 0;
  
  
  outgoing.data.byte[0] = lowByte(AFR);
  outgoing.data.byte[1] = highByte(AFR);
  outgoing.data.byte[2] = oilPressure; //Oil Pressure in %
  outgoing.data.byte[3] = lowByte(brakePressure); // Brake Pressure in PSI
  outgoing.data.byte[4] = highByte(brakePressure); // Brake Pressure in PSI
  outgoing.data.byte[5] = brakePercentage; //Brake Percentage
  outgoing.data.byte[6] = fuelPressure; //fuel pressure in %
  outgoing.data.byte[7] = TPS; //Throttle position in %
  Can0.sendFrame(outgoing);
}

void sendDataPacket2()
{
  CAN_FRAME outgoing;
  outgoing.length = 8;
  outgoing.id = 0x251;
  outgoing.extended = 0; //standard frame
  outgoing.rtr = 0;
  outgoing.fid = 0;
  
  outgoing.data.byte[0] = coolantTemp; //coolant Temperature in C
  outgoing.data.byte[1] = oilTemp; //oil Temperature in C
  outgoing.data.byte[2] = 0; 
  outgoing.data.byte[3] = 0; 
  outgoing.data.byte[4] = 0; 
  outgoing.data.byte[5] = 0; 
  outgoing.data.byte[6] = 0; 
  outgoing.data.byte[7] = 0; 
  Can0.sendFrame(outgoing);
}


void getTemps(){
  coolantTemp = (uint8_t)(getTemp(coolantTempSensor))-40;
    if(coolantTemp < 0){
    coolantTemp = 0;
  }
  Logger::debug("Coolant Temp Reading: %i\n", coolantTemp);
  oilTemp = (uint8_t)(getTemp(oilTempSensor))-40;
  if(oilTemp < 0){
    oilTemp = 0;
  }
  Logger::debug("Oil Temp Reading: %i\n", oilTemp);
}

void getDriverInputs(){
 brakePressure = (uint16_t)(getBrakePressure(brakePressureSensor));
 if(brakePressure <20 || brakePressure >2000){
    brakePressure=0;
  }
  if(brakePressure >2000 && brakePressure < 2500){
    brakePressure=2000;
  }
  Logger::debug("brake pressure / 1300.0: %i\n", (int)( brakePressure/1300.0*100));
  brakePercentage = (int) ((brakePressure/1300.0)*100);
  TPS = (uint8_t)(getTPS(throttlePositionSensor));
  if(TPS <0 || TPS >150){
    TPS=0;
  }
  if(TPS >100 && TPS < 150){
    TPS=100;
  }
}

void getPressures(){
  fuelPressure = (uint8_t)(getPressure(fuelPressureSensor));
  if(fuelPressure < 3 || fuelPressure >150){
    fuelPressure=0;
  }
  oilPressure = (uint8_t)(getPressure(oilPressureSensor));
   if(oilPressure < 3 || oilPressure >150){
    oilPressure=0;
  }

}


void getSlowData(){
  getTemps();
}

void getMediumData(){
  getPressures();
}

void getFastData(){
  getDriverInputs();
  AFR = getAFR(airFuelRatioSensor);
}

void printData(){
  Logger::console("\n\n====================================== Miata Sensor Array ======================================\n");
  Logger::console("Coolant Temperature: %iF\n", (((coolantTemp)*9)/5)+32);
  Logger::console("Oil Temperature: %iF\n", (((oilTemp)*9)/5)+32);
  Logger::console("Oil Pressure: %iPSI\n", oilPressure);
  Logger::console("Fuel Pressure: %iPSI\n", fuelPressure);
  Logger::console("Brake Pressure: %iPSI\n", brakePressure);
  Logger::console("Brake Percent: %i%%\n", brakePercentage);
  Logger::console("Air fuel Ratio: %f\n", AFR/100.0);
  Logger::console("Throttle Position: %i\n", TPS);
  
  
}
