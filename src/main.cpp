#include <Arduino.h>
#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

#include "EEPROMData.h"
#include "Sensor.h"
#include "Define.h"

unsigned long previousMillis = 0;
unsigned long previousLEDMillis = 0;
unsigned int SensorTestTime = 3;

void ComunicationUpdate(){
  ModbusRTUServer.poll();
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
  }

  ModbusRTUServer.inputRegisterWrite(2, GetSensorValues(SENSOR_INPUT_VIN)*100);
  ModbusRTUServer.inputRegisterWrite(3, ReadSensorValues(SENSOR_BIAS));
  ModbusRTUServer.inputRegisterWrite(4, ReadSensorValues(SENSOR_INPUT_1));
  ModbusRTUServer.inputRegisterWrite(5, ReadSensorValues(SENSOR_INPUT_2));
  ModbusRTUServer.inputRegisterWrite(6, ReadSensorValues(SENSOR_INPUT_3));

  ModbusRTUServer.coilWrite(SENSOR_RELAY, ReadRelayState());
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

  char Adr = GetAddressFromEEPROM();
  if(Adr == -1){
    if (!ModbusRTUServer.begin(LEAKSENSORADDRESS, 38400)) {
      Serial.println("Failed to start Modbus RTU Server!");
      while (1);
    }
  }
  else{
    if (!ModbusRTUServer.begin(Adr, 38400)) {
      Serial.println("Failed to start Modbus RTU Server!");
      while (1);
    }
  }

  // configure a single coil at address 0x00
  ModbusRTUServer.configureCoils(0x01, 4); //Relay Control [Sensor "scan" Enable,Enable blinking LED,Enable Auto Relay Control, Relay]
  ModbusRTUServer.configureInputRegisters(0x01, 6);  //Leak Sensor Values [SensorType,BiasVoltage Test,S1 Test,S2 Test,S3 Test]
  ModbusRTUServer.configureHoldingRegisters(0x01, 3); //Control I/O 
  ModbusRTUServer.inputRegisterWrite(1, LEAKSENSORTYPE);// Add Sensor Type, this is fixed. 
  ModbusRTUServer.coilWrite(SENSOR_SCAN_EN, 1); //Sensor Scan Enable - Default ON
  ModbusRTUServer.coilWrite(SENSOR_LED_CONTROL, 1); //LED Control - Default ON
  ModbusRTUServer.coilWrite(AUTO_SENSOR_RELAY_CONTROL, 1); //Auto Relay Control - Default ON
  ModbusRTUServer.coilWrite(SENSOR_RELAY, 0); //Relay Control - Default OFF

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
}

void loop() {
  ComunicationUpdate();
  unsigned long currentMillis = millis();
  if(ModbusRTUServer.coilRead(SENSOR_SCAN_EN)){
    if (currentMillis - previousMillis >= SensorTestTime*SENSORTESTTIMEMULTIPLYER) {
      previousMillis = currentMillis;
      ReadSensors();
      SampleVin();
      if(ModbusRTUServer.coilRead(AUTO_SENSOR_RELAY_CONTROL)){
        RelayCheck();
      }
    }
  }
  if(ModbusRTUServer.coilRead(SENSOR_LED_CONTROL)){
    if (currentMillis - previousLEDMillis >= 500) {
      previousLEDMillis = currentMillis;
      digitalWrite(LED,!digitalRead(LED));
    }
  }
  if(ModbusRTUServer.coilRead(AUTO_SENSOR_RELAY_CONTROL) == 0){
    digitalWrite(RELAY, ModbusRTUServer.coilRead(SENSOR_RELAY));
  }
}
