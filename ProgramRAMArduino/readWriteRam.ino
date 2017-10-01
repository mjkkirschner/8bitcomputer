//start with MSB at 0
#define RAM_ADDRESS_0 22
#define RAM_ADDRESS_1 24
#define RAM_ADDRESS_2 26
#define RAM_ADDRESS_3 28
#define RAM_ADDRESS_4 30
#define RAM_ADDRESS_5 32
#define RAM_ADDRESS_6 34
#define RAM_ADDRESS_7 36

//start with MSB at 0
#define RAM_DATA_0 38
#define RAM_DATA_1 40
#define RAM_DATA_2 42
#define RAM_DATA_3 44
#define RAM_DATA_4 46
#define RAM_DATA_5 48
#define RAM_DATA_6 50
#define RAM_DATA_7 52

#define RUN_MODE_SIGNAL 2
#define PROGRAMMER_WRITE_ENABLE 3

void setup()
{
    // put your setup code here, to run once:
    pinMode(RUN_MODE_SIGNAL, OUTPUT);
    pinMode(PROGRAMMER_WRITE_ENABLE, OUTPUT);
    digitalWrite(RUN_MODE_SIGNAL, LOW);
    //active low so this puts the chip into read mode.
    digitalWrite(PROGRAMMER_WRITE_ENABLE, HIGH);

    Serial.begin(57600);

    Serial.println("set all address pins to output");
    for (int pin = RAM_ADDRESS_0; pin <= RAM_ADDRESS_7; pin += 2)
    {
        pinMode(pin, OUTPUT);
    }

    Serial.println("set all data pins to output");
    for (int pin = RAM_DATA_0; pin <= RAM_DATA_7; pin += 2)
    {
        pinMode(pin, OUTPUT);
    }

    //iterate through the address lines and set them to the correct state
    for (int address = 0; address < 64; address++)
    {
        Serial.println("******************");
        Serial.println("setting address lines for address");
        Serial.println(address);

        setAddressLines(address);
        //some static number for now
        //TODO make this dynamic for another test type
        setDataLines(address*2);
        delayMicroseconds(5);
        //toggle the write enable low then HIGH
        digitalWrite(PROGRAMMER_WRITE_ENABLE, LOW);
        delayMicroseconds(5);
        digitalWrite(PROGRAMMER_WRITE_ENABLE, HIGH);
        delayMicroseconds(5);
    }
    Serial.println("******************");
    Serial.println("WRITE COMPLETE");
    Serial.println("BEGIN READ");

    printRamContents(64);
}

void printRamContents(int maxAddressToRead)
{

    //make sure we are not writing!
    digitalWrite(PROGRAMMER_WRITE_ENABLE, HIGH);
    delayMicroseconds(5);

    //set all the data pins to high imp state:
    Serial.println("set all data pins to tri state off");
    //for (int pin = RAM_DATA_0; pin <= RAM_DATA_7; pin += 2)
   // {
    //    pinMode(pin, INPUT);
    //    digitalWrite(pin, LOW);
    //    //float you glorious bastard float!
   // }
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

void setAddressLines(int address)
{
    int count = 7;
    //loop all pins
    for (int pin = RAM_ADDRESS_0; pin <= RAM_ADDRESS_7; pin += 2)
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

void setDataLines(int dataToWrite)
{
    int count = 7;
    //loop all pins
    for (int pin = RAM_DATA_0; pin <= RAM_DATA_7; pin += 2)
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

byte readData()
{
    byte data = 0;
    for (int pin = RAM_DATA_0; pin <= RAM_DATA_7; pin += 2)
    {
        Serial.println("pin:");
        Serial.print(pin);
        Serial.print("is:");
        Serial.println(digitalRead(pin));
        data = (data << 1) + digitalRead(pin);
    }
    return data;
}

void loop()
{
    // put your main code here, to run repeatedly:
}