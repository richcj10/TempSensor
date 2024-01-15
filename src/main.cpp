#include <Arduino.h>
#include "ModBusController.h"
#include "EEPROMData.h"
#include "Sensor.h"
#include "Define.h"

unsigned long previousMillis = 0;
unsigned long previousLEDMillis = 0;
unsigned int SensorTestTime = 3;

void ComunicationUpdate(){
/*   ModbusRTUServer.poll();
  long holdingRegisterValue = ModbusRTUServer.holdingRegisterRead(0);
  if (holdingRegisterValue == SENSOR_CHANGE_ADDRESS) {
    //NewAddress
    SetAddressFromEEPROM((char)ModbusRTUServer.holdingRegisterRead(1));
  }
  else if (holdingRegisterValue == SENSOR_CHANGE_SCANRATE) {
    //NewScanRate
    SetScanRateFromEEPROM((char)ModbusRTUServer.holdingRegisterRead(1));
  }
  else if (holdingRegisterValue == SENSOR_CHANGE_FAULT) {
     SetSensorWaterDetect((int)ModbusRTUServer.holdingRegisterRead(1));
  }
  else if (holdingRegisterValue == SENSOR_CHANGE_SENSOR_OPEN) {
    SensorWaterDisconected((int)ModbusRTUServer.holdingRegisterRead(1));
  } */
}

void setup() {
  Serial.begin(38400);

  pinMode(DIR,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(SENSORPWR,OUTPUT);
  pinMode(RELAY,OUTPUT);
  digitalWrite(DIR,LOW);
  digitalWrite(LED,LOW);
  digitalWrite(SENSORPWR,LOW);
  digitalWrite(RELAY,LOW);

  SetCoilValues(SENSOR_SCAN_EN,1);
  SetCoilValues(SENSOR_LED_CONTROL,1);
  SetCoilValues(AUTO_SENSOR_RELAY_CONTROL,1);
  SetCoilValues(SENSOR_RELAY,1);

  char ScanRate = GetScanRateFromEEPROM();
  if(ScanRate != -1){
    SensorTestTime = ScanRate;
  }
  else{
    SensorTestTime = SENSORSCANRATE;
  }
  char k = 0;
  for(k=0;k<10;k++){
    SampleVin();
  }
  ModbusSetup();
}

void loop() {
  ModBusLoop();
  ComunicationUpdate();
  unsigned long currentMillis = millis();
  if(GetCoilValues(SENSOR_SCAN_EN)){
    if (currentMillis - previousMillis >= SensorTestTime*SENSORTESTTIMEMULTIPLYER) {
      previousMillis = currentMillis;
      ReadSensors();
      SampleVin();
    }
  }
  if(GetCoilValues(SENSOR_LED_CONTROL)){
    if (currentMillis - previousLEDMillis >= 500) {
      previousLEDMillis = currentMillis;
      digitalWrite(LED,!digitalRead(LED));
    }
  }
/*   if(ModbusRTUServer.coilRead(AUTO_SENSOR_RELAY_CONTROL) == 0){
    digitalWrite(RELAY, ModbusRTUServer.coilRead(SENSOR_RELAY));
  } */
}
