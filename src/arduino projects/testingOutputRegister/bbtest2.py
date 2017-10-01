import Adafruit_BBIO.GPIO as GPIO
 
GPIO.setup("P8_15", GPIO.OUT)
GPIO.setup("P8_16", GPIO.OUT)
GPIO.output("P8_16",GPIO.LOW)
GPIO.output("P8_16",GPIO.LOW)

#GPIO.cleanup()