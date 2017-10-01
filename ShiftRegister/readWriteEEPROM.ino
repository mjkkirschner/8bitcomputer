
//code inspired - basically copied ;) from Ben Eater's fantastic 8bit computer series
//which references A Malvino's Digital Computer Electronics.

#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define EEPROM_WRITE_ENABLE 4
#define EEPROM_OUT_ENABLE 5

#define EEPROM_D0 6
#define EEPROM_D7 13

void shiftOut2(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, byte val)
{
  int i;

  for (i = 0; i < 8; i++)
  {
    if (bitOrder == LSBFIRST)
      digitalWrite(dataPin, !!(val & (1 << i)));
    else
      digitalWrite(dataPin, !!(val & (1 << (7 - i))));

    digitalWrite(clockPin, HIGH);
    delay(100);
    digitalWrite(clockPin, LOW);
  }
}

//shift 16bits of address into the registers
void setAddress(int address, bool write)
{

  //set the out enable on the eeprom chip to read or write before
  //setting the address pins
  digitalWrite(EEPROM_OUT_ENABLE, write);

  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address >> 8);
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);
}

byte readEEPROM(int address)
{
  Serial.println("read");

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1)
  {
    pinMode(pin, INPUT);
  }
  //false should put OE low which means read.
  setAddress(address, false);
  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1)
  {
    //Serial.println(digitalRead(pin));
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

void writeEEPROM(int address, byte data)
{
  Serial.println("write");
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1)
  {
    pinMode(pin, OUTPUT);
  }
  //true should put OE high which means write.
  setAddress(address, true);

  //TODO what does this do!?
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1)
  {
    Serial.println(data & 1);
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  delayMicroseconds(1);
  digitalWrite(EEPROM_WRITE_ENABLE, LOW);
  delayMicroseconds(1);
  digitalWrite(EEPROM_WRITE_ENABLE, HIGH);
  delay(5);
}

void printContents()
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

void setup()
{
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(EEPROM_OUT_ENABLE, OUTPUT);
  digitalWrite(EEPROM_WRITE_ENABLE, HIGH);
  pinMode(EEPROM_WRITE_ENABLE, OUTPUT);

  Serial.begin(57600);
  //lets clear the whole EEPROM to make sure we are reading it right.
  for (int add1 = 0; add1 < 64; add1 += 1)
  {
    for (int add2 = 0; add2 < 64; add2 += 1)
    {
      int fulladd = add1 << 6 | add2;
      Serial.println(fulladd, BIN);

      //writeEEPROM(fulladd, add1 + add2);
    }
  }
  printContents();
  byte shouldBe = readEEPROM(1701);
  Serial.println(shouldBe, BIN);
  Serial.println(shouldBe);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
