from serial import Serial

ser = Serial("/dev/ttyACM0")

buff = bytearray([0x10]);

ser.write(buff)

sync = ser.read(2);

if sync == bytearray([0x15, 0x06]):
    print("sync ok")
