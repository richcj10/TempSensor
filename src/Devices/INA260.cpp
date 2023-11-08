#include "INA260.h"
#include <Adafruit_INA260.h>
#include "Com.h"

Adafruit_INA260 ina260 = Adafruit_INA260();

void INASetup(){
  if (!ina260.begin()) {
    Log(NOTIFY,"Couldn't find INA260 chip");
    while (1);
  }
  Log(DEBUG,"Found INA260 chip");
  // set the number of samples to average
  ina260.setAveragingCount(INA260_COUNT_16);
  // set the time over which to measure the current and bus voltage
  ina260.setVoltageConversionTime(INA260_TIME_140_us);
  ina260.setCurrentConversionTime(INA260_TIME_140_us);
}

void ReportTemp() {
  Serial.print("Current: ");
  Serial.print(ina260.readCurrent());
  Serial.println(" mA");

  Serial.print("Bus Voltage: ");
  Serial.print(ina260.readBusVoltage());
  Serial.println(" mV");

  Serial.print("Power: ");
  Serial.print(ina260.readPower());
  Serial.println(" mW");

  Serial.println();
  delay(1000);
}

float INACurrentRead(unsigned char i){
  Wire.beginTransmission(i); 
  //send a bit and ask for register zero
  Wire.write(0);
  //end transmission
  Wire.endTransmission();
  //request 1 byte from address xx
  Wire.requestFrom(i, 2);
  //wait for response
  while(Wire.available() == 0);
  //put the temperature in variable c
  int c = Wire.read();   
    float Value = ((x1 << 8) + x0);
    return Value*1.2;
}
