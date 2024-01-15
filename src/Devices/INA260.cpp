#include <Arduino.h>
#include "INA260.h"
#include "Com.h"
#include <Wire.h>

float INACurrentRead(unsigned char i);
float INAVoltageRead(unsigned char i);
int INARead(unsigned char i);

void INASetup(){
    int A,B,C = 0;
    A = INARead(POWER_IN);
    B = INARead(FIVE_VOLT_RAIL);
    C = INARead(THREE_VOLT_RAIL);
    if((A == MFG_ID) && (B == MFG_ID) && (C == MFG_ID)){
        Log(DEBUG,"INA OK");
    }
    else{
        Log(NOTIFY,"INA Issue - ");
        if(A != MFG_ID){
            Log(NOTIFY,"A");
        }
        if(B != MFG_ID){
            Log(NOTIFY," B");
        }
        if(C != MFG_ID){
            Log(NOTIFY,"INA Issue - ");
        }
    }
}

void ReportTemp() {
  Serial.print("Current: ");
  Serial.print(INACurrentRead(POWER_IN));
  Serial.println(" mA");

  Serial.print("Bus Voltage: ");
  Serial.print(INAVoltageRead(POWER_IN));
  Serial.println(" mV");
  Serial.println();
}

float INACurrentRead(unsigned char i){
  Wire.beginTransmission(i); 
  //send a bit and ask for register zero
  Wire.write(INA260_REG_CURRENT);
  //end transmission
  Wire.endTransmission();
  //request 1 byte from address xx
  Wire.requestFrom(i, 2);
  //wait for response
  while(Wire.available() == 0);
  //put the temperature in variable c
  int a = Wire.read();
  int b = Wire.read(); 
  float Value = ((a << 8) + b);
  return Value*1.25;
}

float INAVoltageRead(unsigned char i){
  Wire.beginTransmission(i); 
  //send a bit and ask for register zero
  Wire.write(INA260_REG_BUSVOLTAGE);
  //end transmission
  Wire.endTransmission();
  //request 1 byte from address xx
  Wire.requestFrom(i, 2);
  //wait for response
  while(Wire.available() == 0);
  //put the temperature in variable c
  int a = Wire.read();
  int b = Wire.read(); 
  float Value = ((a << 8) + b);
  return Value*1.25;
}

int INARead(unsigned char i){
  Wire.beginTransmission(i); 
  //send a bit and ask for register zero
  Wire.write(INA260_REG_BUSVOLTAGE);
  //end transmission
  Wire.endTransmission();
  //request 1 byte from address xx
  Wire.requestFrom(i, 2);
  //wait for response
  while(Wire.available() == 0);
  //put the temperature in variable c
  int a = Wire.read();
  int b = Wire.read(); 
  float Value = ((a << 8) + b);
  return Value  
}