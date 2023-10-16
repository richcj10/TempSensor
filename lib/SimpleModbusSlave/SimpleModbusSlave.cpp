#include "SimpleModbusSlave.h"
#include <math.h>

#define BUFFER_SIZE 128
#define BitSet(arg,posn) ((arg) | (1L << (posn)))

// frame[] is used to recieve and transmit packages.
// The maximum serial ring buffer size is 128
unsigned char frame[BUFFER_SIZE];
unsigned int holdingRegsSize; // size of the register array
unsigned int InputRegsSize; // size of the register array
unsigned int CoilRegsSize; // size of the register array
unsigned int DiscreateInputSize; // size of the register array
unsigned char broadcastFlag;
unsigned char slaveID;
unsigned char function;
unsigned char TxEnablePin;
unsigned int errorCount;
unsigned int T1_5; // inter character time out
unsigned int T3_5; // frame delay
unsigned char noOfBytes = 0;
boolean BoolReturnData[20];
unsigned char ReturnData[10];
unsigned char ForLoopCounter = 0;
// function definitions
void exceptionResponse(unsigned char exception);
unsigned int calculateCRC(unsigned char bufferSize);
void sendPacket(unsigned char bufferSize);


void modbus_set_bits_from_byte(uint8_t *dest, int idx, const uint8_t value);
void modbus_set_bits_from_bytes(uint8_t *dest, int idx, unsigned int nb_bits, const uint8_t *tab_byte);
uint8_t modbus_get_byte_from_bits(const uint8_t *src, int idx,unsigned int nb_bits);

unsigned int modbus_update(unsigned int *holdingRegs, unsigned int *inputregs,bool *coilregs, bool *DiscreteInputregs){
    unsigned char buffer = 0;
    unsigned char overflow = 0;

    while (Serial.available()) {
        // The maximum number of bytes is limited to the serial buffer size of 128 bytes
        // If more bytes is received than the BUFFER_SIZE the overflow flag will be set and the
        // serial buffer will be red untill all the data is cleared from the receive buffer.
        if (overflow)
            Serial.read();
        else {
            if (buffer == BUFFER_SIZE)
                overflow = 1;
            frame[buffer] = Serial.read();
            buffer++;
        }
        delayMicroseconds(T1_5); // inter character time out
    }

    // If an overflow occurred increment the errorCount
    // variable and return to the main sketch without
    // responding to the request i.e. force a timeout
    if (overflow)
        return errorCount++;

    // The minimum request packet is 8 bytes for function 3 & 16
    if (buffer > 6) {
        unsigned char id = frame[0];

        broadcastFlag = 0;

        if (id == 0)
            broadcastFlag = 1;

        if (id == slaveID || broadcastFlag) { // if the recieved ID matches the slaveID or broadcasting id (0), continue
            unsigned int crc = ((frame[buffer - 2] << 8) | frame[buffer - 1]); // combine the crc Low & High bytes
            if (calculateCRC(buffer - 2) == crc) { // if the calculated crc matches the recieved crc continue
                function = frame[1];
                unsigned int startingAddress = ((frame[2] << 8) | frame[3]); // combine the starting address bytes
                unsigned int no_of_registers = ((frame[4] << 8) | frame[5]); // combine the number of register bytes
                unsigned int maxData = startingAddress + no_of_registers;
                unsigned char index;
                unsigned char address;
                unsigned int crc16;

                // broadcasting is not supported for function 3
                if (!broadcastFlag && (function == 1)) {
                    if (startingAddress < CoilRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
                        if (maxData <= CoilRegsSize) { // check exception 3 ILLEGAL DATA VALUE
                            //delay(2);
                             if(no_of_registers < 8){
                                noOfBytes = 1;
                            }
                            else{
                                noOfBytes = no_of_registers / 8;
                                if((no_of_registers % 8) != 0){
                                    noOfBytes = noOfBytes +1;
                                }
                            }
                            ForLoopCounter = startingAddress;
                            //ForLoopCounter = 0;
                            unsigned char BitCounter = 0;
                            unsigned char ByteCounter = 0;
                             for(;ForLoopCounter<no_of_registers;ForLoopCounter++){
                                if(coilregs[ForLoopCounter] == 1){
                                    ReturnData[ByteCounter] = BitSet(ReturnData[ByteCounter],BitCounter);
                                    //ReturnData[ByteCounter] = 0xAA;
                                }
                                BitCounter++;
                                if(BitCounter>7){
                                    BitCounter = 0;
                                    ByteCounter++;
                                }
                            }
                            //ReturnData[0] = 0x01;
                            //unsigned char noOfBytes = no_of_registers * 2;
                            unsigned char responseFrameSize = 5 + noOfBytes; // ID, function, noOfBytes, (dataLo + dataHi) * number of registers, crcLo, crcHi
                            frame[0] = slaveID;
                            frame[1] = function;
                            frame[2] = noOfBytes;
                            address = 3; // PDU starts at the 4th byte
                            //unsigned int temp;
                            //
                            //modbus_get_byte_from_bits();
                            for (unsigned char jk = 0; jk <= noOfBytes; jk++) {
                                frame[address] = ReturnData[jk]; // split the register into 2 bytes
                                address++;
                                ReturnData[jk] = 0; //Erase Data
                            }
                            crc16 = calculateCRC(responseFrameSize - 2);
                            frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
                            frame[responseFrameSize - 1] = crc16 & 0xFF;
                            sendPacket(responseFrameSize);
                        } else
                            exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
                    } else
                        exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                }
                // broadcasting is not supported for function 2 - @ToDo 
                 else if (!broadcastFlag && (function == 2)) {
                    if (startingAddress < DiscreateInputSize) { // check exception 2 ILLEGAL DATA ADDRESS
                        if (maxData <= DiscreateInputSize) { // check exception 3 ILLEGAL DATA VALUE
                            //delay(2);
                             if(no_of_registers < 8){
                                noOfBytes = 1;
                            }
                            else{
                                noOfBytes = no_of_registers / 8;
                                if((no_of_registers % 8) != 0){
                                    noOfBytes = noOfBytes +1;
                                }
                            }
                            ForLoopCounter = startingAddress;
                            //ForLoopCounter = 0;
                            unsigned char BitCounter = 0;
                            unsigned char ByteCounter = 0;
                             for(;ForLoopCounter<no_of_registers;ForLoopCounter++){
                                if(DiscreteInputregs[ForLoopCounter] == 1){
                                    ReturnData[ByteCounter] = BitSet(ReturnData[ByteCounter],BitCounter);
                                    //ReturnData[ByteCounter] = 0xAA;
                                }
                                BitCounter++;
                                if(BitCounter>7){
                                    BitCounter = 0;
                                    ByteCounter++;
                                }
                            }
                            //ReturnData[0] = 0x01;
                            //unsigned char noOfBytes = no_of_registers * 2;
                            unsigned char responseFrameSize = 5 + noOfBytes; // ID, function, noOfBytes, (dataLo + dataHi) * number of registers, crcLo, crcHi
                            frame[0] = slaveID;
                            frame[1] = function;
                            frame[2] = noOfBytes;
                            address = 3; // PDU starts at the 4th byte
                            //unsigned int temp;
                            //
                            //modbus_get_byte_from_bits();
                            for (unsigned char jk = 0; jk <= noOfBytes; jk++) {
                                frame[address] = ReturnData[jk]; // split the register into 2 bytes
                                address++;
                                ReturnData[jk] = 0; //Erase Data
                            }
                            crc16 = calculateCRC(responseFrameSize - 2);
                            frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
                            frame[responseFrameSize - 1] = crc16 & 0xFF;
                            sendPacket(responseFrameSize);
                        } else
                            exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
                    } else
                        exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                }
                // broadcasting is not supported for function 3
                else if (!broadcastFlag && (function == 3)) {
                    if (startingAddress < holdingRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
                        if (maxData <= holdingRegsSize) { // check exception 3 ILLEGAL DATA VALUE
                            noOfBytes = no_of_registers * 2;
                            unsigned char responseFrameSize = 5 + noOfBytes; // ID, function, noOfBytes, (dataLo + dataHi) * number of registers, crcLo, crcHi
                            frame[0] = slaveID;
                            frame[1] = function;
                            frame[2] = noOfBytes;
                            address = 3; // PDU starts at the 4th byte
                            unsigned int temp;

                            for (index = startingAddress; index < maxData; index++) {
                                temp = holdingRegs[index];
                                frame[address] = temp >> 8; // split the register into 2 bytes
                                address++;
                                frame[address] = temp & 0xFF;
                                address++;
                            }

                            crc16 = calculateCRC(responseFrameSize - 2);
                            frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
                            frame[responseFrameSize - 1] = crc16 & 0xFF;
                            sendPacket(responseFrameSize);
                        } else
                            exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
                    } else
                        exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                }
                else if (!broadcastFlag && (function == 4)) {
                    if (startingAddress < InputRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
                        if (maxData <= InputRegsSize) { // check exception 3 ILLEGAL DATA VALUE
                            noOfBytes = no_of_registers * 2;
                            unsigned char responseFrameSize = 5 + noOfBytes; // ID, function, noOfBytes, (dataLo + dataHi) * number of registers, crcLo, crcHi
                            frame[0] = slaveID;
                            frame[1] = function;
                            frame[2] = noOfBytes;
                            address = 3; // PDU starts at the 4th byte
                            unsigned int temp;

                            for (index = startingAddress; index < maxData; index++) {
                                temp = inputregs[index];
                                frame[address] = temp >> 8; // split the register into 2 bytes
                                address++;
                                frame[address] = temp & 0xFF;
                                address++;
                            }

                            crc16 = calculateCRC(responseFrameSize - 2);
                            frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
                            frame[responseFrameSize - 1] = crc16 & 0xFF;
                            sendPacket(responseFrameSize);
                        } else
                            exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
                    } else
                        exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                }
                else if (!broadcastFlag && (function == 5)) {
                    if (startingAddress < CoilRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
                        //if (maxData <= CoilRegsSize) { // check exception 3 ILLEGAL DATA VALUE
                            //crc16 = calculateCRC(responseFrameSize - 2);
                            //frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
                            //frame[responseFrameSize - 1] = crc16 & 0xFF;
                        
                        if(frame[4] == 0xFF){
                            coilregs[startingAddress] = 1;
                            //digitalWrite(13,HIGH);
                        }
                        else{
                            coilregs[startingAddress] = 0;
                            //digitalWrite(13,LOW);
                        }
                        sendPacket(8);
                        //} else
                        //    exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
                    } else
                        exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                }
                else if (function == 6) {
                    if (startingAddress < holdingRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
                        unsigned int startingAddress = ((frame[2] << 8) | frame[3]);
                        unsigned int regStatus = ((frame[4] << 8) | frame[5]);
                        unsigned char responseFrameSize = 8;

                        holdingRegs[startingAddress] = regStatus;

                        crc16 = calculateCRC(responseFrameSize - 2);
                        frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
                        frame[responseFrameSize - 1] = crc16 & 0xFF;
                        sendPacket(responseFrameSize);
                    } else
                        exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                }
                else if (!broadcastFlag && (function == 15)) {
                    if (startingAddress < CoilRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
                        //if (maxData <= CoilRegsSize) { // check exception 3 ILLEGAL DATA VALUE
                        unsigned char responseFrameSize = 8;
                        crc16 = calculateCRC(responseFrameSize - 2);
                        frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
                        frame[responseFrameSize - 1] = crc16 & 0xFF;
                        sendPacket(responseFrameSize);
                    } else
                        exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                }
                else if (function == 16) {
                    // check if the recieved number of bytes matches the calculated bytes minus the request bytes
                    // id + function + (2 * address bytes) + (2 * no of register bytes) + byte count + (2 * CRC bytes) = 9 bytes
                    if (frame[6] == (buffer - 9)) {
                        if (startingAddress < holdingRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
                            if (maxData <= holdingRegsSize) { // check exception 3 ILLEGAL DATA VALUE
                                address = 7; // start at the 8th byte in the frame

                                for (index = startingAddress; index < maxData; index++) {
                                    holdingRegs[index] = ((frame[address] << 8) | frame[address + 1]);
                                    address += 2;
                                }

                                // only the first 6 bytes are used for CRC calculation
                                crc16 = calculateCRC(6);
                                frame[6] = crc16 >> 8; // split crc into 2 bytes
                                frame[7] = crc16 & 0xFF;

                                // a function 16 response is an echo of the first 6 bytes from the request + 2 crc bytes
                                if (!broadcastFlag) // don't respond if it's a broadcast message
                                    sendPacket(8);
                            } else
                                exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
                        } else
                            exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                    } else
                        errorCount++; // corrupted packet
                } else
                    exceptionResponse(1); // exception 1 ILLEGAL FUNCTION
            } else // checksum failed
                errorCount++;
        } // incorrect id
    } else if (buffer > 0 && buffer < 8)
        errorCount++; // corrupted packet

    return errorCount;
}

void exceptionResponse(unsigned char exception)
{
    errorCount++; // each call to exceptionResponse() will increment the errorCount
    if (!broadcastFlag) { // don't respond if its a broadcast message
        frame[0] = slaveID;
        frame[1] = (function | 0x80); // set the MSB bit high, informs the master of an exception
        frame[2] = exception;
        unsigned int crc16 = calculateCRC(3); // ID, function + 0x80, exception code == 3 bytes
        frame[3] = crc16 >> 8;
        frame[4] = crc16 & 0xFF;
        sendPacket(5); // exception response is always 5 bytes ID, function + 0x80, exception code, 2 bytes crc
    }
}

void modbus_configure(long baud, byte _slaveID, byte _TxEnablePin, unsigned int _holdingRegsSize, unsigned int _InputregsSize, unsigned int _CoilregsSize, unsigned int _DiscreteInputregsSize, unsigned char _lowLatency){
    slaveID = _slaveID;
    Serial.begin(baud);

    if (_TxEnablePin > 1) {
        // pin 0 & pin 1 are reserved for RX/TX. To disable set txenpin < 2
        TxEnablePin = _TxEnablePin;
        pinMode(TxEnablePin, OUTPUT);
        digitalWrite(TxEnablePin, LOW);
    }

    // Modbus states that a baud rate higher than 19200 must use a fixed 750 us
    // for inter character time out and 1.75 ms for a frame delay.
    // For baud rates below 19200 the timeing is more critical and has to be calculated.
    // E.g. 9600 baud in a 10 bit packet is 960 characters per second
    // In milliseconds this will be 960characters per 1000ms. So for 1 character
    // 1000ms/960characters is 1.04167ms per character and finaly modbus states an
    // intercharacter must be 1.5T or 1.5 times longer than a normal character and thus
    // 1.5T = 1.04167ms * 1.5 = 1.5625ms. A frame delay is 3.5T.
    // Added experimental low latency delays. This makes the implementation
    // non-standard but practically it works with all major modbus master implementations.

    if (baud == 1000000 && _lowLatency) {
        T1_5 = 1;
        T3_5 = 10;
    } else if (baud >= 115200 && _lowLatency) {
        T1_5 = 75;
        T3_5 = 175;
    } else if (baud > 19200) {
        T1_5 = 750;
        T3_5 = 1750;
    } else {
        T1_5 = 15000000/baud; // 1T * 1.5 = T1.5
        T3_5 = 35000000/baud; // 1T * 3.5 = T3.5
    }

    InputRegsSize = _InputregsSize;
    CoilRegsSize = _CoilregsSize; // size of the register array
    DiscreateInputSize = _DiscreteInputregsSize; // size of the register array

    holdingRegsSize = _holdingRegsSize;
    errorCount = 0; // initialize errorCount
}

unsigned int calculateCRC(byte bufferSize){
    unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    for (unsigned char i = 0; i < bufferSize; i++) {
        temp = temp ^ frame[i];
        for (unsigned char j = 1; j <= 8; j++) {
            flag = temp & 0x0001;
            temp >>= 1;
            if (flag)
                temp ^= 0xA001;
        }
    }
    // Reverse byte order.
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF;
    return temp; // the returned value is already swopped - crcLo byte is first & crcHi byte is last
}

void sendPacket(unsigned char bufferSize){
    if (TxEnablePin > 1)
        digitalWrite(TxEnablePin, HIGH);
    delayMicroseconds(500);
    for (unsigned char i = 0; i < bufferSize; i++)
        Serial.write(frame[i]);

    Serial.flush();

    // allow a frame delay to indicate end of transmission
    delayMicroseconds(T3_5);

    if (TxEnablePin > 1)
        digitalWrite(TxEnablePin, LOW);
}

/* Sets many bits from a single byte value (all 8 bits of the byte value are
   set) */
void modbus_set_bits_from_byte(uint8_t *dest, int idx, const uint8_t value){
    int i;

    for (i=0; i < 8; i++) {
        dest[idx+i] = (value & (1 << i)) ? 1 : 0;
    }
}

/* Sets many bits from a table of bytes (only the bits between idx and
   idx + nb_bits are set) */
void modbus_set_bits_from_bytes(uint8_t *dest, int idx, unsigned int nb_bits, const uint8_t *tab_byte){
    unsigned int i;
    int shift = 0;

    for (i = idx; i < idx + nb_bits; i++) {
        dest[i] = tab_byte[(i - idx) / 8] & (1 << shift) ? 1 : 0;
        /* gcc doesn't like: shift = (++shift) % 8; */
        shift++;
        shift %= 8;
    }
}

/* Gets the byte value from many bits.
   To obtain a full byte, set nb_bits to 8. */
uint8_t modbus_get_byte_from_bits(const uint8_t *src, int idx,unsigned int nb_bits){
    unsigned int i;
    uint8_t value = 0;

    if (nb_bits > 8) {
        nb_bits = 8;
    }

    for (i=0; i < nb_bits; i++) {
        value |= (src[idx+i] << i);
    }

    return value;
}
