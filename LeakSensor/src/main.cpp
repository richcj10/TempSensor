#include <Arduino.h>

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

int SenorWaterFault = 550;
int SenorWaterTripThreshold = 300;
int SenorDisconectedThreshold = 600;
int SenorVoltageHighThreshold = 800;
int SenorVoltageLowThreshold = 300;
long SensorTestTime = 3600;

int LeakSenor[4] = {SENSOROK,SENSOROK,SENSOROK,SENSOROK};

byte ReadSensors(){
  digitalWrite(SENSORPWR, HIGH);
  delay(50);
  float VoltSense = analogRead(VOLTSENSE)/1025.0*5.5;
  Serial.print(" SensorVoltage = ");
  Serial.println(VoltSense);
    if((VoltSense >= SenorVoltageLowThreshold/100) and (VoltSense <= SenorVoltageHighThreshold/100)){
    LeakSenor[0] = SENSOROK;
    Serial.println("Sensor Voltage Ok");
  }
  else{
    LeakSenor[0] = SENSORFAULT;
    Serial.println("Sensor Voltage Out Of Range");
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
  digitalWrite(SENSORPWR, LOW);
  return 0;
}

char SensorValueCheck(double ValueIn){
  if(ValueIn > SenorWaterFault){
    return SENSORHIGHFAULT;
  }
  if((ValueIn < SenorWaterFault) and (ValueIn > SenorDisconectedThreshold)){
    return SENSORHIGHFAULT;
  }
}

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
