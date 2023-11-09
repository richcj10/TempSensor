#ifndef INA260_H
#define INA260_H

void INASetup();

#define MFG_ID 21577

#define POWER_IN 0x40
#define FIVE_VOLT_RAIL 0x41
#define THREE_VOLT_RAIL 0x42



#define INA260_I2CADDR_DEFAULT 0x40 ///< INA260 default i2c address
#define INA260_REG_CONFIG 0x00      ///< Configuration register
#define INA260_REG_CURRENT 0x01 ///< Current measurement register (signed) in mA
#define INA260_REG_BUSVOLTAGE 0x02 ///< Bus voltage measurement register in mV
#define INA260_REG_POWER 0x03      ///< Power calculation register in mW
#define INA260_REG_MASK_ENABLE 0x06 ///< Interrupt/Alert setting and checking register
#define INA260_REG_ALERT_LIMIT 0x07 ///< Alert limit value register
#define INA260_REG_MFG_UID 0xFE     ///< Manufacturer ID Register
#define INA260_REG_DIE_UID 0xFF     ///< Die ID and Revision Register

#endif