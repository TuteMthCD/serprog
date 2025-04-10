from serial import Serial
from time import sleep

ser = Serial("/dev/ttyACM0")

buff = bytearray([0x03, 0xA]);

ser.write(buff)
