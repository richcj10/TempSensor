#ifndef MODBUSCONTROLLER_H
#define MODBUSCONTROLLER_H

void ModBusLoop();
void ModbusSetup();
bool GetCoilValues(unsigned char Type);
void SetCoilValues(unsigned char Type,bool value);
bool GetDigitalInputValues(unsigned char Type);
void SetDigitalInputValues(unsigned char Type,bool value);

#endif