from gpiozero import PWMLED
from time import sleep

led = PWMLED(15)
brilho = 0
entrada = 0
while True:
    try:
        print(f'Brilho atual: {entrada}%')
        entrada = int(input("\nInsira o valor do brilho do LED (0-100): "))
        if 0 <= entrada <= 100:
            brilho = entrada/100
            led.value = brilho
            sleep(1)
        else:
            print("Insira um valor entre 0 e 100")
    except ValueError:
        print("Entrada inválida. A entrada deve ser um número inteiro de 0 a 100")
