#ifndef sramWriter_h
#define sramWriter_h
#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif
class BYTE1_SRAM
{
  public:
    BYTE1_SRAM(uint8_t MSB_ADDRESS_PIN,
               uint8_t LSB_ADDRESS_PIN,
               uint8_t MSB_DATA_PIN,
               uint8_t LSB_DATA_PIN,
               uint8_t RUNMODE_PIN,
               uint8_t WRITE_ENABLE_PIN);
               byte readData();
               void printRamContents(int);
               void setAddressLines(int);
               void setDataLines(int);
               void writeData(int,int);

  private:
    uint8_t _msbAddress;
    uint8_t _lsbAddress;
    uint8_t _msbData;
    uint8_t _lsbData;
    uint8_t runmodePin;
    uint8_t writeEnablePin;
};
#endif