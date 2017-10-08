#include <Arduino.h>
#include <sramWriter.h>

BYTE1_SRAM::BYTE1_SRAM(uint8_t MSB_ADDRESS_PIN,
                       uint8_t LSB_ADDRESS_PIN,
                       uint8_t MSB_DATA_PIN,
                       uint8_t LSB_DATA_PIN,
                       uint8_t RUNMODE_PIN,
                       uint8_t WRITE_ENABLE_PIN, bool DEBUG)
{
    _msbAddress = MSB_ADDRESS_PIN;
    _lsbAddress = LSB_ADDRESS_PIN;
    _msbData = MSB_DATA_PIN;
    _lsbData = LSB_DATA_PIN;
    runmodePin = RUNMODE_PIN;
    writeEnablePin = WRITE_ENABLE_PIN;
    _debug = DEBUG;

    pinMode(runmodePin, OUTPUT);
    pinMode(writeEnablePin, OUTPUT);

    //start under computer control
    digitalWrite(runmodePin, HIGH);
    //active low so this puts the chip into read mode.
    digitalWrite(writeEnablePin, HIGH);

    for (int pin = _msbAddress; pin <= _lsbAddress; pin += 2)
    {
        pinMode(pin, OUTPUT);
    }

    for (int pin = _msbData; pin <= _lsbData; pin += 2)
    {
        pinMode(pin, OUTPUT);
    }
}

/**
 * returns the byte contained at the current address via the data lines
 */
byte BYTE1_SRAM::readData()
{
    digitalWrite(runmodePin, LOW);
    digitalWrite(writeEnablePin, HIGH);
    delayMicroseconds(5);
    byte data = 0;
    for (int pin = _msbData; pin <= _lsbData; pin += 2)
    {
        //Serial.println("pin:");
        //Serial.print(pin);
        //Serial.print("is:");
        //Serial.println(digitalRead(pin));
        data = (data << 1) + digitalRead(pin);
    }
    if (_debug)
    {
        Serial.println("******************");
        Serial.println("READ COMPLETE");
        Serial.println(data);
    }

    return data;
}

/**
 * print all data in ram up to address to the serial console.
 */

void BYTE1_SRAM::printRamContents(int maxAddressToRead)
{
    //make sure we are not writing!
    digitalWrite(runmodePin, LOW);
    digitalWrite(writeEnablePin, HIGH);
    delayMicroseconds(5);
    for (int address = 0; address < maxAddressToRead; address++)
    {
        if (_debug)
        {
            Serial.println("READING ******************");
            Serial.println("setting address lines for address");
            Serial.println(address);
        }
        setAddressLines(address);
        delayMicroseconds(5);
        if (_debug)
        {
            //read all the data lines
            Serial.println("data is:");
            Serial.println(readData());
        }
        delayMicroseconds(5);
    }
}

void BYTE1_SRAM::setAddressLines(int address)
{
    int count = 7;
    //loop all pins
    for (int pin = _msbAddress; pin <= _lsbAddress; pin += 2)
    {
        int bitState = bitRead(address, count);
        digitalWrite(pin, bitState);
        count = count - 1;
    }
}

void BYTE1_SRAM::setDataLines(int dataToWrite)
{
    int count = 7;
    //loop all pins
    for (int pin = _msbData; pin <= _lsbData; pin += 2)
    {
        int bitState = bitRead(dataToWrite, count);
        digitalWrite(pin, bitState);
        count = count - 1;
    }
}

void BYTE1_SRAM::writeData(int address, int data)
{

    setAddressLines(address);
    setDataLines(data);
    //assert that we are under control
    digitalWrite(runmodePin, LOW);

    delayMicroseconds(5);
    //toggle the write enable low then HIGH
    digitalWrite(writeEnablePin, LOW);
    delayMicroseconds(5);
    digitalWrite(writeEnablePin, HIGH);
    delayMicroseconds(5);
    if (_debug)
    {
        Serial.println("******************");
        Serial.println("WRITE COMPLETE");
        Serial.println(data);
    }
}