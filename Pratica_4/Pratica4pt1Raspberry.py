from smbus import SMBus

addr = 0x8
bus = SMBus(1)


while True:
    data = bus.read_i2c_block_data(addr, 0, 2)
    value = data[0]*256+data[1]
    print(value, end='\r')
 