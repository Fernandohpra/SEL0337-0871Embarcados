# Tutorial: Raspberry Pi controlando o Arduino
# sudo raspi-config - interface options - habilitar I2C
# sudo i2cdetect -y 1 - verifica o barramento I2C - ao realizar a conexão I2C física com
Arduino o endereço ocupado deve aparecer
from smbus import SMBus # importa a classe SMBus
addr = 0x8 # bus address - define o endereço do dispositivo I2C como 0x8 (hexa)
bus = SMBus(1) # /dev/ic2-1 - inicializa o objeto, criando uma instância para
flag = True # variv. usada p/ controlar o loop
print ("Digite 1 para ON ou 0 para OFF")
while flag:
    ledstate = input(">>>> ") # solicita entrada no terminal
    if ledstate == "1":
        bus.write_byte(addr, 0x1) # se for 1, escreve 0x1 no endereço
    elif ledstate == "0":
        bus.write_byte(addr, 0x0) # ou escreve 0x0, caso 0
    else:
        flag = False # encerra o loop caso digite algo diferente de 0 ou 1

