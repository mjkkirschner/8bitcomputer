#include <Arduino.h>

#include <Button.h> //https://github.com/JChristensen/Button
#include <sramWriter.h>

#define ClOCK_RUNMODE_BUTTON 2 //Connect a tactile button switch (or something similar)
#define CLOCKPULSE_BUTTON 3    //Connect a tactile button switch (or something similar)

#define RUNMODE_AUTO_LED 4
#define RUNMODE_MANUAL_LED 7

#define CLOCKSIGNAL 8
#define CLOCK_SPEED_IN 3

#define DEBOUNCE_MS 20 //A debounce time of 20 milliseconds usually works well for tactile button switches.

#define PROGRAMMER_WRITE_ENABLE 31
#define RUN_MODE_SIGNAL 33

#define RAM_ADDR_MSB 22
#define RAM_ADDR_LSB 36

#define RAM_DATA_MSB 38
#define RAM_DATA_LSB 52

//when this button is pressed we will take control of the ram
//and write to it some selected array or table.
#define WRITE_TO_SRAM_BUTTON 35

Button runModeButton(ClOCK_RUNMODE_BUTTON, false, true, DEBOUNCE_MS); //Declare the button
Button clockPulseButton(CLOCKPULSE_BUTTON, false, true, DEBOUNCE_MS); //Declare the button
Button writeRAMButton(WRITE_TO_SRAM_BUTTON, false, true, DEBOUNCE_MS);
byte PROGRAM_TABLE[255];
BYTE1_SRAM  ramWriter(RAM_ADDR_MSB,
               RAM_ADDR_LSB,
               RAM_DATA_MSB,
               RAM_DATA_LSB, RUN_MODE_SIGNAL, PROGRAMMER_WRITE_ENABLE,true);

enum RUNMODE
{
    AUTO,
    MANUAL
};

RUNMODE runMode = AUTO; //A variable that keeps the current LED status
int clockspeed = 0;

void generateProgramData(byte *p_array)
{
    for (int i = 0; i < 255; i++)
    {
        p_array[i] = i;
    }
}

void setup(void)
{
    pinMode(CLOCKSIGNAL, OUTPUT);
    pinMode(RUNMODE_MANUAL_LED, OUTPUT);
    pinMode(RUNMODE_AUTO_LED, OUTPUT);
    digitalWrite(CLOCKSIGNAL, LOW);
    digitalWrite(RUNMODE_AUTO_LED, HIGH);
    Serial.begin(9600);
    //create a sram writer

    //setup some test data for the programTable
    generateProgramData(PROGRAM_TABLE);
}

void SetLedsBasedOnRunMode(RUNMODE runMode)
{
    if (runMode == AUTO)
    {
        digitalWrite(RUNMODE_AUTO_LED, HIGH);
        digitalWrite(RUNMODE_MANUAL_LED, LOW);
    }
    else
    {
        digitalWrite(RUNMODE_AUTO_LED, LOW);
        digitalWrite(RUNMODE_MANUAL_LED, HIGH);
    }
}

void loop(void)
{
    clockspeed = analogRead(CLOCK_SPEED_IN);

    runModeButton.read();
    clockPulseButton.read();
    writeRAMButton.read();

    if (runModeButton.wasReleased())
    {
        if (runMode == AUTO)
        {
            runMode = MANUAL;
        }
        else if (runMode == MANUAL)
        {
            runMode = AUTO;
        }
        //light LEDS correctly
        SetLedsBasedOnRunMode(runMode);
    }

    //now if we are in auto mode clock the clock pin
    if (runMode == AUTO)
    {
        int newclockMS = map(clockspeed, 0, 1024, 100, 1000);
        digitalWrite(CLOCKSIGNAL, HIGH);
        //TODO tie this time to the clock speed pin
        delay(newclockMS);
        digitalWrite(CLOCKSIGNAL, LOW);
        delay(newclockMS);
    }
    else
    {
        //the runmode was manual so lets just wait for a trigger and trigger a pulse then
        if (clockPulseButton.isPressed())
        {
            digitalWrite(CLOCKSIGNAL, HIGH);
        }
        else
        {
            digitalWrite(CLOCKSIGNAL, LOW);
        }
        //if the runMode is manual and the write button is pressed
        //then initiate a write from arduino memory
        if (writeRAMButton.wasReleased())
        {  
            //write data to the RAM
            for(int i = 0; i< 255;i++){
                byte data = PROGRAM_TABLE[i];
                ramWriter.writeData(i,data);
            }
            //put the 8bit computer back in contorl
            digitalWrite(RUN_MODE_SIGNAL, HIGH);
        }
    }
}
