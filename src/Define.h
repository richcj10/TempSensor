#ifndef DEFINE_H
#define DEFINE_H

//IO DEFINE
#define SOM_RESET A0 //out
#define SDCARD_EMMC A1 //out
#define NAND_H A2 //out
#define NAND_L A3 //out
#define REMT_EN 3 //Out
#define USBC_PWR_RST 5 //out
#define PMIC_ON_REQ 7 //In
#define POWER_3V3_EN 8 //Out
#define POWER_5V_EN 9 //Out

//SENSOR DEFINE
#define TEMPSENSORADDRESS 0x20
#define TEMPSENSORTYPE 0x15

#define SENSOR_CHANGE_ADDRESS 0x10
#define SENSOR_CHANGE_SCANRATE 0x11
#define SENSOR_CHANGE_FAULT 0x12
#define SENSOR_CHANGE_SENSOR_OPEN 0x13

#define SENSOR_SCAN_EN 0//Sensor Scan Enable
#define SENSOR_LED_CONTROL 1//LED Control
#define AUTO_SENSOR_RELAY_CONTROL 2 //Auto Relay Control - Default ON
#define SENSOR_RELAY 3 //Relay Control - Default OFF

#define SENSORSCANRATE 2
#define SENSORTESTTIMEMULTIPLYER 1000

#define SENSORHIGHFAULT 3
#define SENSORFAULT 2
#define SENSORWATERDETECT 4
#define SENSOROK 1
#define VOLT_AVG 10

#endif