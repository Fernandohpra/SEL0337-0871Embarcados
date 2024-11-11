
import lgpio
from time import sleep

LED_PIN = 15
BUTTON_PIN = 4
chip = lgpio.gpiochip_open(0)

lgpio.gpio_claim_output(chip, LED_PIN)
lgpio.gpio_claim_input(chip, BUTTON_PIN)

def button_state(pin):
    state = lgpio.gpio_read(chip, pin)
    return state
    
def button_control():
    while True:
        if button_state(BUTTON_PIN):
            lgpio.gpio_write(chip, LED_PIN, 0)
        else:
            lgpio.gpio_write(chip, LED_PIN, 1)
        sleep(0.1)

def contagem_regressiva(tempo):
    minutos, segundos =divmod(tempo,60)
    while tempo:
        print(f'{minutos:02d}:{segundos:02d}', end= '\r')
        sleep(1)
        tempo -= 1
        minutos, segundos =divmod(tempo,60)
    print("\nAcendendo LED.")
    lgpio.gpio_write(chip, LED_PIN, 1)
    
def receber_tempo():
    while True:
        try:
            tempo = int(input("Digite o tempo em segundos: "))
            if tempo <= 0:
                raise ValueError("O numero deve ser positivo")
            return tempo
        except ValueError as e:
            print(f"Erro, insira um numero valido")
            
def set_mode(modo):
    if modo == 1:
        print("Modo 1: LED ao pressionar o botao")
        button_control()
    
    elif modo ==2:
        print("Modo 2: LED com delay ao pressionar o botao")
        tempo = receber_tempo()
        print("Pressione o botao para iniciar a contagem")
        while button_state(BUTTON_PIN):
            sleep(0.1)
        print("Iniciando contagem")
        contagem_regressiva(tempo)
    else:
        print("Entrada invalida, selecione 1 ou 2.")
        
        
entrada = int(input("Digite 1 para botao normal e 2 para botao com delay: "))
set_mode(entrada)
try:
    while True:
        sleep(0.1)
except KeyboardInterrupt:
    print("Encerrando o programa")
finally:
    lgpio.gpiochip_close(chip)
