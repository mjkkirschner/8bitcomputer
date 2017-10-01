import Adafruit_BBIO.GPIO as GPIO
import time

MSBDATA = 11;
LSBDATA = 18;
PORT = "P8";
PORT2 = "P9";


clock = 11;
load = 12;

GPIO.setup(PORT2+"_"+str(clock).zfill(2), GPIO.OUT)
GPIO.setup(PORT2+"_"+str(load).zfill(2), GPIO.OUT)

GPIO.output(PORT2+"_"+str(load).zfill(2),GPIO.LOW)
GPIO.output(PORT2+"_"+str(clock).zfill(2),GPIO.LOW)

def setDataLinesToValue(value):
    count=0
    for pin in range(MSBDATA,LSBDATA+1):
        bitstate = get_bit(value,count)
        print("setting" + PORT + "_"+str(pin).zfill(2) + "to: "+ str(bitstate))
        GPIO.output(PORT + "_"+str(pin).zfill(2),bitstate);
        count = count + 1


def bitfield(n):
    return [1 if digit=='1' else 0 for digit in bin(n).zfill(10)[2:]]

def get_bit(byteVal,idx):
    byteArray = bitfield(byteVal);
    print(byteArray)
    return byteArray[idx];
##loop through the pins and set pinmode output
for pin in range(MSBDATA,LSBDATA+1):
    print("setting   " + PORT + "_"+str(pin).zfill(2) + "    to: output")
    GPIO.setup(PORT+"_"+str(pin).zfill(2), GPIO.OUT)


for data in range(0,255):
    print(data)
    time.sleep(.5)

    setDataLinesToValue(data)
    print("setting clock and load high");
    GPIO.output(PORT2+"_"+str(clock).zfill(2),GPIO.HIGH)
    GPIO.output(PORT2+"_"+str(load).zfill(2),GPIO.HIGH)
    time.sleep(.2)
    print("setting clock and load low");
    GPIO.output(PORT2+"_"+str(load).zfill(2),GPIO.LOW)
    GPIO.output(PORT2+"_"+str(clock).zfill(2),GPIO.LOW)
    

GPIO.cleanup()