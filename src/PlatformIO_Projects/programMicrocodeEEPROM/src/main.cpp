#include <Arduino.h>

//code inspired - basically copied ;) from Ben Eater's fantastic 8bit computer series
//which references A Malvino's Digital Computer Electronics.

#define SHIFT_DATA_OUT A0
#define SHIFT_DATA_IN A1

#define SHIFT_CLK A3

#define SHIFT_MODE0 10
#define SHIFT_MODE1 11

#define EEPROM_WRITE_ENABLE A4
#define EEPROM_OUT_ENABLE A5

#define EEPROM_A0 2
#define EEPROM_A7 9

#define RAMIN 0b00000000100000000000000000000000
#define RAMOUT 0b00000000010000000000000000000000
#define INSTRIN 0b00000000001000000000000000000000
#define INSTROUT 0b00000000000100000000000000000000
#define COUNTEROUT 0b00000000000010000000000000000000
//THERE IS AN EMPTY BIT HERE // UNUSED!!
#define COUNTERENABLE 0b00000000000000100000000000000000
#define AIN 0b00000000000000010000000000000000
#define AOUT 0b00000000000000001000000000000000
#define SUMOUT 0b00000000000000000100000000000000
#define ALU1 0b00000000000000000010000000000000
#define ALU2 0b00000000000000000001000000000000
#define ALU3 0b00000000000000000000100000000000
#define ALU4 0b00000000000000000000010000000000
#define B_IN 0b00000000000000000000001000000000
#define BOUT 0b00000000000000000000000100000000
#define OUTIN 0b00000000000000000000000010000000
#define HALT 0b00000000000000000000000001000000
#define MEMORYREGIN 0b00000000000000000000000000100000
#define JUMPALESSB 0b00000000000000000000000000010000
#define JUMPAEQUALB 0b00000000000000000000000000001000
#define JUMPAGREATERB 0b00000000000000000000000000000100

#define ADD ALU1 | ALU4
#define SUBTRACT ALU2 | ALU3

//TODO these are not updated to the instructions in speadsheet yet.
uint32_t microCodeData[] = {
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, 0, 0, 0, 0, 0, 0,                                                                                             //0000 NOP
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, RAMOUT | MEMORYREGIN, RAMOUT | AIN | COUNTERENABLE, 0, 0, 0,                        //0001 LOADA
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, AOUT | OUTIN, 0, 0, 0, 0, 0,                                                                                  //0010 OUT
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, RAMOUT | MEMORYREGIN, RAMOUT | B_IN | COUNTERENABLE, SUMOUT | AIN | ADD, 0, 0,      //0011 ADD
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, RAMOUT | MEMORYREGIN, RAMOUT | B_IN | COUNTERENABLE, SUMOUT | AIN | SUBTRACT, 0, 0, //0100 SUBTRACT
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, RAMOUT | MEMORYREGIN, RAMIN | AOUT | COUNTERENABLE, 0, 0, 0, 0,                     //0101 STOREA
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, RAMOUT | AIN | COUNTERENABLE, 0, 0, 0, 0,                                           //0110 LOADAIMMEDIATE
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, RAMOUT | JUMPAEQUALB | JUMPALESSB | JUMPAGREATERB, 0, 0, 0, 0,                      //0111 JUMP
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, COUNTERENABLE, RAMOUT | JUMPAEQUALB, 0, 0, 0,                                       //1000 JUMPIFEQUAL
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, COUNTERENABLE, RAMOUT | JUMPALESSB, 0, 0, 0,                                        //1001 JUMPIFLESS
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, COUNTERENABLE, RAMOUT | JUMPAGREATERB, 0, 0, 0,                                     //1010 JUMPIFGREATER
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, RAMOUT | MEMORYREGIN, RAMOUT | B_IN | COUNTERENABLE, 0, 0, 0,                       //1011 LOADB
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, RAMOUT | B_IN | COUNTERENABLE, 0, 0, 0, 0,                                          //1100 LOADBIMMEDIATE
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, MEMORYREGIN | COUNTEROUT, RAMOUT | MEMORYREGIN, RAMIN | BOUT | COUNTERENABLE, 0, 0, 0, 0,                     //1101 STOREB
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, 0, 0, 0, 0, 0, 0,                                                                                             //1110 NOP
    MEMORYREGIN | COUNTEROUT, RAMOUT | INSTRIN | COUNTERENABLE, HALT, 0, 0, 0, 0, 0,                                                                                          //1111 HALT

};


//set Address lines, MSB.
//ie, EEPROM_A0 is MSB.
void setAddress(int address)
{
  //set the out enable on the eeprom chip to read or write before
  //setting the address pins
  //digitalWrite(EEPROM_OUT_ENABLE, write);

  // shiftOut(SHIFT_DATA_OUT, SHIFT_CLK, MSBFIRST, address >> 8);
  // shiftOut(SHIFT_DATA_OUT, SHIFT_CLK, MSBFIRST, address);
  int count = 7;
  //loop all pins
  for (int pin = EEPROM_A0; pin <= EEPROM_A7; pin += 1)
  {
    int bitState = bitRead(address, count);
    digitalWrite(pin, bitState);
    count = count - 1;
  }
}

byte *readEEPROM(int address, int dataLength)
{
  digitalWrite(SHIFT_CLK, LOW);
  //false should put OE low which means read.
  digitalWrite(EEPROM_OUT_ENABLE, LOW);
  digitalWrite(EEPROM_WRITE_ENABLE, HIGH);
  setAddress(address);
  delay(10);

  //now we need to do two things:
  //1.load the data into the shift registers in parallel
  //2.shift it out to the left.

  digitalWrite(SHIFT_MODE0, HIGH);
  digitalWrite(SHIFT_MODE1, HIGH);
  delay(1);

  digitalWrite(SHIFT_CLK, HIGH);
  delay(1);

  //load the data in parallel into the registers.
  delay(10);

  //put the registers into shift left mode:
  digitalWrite(SHIFT_MODE0, LOW);
  digitalWrite(SHIFT_MODE1, HIGH);
  delay(10);

  byte *datas = new byte[3];
  for (int i = 0; i < dataLength; i++)
  {
    Serial.println("reading a byte from the EEPROM");
    Serial.println("address");
    Serial.println(address);

    byte data = shiftIn(SHIFT_DATA_IN, SHIFT_CLK, MSBFIRST);
    Serial.println("data");
    Serial.println(data);
    datas[i] = data;
  }
  return datas;
}

//shift out the bytes in the data array, then set address lines,
//and write out to the EEPROM
void writeEEPROM(int address, byte data[], int dataLength)
{
  digitalWrite(EEPROM_OUT_ENABLE, HIGH);
  digitalWrite(EEPROM_WRITE_ENABLE, HIGH);
  digitalWrite(SHIFT_CLK, LOW);

  setAddress(address);
  delay(10);

  byte curData;
  //iterate over each byte in the array and shift it out, iterate
  //backward so we shift the last byte out first.
  for (int i = dataLength - 1; i > -1; i = i - 1)
  {
    Serial.println("shift out a byte to EEPROM");
    Serial.println("address");
    Serial.println(address);
    Serial.println("data");
    curData = data[i];
    Serial.println(curData);

    digitalWrite(SHIFT_CLK, LOW);
    shiftOut(SHIFT_DATA_OUT, SHIFT_CLK, LSBFIRST, curData);

    delay(10);
  }
  //pulsing the write line low then high will write to byte to the eeprom.
  Serial.println("write out a byte to EEPROM");
  Serial.println(address);

  delay(15);
  digitalWrite(EEPROM_WRITE_ENABLE, LOW);
  delayMicroseconds(2);
  digitalWrite(EEPROM_WRITE_ENABLE, HIGH);
  delay(15);
}

/**void printContents()
{
  for (int base = 0; base <= 255; base += 16)
  {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1)
    {
      data[offset] = readEEPROM(base + offset);
    }
    char buf[80];

    sprintf(buf, "address: %03x: %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9],
            data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}

unsigned concatenate(int x, int y)
{
  int pow = 10;
  while (y >= pow)
    pow *= 10;
  return x * pow + y;
}
*/

void setup()
{
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA_OUT, OUTPUT);
  pinMode(SHIFT_DATA_IN, INPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_MODE0, OUTPUT);
  pinMode(SHIFT_MODE1, OUTPUT);

  digitalWrite(EEPROM_OUT_ENABLE, HIGH);
  pinMode(EEPROM_OUT_ENABLE, OUTPUT);
  digitalWrite(EEPROM_WRITE_ENABLE, HIGH);
  pinMode(EEPROM_WRITE_ENABLE, OUTPUT);

  //set the address lines to outputs.
  for (int pin = EEPROM_A0; pin <= EEPROM_A7; pin += 1)
  {
    pinMode(pin, OUTPUT);
  }

  Serial.begin(9600);

  Serial.print("Programming EEPROM");
  byte originalData[131][3];
  byte readData[131][3];

  //put the registers into shift right mode:
  digitalWrite(SHIFT_MODE0, HIGH);
  digitalWrite(SHIFT_MODE1, LOW);

  for (uint32_t address = 0; address < sizeof(microCodeData) / sizeof(microCodeData[0]); address += 1)
  {
    byte microInstruction[3] = {byte(microCodeData[address] >> 16), byte(microCodeData[address] >> 8), byte(microCodeData[address])};
    writeEEPROM(address, microInstruction, 3);

    //TODO should I be using an array of pointers?
    //we'll use this to compare after our read.
    originalData[address][0] = microInstruction[0];
    originalData[address][1] = microInstruction[1];
    originalData[address][2] = microInstruction[2];
  }
  //now that we have written all data in the microcode array to the eeprom - we want to read it back
  //and make sure it's the equal to the data we thought we wrote.

  for (uint32_t address = 0; address < sizeof(microCodeData) / sizeof(microCodeData[0]); address += 1)
  {
    byte *currentData = readEEPROM(address, 3);
    readData[address][0] = currentData[0];
    readData[address][1] = currentData[1];
    readData[address][2] = currentData[2];
    delete currentData;
  }
  //now we have built up our data... lets compare them.

  int count = 0;
  for (int address = 0; address < 130; address += 1)
  {
    for (int subByteAddress = 0; subByteAddress < 3; subByteAddress += 1)
    {
      byte originalByte = originalData[address][subByteAddress];
      byte readByte = readData[address][subByteAddress];
      if (originalByte == readByte)
      {
        Serial.println("bytes are the same - good job!");
        Serial.println(originalByte);
      }
      else
      {
        Serial.println("BYTES ARE NOT THE SAME - SOMETHING IS WRONG!!!!");
        Serial.println("address");
        Serial.println(address);
        Serial.println("byte");
        Serial.println(subByteAddress);
        Serial.println("original");
        Serial.println(originalByte);

        Serial.println("actual");

        Serial.println(readByte);
        count = count + 1;
      }
    }
  }
  Serial.println("There were some differences:");
  Serial.println(count);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
