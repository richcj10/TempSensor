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
  else{
    //Do Nothing, Wrong Config
    digitalWrite(LED,!digitalRead(LED));
    delay(50);
    digitalWrite(LED,!digitalRead(LED));
    delay(50);
    digitalWrite(LED,!digitalRead(LED));
    delay(50);
    digitalWrite(LED,!digitalRead(LED));
    delay(50);
  }
  ModbusRTUServer.inputRegisterWrite(1, ReadSensorValues(SENSOR_BIAS));
  ModbusRTUServer.inputRegisterWrite(2, ReadSensorValues(SENSOR_INPUT_1));
  ModbusRTUServer.inputRegisterWrite(3, ReadSensorValues(SENSOR_INPUT_2));
  ModbusRTUServer.inputRegisterWrite(4, ReadSensorValues(SENSOR_INPUT_3));
  ModbusRTUServer.coilWrite(SENSOR_RELAY, ReadRelayState());
}

void setup() {
  //SetAddressFromEEPROM(LEAKSENSORADDRESS);
  //SetScanRateFromEEPROM(3);
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
/*   if (!ModbusRTUServer.begin(RS485Class(Serial, 1, 4,0), 42, 38400)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  } */


  // configure a single coil at address 0x00
  ModbusRTUServer.configureCoils(0x00, 4); //Relay Control [Sensor "scan" Enable,Enable blinking LED,Enable Auto Relay Control, Relay]
  ModbusRTUServer.configureInputRegisters(0x00, 5);  //Leak Sensor Values [SensorType,BiasVoltage Test,S1 Test,S2 Test,S3 Test]
  ModbusRTUServer.configureHoldingRegisters(0x00, 2); //Control I/O 
  ModbusRTUServer.inputRegisterWrite(0, LEAKSENSORTYPE);// Add Sensor Type, this is fixed. 
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
}

void loop() {
  ComunicationUpdate();
  unsigned long currentMillis = millis();
  if(ModbusRTUServer.coilRead(SENSOR_SCAN_EN)){
    if (currentMillis - previousMillis >= SensorTestTime*SENSORTESTTIMEMULTIPLYER) {
      previousMillis = currentMillis;
      ReadSensors();
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
