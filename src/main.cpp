#include <Arduino.h>
#include "Sensor.h"
#include "Define.h"
#include "Display.h"
#include "Com.h"

unsigned long previousMillis = 0;
unsigned long previousLEDMillis = 0;
unsigned int SensorTestTime = 3;

void IOSetup(){
  pinMode(SOM_RESET, OUTPUT);
  pinMode(SDCARD_EMMC, OUTPUT);
  pinMode(NAND_H, OUTPUT);
  pinMode(NAND_L, OUTPUT);
  pinMode(REMT_EN, OUTPUT);
  pinMode(USBC_PWR_RST, OUTPUT);
  pinMode(PMIC_ON_REQ, INPUT);
  pinMode(POWER_3V3_EN, OUTPUT);
  pinMode(POWER_5V_EN, OUTPUT);
}

void setup(){
  IOSetup();
  Serial.begin(115200);
  LogSetup(DEBUG);
  //Wire.begin();
  Serial.print("-Start-");
  DisplayStart();
  DisplayUpdate();
  DevicesSetup();
  LoadCmd("Help", "Description of Commands", 1, {Printhelp});
}

void loop(){
  CheckCmd();
}
