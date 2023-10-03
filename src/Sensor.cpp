#include "Sensor.h"
#include <Arduino.h>

#include "Define.h"

//These values can be modified 
unsigned int SenorWaterFault = 550;
unsigned int SenorDisconectedThreshold = 600;

//This is hard coded, board Level issue
int SenorVoltageHighThreshold = 800;
int SenorVoltageLowThreshold = 270;
float Vin = 0;
int VinAdc[VOLT_AVG];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
long Vintotal = 0;     

int ADCValue[5] = {0,0,0,0,0};
float TempResult[4] = {0,0,0,0};

const float A = 0.000686524426;
const float B = 0.000244478897;
const float C = 0.000000019057;

float R1 = 10000; //62330 ohm Total Pull - up resistors in series that I used.
float TempK;
float Vout;
float TempC;
float R2;
float logR2;

char RelayState = 0;

char ReadSensors(){
  digitalWrite(SENSORPWR, HIGH);
  delay(150);
  float VoltSense = analogRead(VOLTSENSE)/1025.0*5*2;
  //Serial.print(" SensorVoltage = ");
  //Serial.println(VoltSense);
  if((VoltSense >= SenorVoltageLowThreshold/100) and (VoltSense <= SenorVoltageHighThreshold/100)){
    TempResult[0] = SENSOROK;
  }
  else{
    TempResult[0] = SENSORFAULT;
    digitalWrite(SENSORPWR, LOW);
    return 0;
  }
  double SensorA = 0;
  double SensorB = 0;
  double SensorC = 0;
  for(char i = 0;i<5;i++){
    SensorA += (double)analogRead(TEMPSENSOR1);
    SensorB += (double)analogRead(TEMPSENSOR2);
    SensorC += (double)analogRead(TEMPSENSOR3);
  }
  SensorA = SensorA/5;
  SensorB = SensorB/5;
  SensorC = SensorC/5;
  VoltSense = analogRead(VOLTSENSE)/1025.0*5*2;
  TempResult[1] = TempConverter(SensorA,VoltSense);
  TempResult[2] = TempConverter(SensorB,VoltSense);
  TempResult[3] = TempConverter(SensorC,VoltSense);
  digitalWrite(SENSORPWR, LOW);
  return 1;
}

void SampleVin(){
  Vintotal = Vintotal - VinAdc[readIndex];
  // read from the sensor:
  VinAdc[readIndex] = analogRead(VIN);
  // add the reading to the total:
  Vintotal = Vintotal + VinAdc[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= VOLT_AVG) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  Vin = (Vintotal / VOLT_AVG)*(5.1/1024)*5.6;
  //Serial.print("Vin = ");
  //Serial.println(Vin);
}

float TempConverter(double ValueIn,float Vref){
  double steinhart = 0;
  float voltage = 5*(ValueIn/1024);
  //Serial.print("voltage = ");
  //Serial.println(voltage);
  //Serial.print("Vref = ");
  //Serial.println(Vref);
  float R = (voltage / ((Vref - voltage)/10000.0));
  if(R<0){
    return 555.99;
  }
  //Serial.print("Res = ");
  //Serial.println(R);
  steinhart = R/THERMISTORNOMINAL;     // (R/Ro)  
  steinhart = log(steinhart);                  // ln(R/Ro) 
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)  
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15; 

/*   logR2 = log(R);
  TempK = 1.0 / (A + B * logR2 + C * logR2 * logR2 * logR2);
  TempC = TempK - 273.15; */
  //Serial.print("steinhart = ");
  //Serial.println(steinhart);
  return steinhart;
}

float ReadSensorValues(unsigned char Value){
    return TempResult[Value];
}

char RelayCheck(){
  return 0;
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

float GetSensorValues(char Sensor){
  return Vin;
}
