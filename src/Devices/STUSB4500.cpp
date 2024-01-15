#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_STUSB4500.h>
#include "Com.h"
#include "STUSB4500.h"

STUSB4500 usb;

void USBSinkSetup(){
    if(!usb.begin()){
        Log(NOTIFY,"STUSB4500 - Not detected");
        while(1);
    }
    Log(NOTIFY,"STUSB4500 - OK");
    //delay(100);
}

void SetUSBSink(){
    usb.setPdoNumber(3);
    //PDO1 - 5VOnly
    usb.setCurrent(1,1.0);
    usb.setUpperVoltageLimit(1,7);
    
    //PDO2
    usb.setVoltage(2,9.0);
    usb.setCurrent(2,1.0);
    usb.setLowerVoltageLimit(2,7.5);
    usb.setUpperVoltageLimit(2,10.5);
    
    //PDO3
    usb.setVoltage(3,12);
    usb.setCurrent(3,0.5);
    usb.setLowerVoltageLimit(3,10.5);
    usb.setUpperVoltageLimit(3,13.5);

    /* Flexible current value common to all PDOs */
    //usb.setFlexCurrent(1.0);

    /* Unconstrained Power bit setting in capabilities message sent by the sink */
    //  usb.setExternalPower(false);

    /* USB 2.0 or 3.x data communication capability by sink system */
    // usb.setUsbCommCapable(false);

    /* Selects POWER_OK pins configuration
        0 - Configuration 1
        1 - No applicable
        2 - Configuration 2 (default)
        3 - Configuration 3
    */
    //usb.setConfigOkGpio(2);

    /* Selects GPIO pin configuration
        0 - SW_CTRL_GPIO
        1 - ERROR_RECOVERY
        2 - DEBUG
        3 - SINK_POWER
    */
    //usb.setGpioCtrl(3);

    /* Selects VBUS_EN_SNK pin configuration */
    usb.setPowerAbove5vOnly(true);

    usb.write(); //Save Data
}

void GetUBCSinkSettings(){
    usb.read();
    /* Read settings for PDO1 */
    Serial.println();
    Serial.print("Voltage1 (V): ");
    Serial.println(usb.getVoltage(1));
    Serial.print("Current1 (A): ");
    Serial.println(usb.getCurrent(1));
    Serial.print("Lower Voltage Tolerance1 (%): ");
    Serial.println(usb.getLowerVoltageLimit(1));
    Serial.print("Upper Voltage Tolerance1 (%): ");
    Serial.println(usb.getUpperVoltageLimit(1));
    Serial.println();

    /* Read settings for PDO2 */
    Serial.print("Voltage2 (V): ");
    Serial.println(usb.getVoltage(2));
    Serial.print("Current2 (A): ");
    Serial.println(usb.getCurrent(2));
    Serial.print("Lower Voltage Tolerance2 (%): ");
    Serial.println(usb.getLowerVoltageLimit(2));
    Serial.print("Upper Voltage Tolerance2 (%): ");
    Serial.println(usb.getUpperVoltageLimit(2));
    Serial.println();

    /* Read settings for PDO3 */
    Serial.print("Voltage3 (V): ");
    Serial.println(usb.getVoltage(3));
    Serial.print("Current3 (A): ");
    Serial.println(usb.getCurrent(3));
    Serial.print("Lower Voltage Tolerance3 (%): ");
    Serial.println(usb.getLowerVoltageLimit(3));
    Serial.print("Upper Voltage Tolerance3 (%): ");
    Serial.println(usb.getUpperVoltageLimit(3));
    Serial.println();
}
