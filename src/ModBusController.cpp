#include <Arduino.h>
#include "ModBusController.h"
#include <SimpleModbusSlave.h>
#include "Define.h"
#include "Sensor.h"
#define  ledPin  13 // onboard led 
#define  buttonPin  7 // push button

unsigned int holdingRegs[10]; // function 3 and 16 register array
bool Coil[5]; //8 "bits"
bool DI[5]; //8 "bits
unsigned int InputRegs[8]; // function 3 and 16 register array
int ErrorCounter = 0;
unsigned long Time = 0;

void ModbusSetup(){
    modbus_configure(38400, TEMPSENSORADDRESS, DIR, 10,8,5,5, 0);
}

void ModBusLoop(){
    // modbus_update() is the only method used in loop(). It returns the total error
    // count since the slave started. You don't have to use it but it's useful
    // for fault finding by the modbus master.
    ErrorCounter = modbus_update(holdingRegs,InputRegs,Coil,DI);

    if((millis() - Time) > 200){
        //Update Regs
        InputRegs[0] = TEMPSENSORTYPE;
        InputRegs[1] = GetSensorValues(SENSOR_INPUT_VIN)*100;
        InputRegs[2] = ReadSensorValues(SENSOR_BIAS);
        InputRegs[3] = ReadSensorValuesModBus(SENSOR_INPUT_1);
        InputRegs[4] = ReadSensorValuesModBus(SENSOR_INPUT_2);
        InputRegs[5] = ReadSensorValuesModBus(SENSOR_INPUT_3);
        InputRegs[6] = ReadSensorValuesModBus(SENSOR_INPUT_3);
        Time = millis();
    }
    //delay(100);
}

bool GetCoilValues(unsigned char Type){
    return Coil[Type];
}

void SetCoilValues(unsigned char Type,bool value){
    Coil[Type] = value;
}

bool GetDigitalInputValues(unsigned char Type){
    return DI[Type];
}

void SetDigitalInputValues(unsigned char Type,bool value){
    DI[Type] = value;
}