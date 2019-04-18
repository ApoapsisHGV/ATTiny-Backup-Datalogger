// This Code is used for connecting an ATTiny85 Microcontroller
// to a BMP280 or BME280 Pressure / Temperature / (Humidity) Sensor
// and a 24 Series EEPROM. It measures Pressure and Temperature and
// stores it in the EEPROM.

// This Code is based on a modified version of TinyBME280 Library found here - https://github.com/technoblogy/tiny-bme280
// It has to be modifief to use TinyWireM instead of the regular Wire library

// TODO: Everytime the ATTiny reaches the end of the EEPROM it overflows and starts to write it from the beginning
// erasing previously taken measurements. Implement a stop for that.

// TODO: Add deepsleep after EEPROM is full

#include <TinyWireM.h>
#include <TinyBME280.h>

//Set EEPROM Address
#define eeprom_address 0x50

//Max. EEPROM Byte size
int max_byte = 0x7ff;
int current_byte = 0x000;
unsigned addr = 0x000;



void setup() {

  //Disable ADC (for power savings)
  ADCSRA = 0;

  //Initalize Communications
  TinyWireM.begin();
  BME280setup();
 
}

void loop() {

  //Create Array for Measurements
  byte temperature_bytes[4];
  byte pressure_bytes[4];

  //Take Measurements
  int32_t temperature = BME280temperature();
  int32_t pressure = BME280pressure();  

  //Split each measurement into 4 Bytes
  temperature_bytes[0] = temperature & 255;
  temperature_bytes[1] = (temperature >> 8) & 255;
  temperature_bytes[2] = (temperature >> 16) & 255;
  temperature_bytes[3] = (temperature >> 32) & 255;

  pressure_bytes[0] = pressure & 255;
  pressure_bytes[1] = (pressure >> 8) & 255;
  pressure_bytes[2] = (pressure >> 16) & 255;
  pressure_bytes[3] = (pressure >> 32) & 255;
  

  //Write values to EEPROM

  //Get the  actual device address based of the data address
  byte device_address = eeprom_address | ((addr >> 8) & 0x07);  

  //Store the measured values as a block on the eeprom
  TinyWireM.beginTransmission(device_address);
  TinyWireM.write(int(addr));
  TinyWireM.write(int(temperature_bytes[0]));
  TinyWireM.write(int(temperature_bytes[1]));
  TinyWireM.write(int(temperature_bytes[2]));
  TinyWireM.write(int(temperature_bytes[3]));
  TinyWireM.write(int(pressure_bytes[0]));
  TinyWireM.write(int(pressure_bytes[1]));
  TinyWireM.write(int(pressure_bytes[2]));
  TinyWireM.write(int(pressure_bytes[3]));
  TinyWireM.endTransmission();
   
  addr += 8;
  delay(50);
}

