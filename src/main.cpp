#include <Arduino.h>
#include "EEPROMData.h"

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

#define LEAKSENSORADDRESS 0x20
#define LEAKSENSORTYPE 0x20

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
unsigned long previousLEDMillis = 0;
int SenorWaterFault = 550;
int SenorWaterTripThreshold = 300;
int SenorDisconectedThreshold = 600;
int SenorVoltageHighThreshold = 800;
int SenorVoltageLowThreshold = 270;
unsigned int SensorTestTime = 3;

char LeakSenor[5] = {SENSOROK,SENSOROK,SENSOROK,SENSOROK};

char SensorValueCheck(double ValueIn);
char ReadSensors();

char ReadSensors(){
  digitalWrite(SENSORPWR, HIGH);
  delay(50);
  float VoltSense = analogRead(VOLTSENSE)/1025.0*5*2;
  //Serial.print(" SensorVoltage = ");
  //Serial.println(VoltSense);
    if((VoltSense >= SenorVoltageLowThreshold/100) and (VoltSense <= SenorVoltageHighThreshold/100)){
    LeakSenor[0] = SENSOROK;
    //Serial.println("Sensor Voltage Ok");
  }
  else{
    LeakSenor[0] = SENSORFAULT;
    //Serial.println("Sensor Voltage Out Of Range");
    digitalWrite(SENSORPWR, LOW);
    return 0;
  }
  double SensorA = 0;
  double SensorB = 0;
  double SensorC = 0;
  for(char i = 0;i<5;i++){
    SensorA += analogRead(LEAKSENSOR1);
    SensorB += analogRead(LEAKSENSOR2);
    SensorC += analogRead(LEAKSENSOR3);
  }
  SensorA = SensorA/5;
  SensorB = SensorB/5;
  SensorC = SensorC/5;
  LeakSenor[1] = SensorValueCheck(SensorA);
  LeakSenor[2] = SensorValueCheck(SensorB);
  LeakSenor[3] = SensorValueCheck(SensorC);
  digitalWrite(SENSORPWR, LOW);
  return 0;
}

char SensorValueCheck(double ValueIn){
  if(ValueIn > SenorWaterFault){
    return SENSORHIGHFAULT;
  }
  else if((ValueIn < SenorWaterFault) and (ValueIn > SenorDisconectedThreshold)){
    return SENSOROK;
  }
  else{
    return SENSORWATERDETECT;
  }
}

char RelayCheck(){
  char error = 0;
  unsigned char k;
  for( k= 0;k<5;k++){
    if(LeakSenor[k] != SENSOROK){
      error++;
    }
  }
  if(error){
    digitalWrite(RELAY, HIGH);
    ModbusRTUServer.discreteInputWrite(4, 1);
    return  0;
  }
  else{
    digitalWrite(RELAY, LOW);
    ModbusRTUServer.discreteInputWrite(4, 0);
    return  1;
  }
}

void ComunicationUpdate(){
  ModbusRTUServer.poll();
/*   int coilValue = ModbusRTUServer.coilRead(0x00);
  if (coilValue == 0x11) {
    //NewAddress
    SetAddressFromEEPROM(ModbusRTUServer.coilRead(0x01));
  }
  else if (coilValue == 0x12) {
    //NewScanRate
    SetScanRateFromEEPROM(ModbusRTUServer.coilRead(0x01));
  }  */
  // else if (coilValue == 0x13) {
  //   //Turn On LED
  //   //SetScanRateFromEEPROM(ModbusRTUServer.coilRead(0x01);
  // } 
  // else if (coilValue == 0x14) {
  //   //SensorFault_Open
  //   //SetScanRateFromEEPROM((ModbusRTUServer.coilRead(0x01)<<8) | ModbusRTUServer.coilRead(0x02));
  // } 
  ModbusRTUServer.inputRegisterWrite(1, LeakSenor[0]);
  ModbusRTUServer.inputRegisterWrite(2, LeakSenor[1]);
  ModbusRTUServer.inputRegisterWrite(3, LeakSenor[2]);
  ModbusRTUServer.inputRegisterWrite(4, LeakSenor[3]);
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
  //Serial.println("Start");
  //char Adr = GetAddressFromEEPROM();
  //if(Adr == -1){
  if (!ModbusRTUServer.begin(LEAKSENSORADDRESS, 38400)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }
/*   if (!ModbusRTUServer.begin(RS485Class(Serial, 1, 4,0), 42, 38400)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  } */
/*   }
  else{
    if (!ModbusRTUServer.begin(Adr, 38400)) {
      Serial.println("Failed to start Modbus RTU Server!");
      while (1);
    }
  } */
  //char ScanRate = GetScanRateFromEEPROM();
  //if(ScanRate == -1){
  //  SensorTestTime = ScanRate;
  //}
  // configure a single coil at address 0x00
  ModbusRTUServer.configureCoils(0x00, 3); //Relay Control
  ModbusRTUServer.configureInputRegisters(0x00, 5);  //Leak Sensor Values
  ModbusRTUServer.inputRegisterWrite(0, LEAKSENSORTYPE);
  ModbusRTUServer.coilWrite(0x00, 1); //Sensor Scan Enable
  ModbusRTUServer.coilWrite(0x01, 1); //LED Control
  ModbusRTUServer.coilWrite(0x02, 0); //Relay Control
  // put your setup code here, to run once:

  SetAddressFromEEPROM(LEAKSENSORADDRESS);
  GetAddressFromEEPROM();
  Serial.print("Scan Rate : ");
  SetScanRateFromEEPROM(3);
  Serial.println(GetScanRateFromEEPROM()+41);
}

void loop() {
  ComunicationUpdate();
  unsigned long currentMillis = millis();
  if(ModbusRTUServer.coilRead(0)){
    if (currentMillis - previousMillis >= SensorTestTime*SENSORTESTTIMEMULTIPLYER) {
      previousMillis = currentMillis;
      ReadSensors();
      RelayCheck();
    }
  }
  if(ModbusRTUServer.coilRead(1)){
    if (currentMillis - previousLEDMillis >= 500) {
      previousLEDMillis = currentMillis;
      digitalWrite(LED,!digitalRead(LED));
    }
  }
}
