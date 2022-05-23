#include "Sensor.h"
#include <Arduino.h>

#include "Define.h"

//These values can be modified 
unsigned int SenorWaterFault = 550;
unsigned int SenorDisconectedThreshold = 600;

//This is hard coded, board Level issue
int SenorVoltageHighThreshold = 800;
int SenorVoltageLowThreshold = 270;

char LeakSenor[5] = {SENSOROK,SENSOROK,SENSOROK,SENSOROK};

char RelayState = 0;

char ReadSensors(){
  digitalWrite(SENSORPWR, HIGH);
  delay(50);
  float VoltSense = analogRead(VOLTSENSE)/1025.0*5*2;
  //Serial.print(" SensorVoltage = ");
  //Serial.println(VoltSense);
    if((VoltSense >= SenorVoltageLowThreshold/100) and (VoltSense <= SenorVoltageHighThreshold/100)){
    LeakSenor[0] = SENSOROK;
  }
  else{
    LeakSenor[0] = SENSORFAULT;
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
  if(ValueIn > SenorVoltageHighThreshold){
    return SENSORHIGHFAULT;
  }
  else if((ValueIn > SenorWaterFault) and (ValueIn < SenorDisconectedThreshold)){
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
    RelayState = 1;
    return  0;
  }
  else{
    digitalWrite(RELAY, LOW);
    RelayState = 0;
    return  1;
  }
}

char ReadSensorValues(unsigned char Value){
    return LeakSenor[Value];
}

char ReadRelayState(){
    return RelayState;
}

void SetSensorWaterDetect(unsigned int Value){
    SenorWaterFault = Value;
}

void SensorWaterDisconected(unsigned int Value){
    SenorDisconectedThreshold = Value;
}
