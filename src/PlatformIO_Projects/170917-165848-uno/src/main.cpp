#include <Arduino.h>

#include <Button.h> //https://github.com/JChristensen/Button
#include <sramWriter.h>

#define RUNMODE_BUTTON 2    //Connect a tactile button switch (or something similar)
#define CLOCKPULSE_BUTTON 3 //Connect a tactile button switch (or something similar)

#define RUNMODE_AUTO_LED 4
#define RUNMODE_MANUAL_LED 7

#define CLOCKSIGNAL 8
#define CLOCK_SPEED_IN 3

#define DEBOUNCE_MS 20 //A debounce time of 20 milliseconds usually works well for tactile button switches.

Button runModeButton(RUNMODE_BUTTON, false, true, DEBOUNCE_MS);       //Declare the button
Button clockPulseButton(CLOCKPULSE_BUTTON, false, true, DEBOUNCE_MS); //Declare the button

enum RUNMODE
{
    AUTO,
    MANUAL
};

RUNMODE runMode = AUTO; //A variable that keeps the current LED status
int clockspeed = 0; 

void setup(void)
{
    pinMode(CLOCKSIGNAL, OUTPUT);
    pinMode(RUNMODE_MANUAL_LED, OUTPUT);
    pinMode(RUNMODE_AUTO_LED, OUTPUT);
    digitalWrite(CLOCKSIGNAL, LOW);
    digitalWrite(RUNMODE_AUTO_LED, HIGH);
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
        int newclockMS = map(clockspeed,0,1024,100,1000);
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
        else{
            digitalWrite(CLOCKSIGNAL, LOW);
        }
    }
}
