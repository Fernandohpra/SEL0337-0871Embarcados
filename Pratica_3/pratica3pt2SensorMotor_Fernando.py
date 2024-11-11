from gpiozero import DistanceSensor
from gpiozero import Motor
from gpiozero import RGBLED
from gpiozero import Servo
from time import sleep

led = RGBLED(red=17, green=27, blue=22)
servo = Servo(15)
sensor = DistanceSensor(23, 24)

while True:
    servo.min()
    led.color = (0, 1, 0)
    sleep(1)
    led.color = (0, 0, 0)
    sleep(1)
    if sensor.distance <= 0.5:
        while sensor.distance <=0.5:
            try:
                led.color = (1, 1, 0)    
                entrada = int(input('Aceitar peça (1) ou Rejeitar peça (2)?: '))
                if entrada == 1:
                    print('Peça aceita')
                    led.color = (0, 1, 0)
                    servo.max()
                    sleep(5)
                elif entrada == 2:
                    print('Peça rejeitada')
                    led.red = 1
                    sleep(5)
                else:
                    print('Comando inválido, aceite (1) ou rejeite (2) a peça.')    
            except ValueError:
                print('Comando inválido, aceite (1) ou rejeite (2) a peça.')
    else:
        color = (0, 1, 0)
        sleep(1)
        led.color = (0, 0, 0)
        sleep(1)
        
            
         

        
