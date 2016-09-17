# API to get weather data
import pyowm
# unique API key
owm = pyowm.OWM('2b14eccb4dae05ee19ca49a8e39fb16e');
# serial for arduino communication
import serial
# time for delays
import time
# datetime for time
import datetime

# connect and mandatory wait
a = serial.Serial('/dev/cu.usbmodem14141', 9600);
time.sleep(2);
# send start message, wait for ack
a.write('a');
time.sleep(1);
a.readline();

# call over and over, but can't do any more than 60 times per minute
while (True):
    # get the current weather in Ithaca
#     print 'Getting weather in Ithaca'
    obs = owm.weather_at_place('ithaca, ny');
    w = obs.get_weather();
    # only want current temperature reading (celsius)
    out_temp = w.get_temperature('celsius')['temp'];
    print out_temp;
    a.write(str(out_temp));
    time.sleep(1);
    print a.readline();
    
    # get the hour
    d = datetime.datetime.now();
    a.write(str(d.hour));
    time.sleep(1);
    a.write(str(d.minute));
    time.sleep(1);
    
#     time.sleep(3);
    
    