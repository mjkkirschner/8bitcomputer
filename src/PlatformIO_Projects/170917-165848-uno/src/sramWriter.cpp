#include <Arduino.h>
#include <sramWriter.h>

BYTE1_SRAM::BYTE1_SRAM(uint8_t MSB_ADDRESS_PIN,
                       uint8_t LSB_ADDRESS_PIN,
                       uint8_t MSB_DATA_PIN,
                       uint8_t LSB_DATA_PIN,
                       uint8_t RUNMODE_PIN,
                       uint8_t WRITE_ENABLE_PIN)
{
    _msbAddress = MSB_ADDRESS_PIN;
    _lsbAddress = LSB_ADDRESS_PIN;
    _msbData = MSB_DATA_PIN;
    _lsbData = LSB_DATA_PIN;
    runmodePin = RUNMODE_PIN;
    writeEnablePin = WRITE_ENABLE_PIN;

    pinMode(runmodePin, OUTPUT);
    pinMode(writeEnablePin, OUTPUT);

    digitalWrite(runmodePin, LOW);
    //active low so this puts the chip into read mode.
    digitalWrite(writeEnablePin, HIGH);

    Serial.begin(57600);

    Serial.println("set all address pins to output");
    //TODO offset or something?... it skips by two now since we use the mega pins...
    for (int pin = _msbAddress; pin <= _lsbAddress; pin += 2)
    {
        pinMode(pin, OUTPUT);
    }

    Serial.println("set all data pins to output");
    for (int pin = _msbData; pin <= _lsbData; pin += 2)
    {
        pinMode(pin, OUTPUT);
    }
    Serial.println("SRAM OBJECT READY...");
}

/**
 * returns the byte contained at the current address via the data lines
 */
byte BYTE1_SRAM::readData()
{
    byte data = 0;
    for (int pin = _msbData; pin <= _lsbData; pin += 2)
    {
        Serial.println("pin:");
        Serial.print(pin);
        Serial.print("is:");
        Serial.println(digitalRead(pin));
        data = (data << 1) + digitalRead(pin);
    }
    return data;
}

/**
 * print all data in ram up to address to the serial console.
 */

void BYTE1_SRAM::printRamContents(int maxAddressToRead)
{
    //make sure we are not writing!
    digitalWrite(writeEnablePin, HIGH);
    delayMicroseconds(5);
    for (int address = 0; address < maxAddressToRead; address++)
    {
        Serial.println("READING ******************");
        Serial.println("setting address lines for address");
        Serial.println(address);

        setAddressLines(address);
        delayMicroseconds(5);
        //read all the data lines
        Serial.println("data is:");
        Serial.println(readData());
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
        Serial.println("set address pin: ");
        Serial.print(pin);
        Serial.print(" :state:  ");
        Serial.print(bitState);
        Serial.println("");
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
        Serial.println("set data pin: ");
        Serial.print(pin);
        Serial.print(" :state:  ");
        Serial.print(bitState);
        Serial.println("");
        digitalWrite(pin, bitState);
        count = count - 1;
    }
}

void BYTE1_SRAM::writeData(int address, int data)
{
    setAddressLines(address);
    setDataLines(data);
    delayMicroseconds(5);
    //toggle the write enable low then HIGH
    digitalWrite(PROGRAMMER_WRITE_ENABLE, LOW);
    delayMicroseconds(5);
    digitalWrite(PROGRAMMER_WRITE_ENABLE, HIGH);
    delayMicroseconds(5);

    Serial.println("******************");
    Serial.println("WRITE COMPLETE");
}