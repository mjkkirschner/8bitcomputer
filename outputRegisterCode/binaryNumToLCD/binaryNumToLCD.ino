/******************************************************************************
   TeensyView_Demo.ino
   SFE_TeensyView Library Demo
   Jim Lindblom @ SparkFun Electronics
   Original Creation Date: October 27, 2014
   Modified Febuary 2, 2017

   This sketch uses the TeensyView library to draw a 3-D projected
   cube, and rotate it along all three axes.

   Development environment specifics:
   Arduino IDE 1.6.12 w/ Teensyduino 1.31
   Arduino IDE 1.8.1 w/ Teensyduino 1.35
   TeensyView v1.0

   This code is beerware; if you see me (or any other SparkFun employee) at the
   local, and you've found our code helpful, please buy us a round!

   Distributed as-is; no warranty is given.
 ******************************************************************************/
#include <TeensyView.h>  // Include the SFE_TeensyView library

///////////////////////////////////
// TeensyView Object Declaration //
///////////////////////////////////
//Standard
#define PIN_RESET 15
#define PIN_DC    5
#define PIN_CS    10
#define PIN_SCK   13
#define PIN_MOSI  11

#define REG_0 23
#define REG_1 22
#define REG_2 21
#define REG_3 20

#define REG_4 19
#define REG_5 18
#define REG_6 17
#define REG_7 16

#define POT_SIG 14

int LASTPOTVALUE = 0;
byte memory[1000];

// we only increment this when we have loaded new data into
// the output register, and should actually store that as a past value.
//this will act as our last index pointer into the memory array.
int LASTMEMORYINDEX = 0;
byte LASTMEMORYVALUE = 0;


//assemble a byte by adding and shifting bits from the register pins.
byte readData()
{
  byte data = 0;
  for (int pin = REG_0; pin >= REG_7; pin -= 1)
  {
    Serial.println("pin:");
    Serial.print(pin);
    Serial.print("is:");
    Serial.println(digitalRead(pin));
    data = (data << 1) + digitalRead(pin);
  }
  Serial.println(data);
  return data;
}

TeensyView oled(PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);

void setup()
{
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  
  Serial.println("set all data pins to output");
  for (int pin = REG_0; pin >= REG_7; pin -= 1)
  {
    pinMode(pin, INPUT_PULLDOWN);
  }

  Serial.println("set pot pin as input");
  pinMode(POT_SIG, INPUT);

  //take first reading and save it.
  LASTMEMORYVALUE = readData();

    
  printTitle("Booting UP!!", 1);
  oled.clear(PAGE); // Clear the buffer.

  
  
  
}

void displayRegisterDataAsDec()
{
  // Demonstrate font 2. 10x16. Only numbers and '.' are defined.
  // This font looks like 7-segment displays.
  // Lets use this big-ish font to display readings from the
  // analog pins.
  oled.setCursor(0, 0);        // Set cursor to top-left
  oled.clear(PAGE);
  oled.setFontType(2);         // 7-segment font
  oled.print(readData());  // Print a0 reading
  oled.display();
}



void loop()
{
  //poll the data lines - 
  byte currentmemoryvalue = readData();
  if(currentmemoryvalue != LASTMEMORYVALUE){
    //store the value in memory
    memory[] = 
    LASTMEMORYVALUE = currentmemoryvalue;
  }
  
  // if the value on the data lines after reading is different from the last value
  displayRegisterDataAsDec();
}

// Center and print a small title
// This function is quick and dirty. Only works for titles one
// line long.
void printTitle(String title, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (title.length() / 2)),
                 middleY - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print(title);
  oled.display();
  delay(1500);
  oled.clear(PAGE);
}
