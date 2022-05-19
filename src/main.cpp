#include <Arduino.h>
#include "EEPROMData.h"

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

#define LEAKSENSORADDRESS 0x20
#define LEAKSENSORTYPE 0x03

#define LEAKSENSOR1 A0
#define LEAKSENSOR2 A1
#define LEAKSENSOR3 A2
#define VOLTSENSE A3
#define DIR 4
#define SENSORPWR 5
#define RELAY 6
#define LED 7
#define SENSORTESTTIMEMULTIPLYER 1000

#define SENSORHIGHFAULT 3
#define SENSORFAULT 2
#define SENSORWATERDETECT 1
#define SENSOROK 0

unsigned long previousMillis = 0;
int SenorWaterFault = 550;
int SenorWaterTripThreshold = 300;
int SenorDisconectedThreshold = 600;
int SenorVoltageHighThreshold = 800;
int SenorVoltageLowThreshold = 300;
unsigned int SensorTestTime = 1;

void SetLED(char R, char G,char B){
  //Sreial.print(R);
}

void ComunicationUpdate(){
  ModbusRTUServer.poll();
  int coilValue = ModbusRTUServer.coilRead(0x00);
  if (coilValue == 0x11) {
    //NewAddress
    SetAddressFromEEPROM(ModbusRTUServer.coilRead(0x01));
  }
  else if (coilValue == 0x12) {
    //NewScanRate
    SetScanRateFromEEPROM(ModbusRTUServer.coilRead(0x01));
  } 
  else if (coilValue == 0x13) {
    //Turn On LED
    SetScanRateFromEEPROM((ModbusRTUServer.coilRead(0x01)<<8) | ModbusRTUServer.coilRead(0x02));
  } 
  else if (coilValue == 0x14) {
    //SensorFault_Open
    SetScanRateFromEEPROM((ModbusRTUServer.coilRead(0x01)<<8) | ModbusRTUServer.coilRead(0x02));
  } 
  else {
    // coil value clear, turn LED off
    //digitalWrite(ledPin, LOW);
  }
  ModbusRTUServer.discreteInputWrite(0, LeakSenor[0]);
  ModbusRTUServer.discreteInputWrite(1, LeakSenor[1]);
  ModbusRTUServer.discreteInputWrite(2, LeakSenor[2]);
  ModbusRTUServer.discreteInputWrite(3, LeakSenor[3]);
}

void setup() {
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
  int ScanRate = GetScanRateFromEEPROM();
  if(ScanRate == -1){
    SensorTestTime = ScanRate;
  }
  // configure a single coil at address 0x00
  ModbusRTUServer.configureCoils(0x00, 2);
  ModbusRTUServer.configureDiscreteInputs(0x00, 5);
  ModbusRTUServer.discreteInputWrite(0x00, LEAKSENSORTYPE);
  ModbusRTUServer.discreteInputWrite(4, 0);
  // put your setup code here, to run once:
}

void loop() {
  ComunicationUpdate();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= SensorTestTime*SENSORTESTTIMEMULTIPLYER) {
    previousMillis = currentMillis;
    ReadSensors();
    RelayCheck();
  }
}
