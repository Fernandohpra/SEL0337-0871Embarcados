import RPi.GPIO as GPIO
from gpiozero import RGBLED
from time import sleep
from mfrc522 import SimpleMFRC522
from colorzero import Color
led = RGBLED(17, 27, 22) 

leitor = SimpleMFRC522()

def autenticador_rfid():
    print("Aproxime a tag RFID")
    try:   
        while True: 
                led.color = Color('yellow')
                print("Aproxime a tag do leitor para leitura.\n") 
                id,texto = leitor.read()
                print("ID: {}\nTexto: {}".format(id, texto))
                if id == 497386497680:
                    print("Acesso liberado.")
                    led.color = Color('green')
                    return 0
                else:
                    print("Acesso Negado. Tente novamente.")
                    led.color = Color('red')
                    sleep(3) 
    except KeyboardInterrupt:
         pass
    finally:
         GPIO.cleanup()


if __name__ == "__main__":
    autenticador_rfid()