#ifndef SENSOR_H
#define SENSOR_H

#define SENSOR_BIAS 0
#define SENSOR_INPUT_1 1
#define SENSOR_INPUT_2 2
#define SENSOR_INPUT_3 3
#define SENSOR_INPUT_VIN 4

#define SERIESRESISTOR 10000

#define THERMISTORNOMINAL 10000      // temp. for nominal resistance (almost always 25 C) 
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950

float TempConverter(double ValueIn,float Vref);
char ReadSensors();
char RelayCheck();
char ReadRelayState();
float ReadSensorValues(unsigned char Value);
int ReadSensorValuesModBus(unsigned char Value);
void SetSensorWaterDetect(unsigned int Value);
void SensorWaterDisconected(unsigned int Value);
float GetSensorValues(char Sensor);
void SampleVin();

#endif