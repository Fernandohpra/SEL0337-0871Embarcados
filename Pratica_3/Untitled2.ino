from gpiozero import DistanceSensor, RGBLED, Servo, PWMLED, Button
from gpiozero.pins.pigpio import PiGPIOFactory
from time import sleep
import threading
import os
import sys

# Configurações do hardware
factory = PiGPIOFactory()
servo = Servo(18, pin_factory=factory)
led = RGBLED(17, 27, 22)
sensor = DistanceSensor(23, 24)
ledpwm = PWMLED(15)
button = Button(4, bounce_time=0.1)  # Botão no pino 4 com debounce

# Variáveis e mutex para proteger o acesso à distância e estado da esteira
distancia = 0.0
distancia_mutex = threading.Lock()
esteira_movendo = True  # Inicialmente a esteira está se movendo
esteira_mutex = threading.Lock()
ativar_servo = False  # Variável para ativar o servo
servo_mutex = threading.Lock()
parar_manual = False  # Variável para parada manual da esteira
emergency_triggered = False  # Variável para garantir que a parada de emergência só ocorra uma vez

# Função para limpar o terminal
def limpar_terminal():
    os.system('cls' if os.name == 'nt' else 'clear')

# Função para mover o cursor no terminal usando ANSI escape codes
def mover_cursor(linha, coluna):
    print(f"\033[{linha};{coluna}H", end='')

# Função para monitorar a distância e atualizar continuamente
def monitorar_distancia():
    global distancia
    while True:
        with distancia_mutex:
            distancia = sensor.distance
        # Atualiza a distância na primeira linha
        mover_cursor(1, 0)
        print(f"Distância: {distancia:.2f} m")
        sleep(0.1)

# Função para controlar o servo da esteira (executada em uma thread separada)
def mover_servo():
    global ativar_servo
    while True:
        with servo_mutex:
            if ativar_servo:
                servo.max()  # Move o servo para aceitar a peça
                sleep(1)
                servo.min()  # Retorna o servo para a posição original
                sleep(1)
                ativar_servo = False  # Desativa a ativação do servo após o movimento

# Função para controlar a esteira, LEDs e decisão de aceitar/rejeitar
def controlar_esteira():
    global distancia, esteira_movendo, ativar_servo, parar_manual
    while True:
        with distancia_mutex:
            dist_atual = distancia

        # A esteira para quando a peça está próxima (distância <= 0.5) ou em caso de parada manual
        if dist_atual <= 0.5 or parar_manual:
            with esteira_mutex:
                esteira_movendo = False  # A esteira para enquanto a peça está próxima ou em parada manual
            led.color = (1, 1, 0)  # Amarelo quando a peça está perto ou parada manual

            # Pergunta ao usuário se deve aceitar ou rejeitar a peça, exceto em parada manual
            if not parar_manual:
                try:
                    mover_cursor(2, 0)  # Move o cursor para a segunda linha
                    entrada = int(input('Aceitar peça (1) ou Rejeitar peça (2)?: '))
                    mover_cursor(2, 0)  # Limpa a linha de input após a resposta
                    print(" " * 50, end='\r')

                    if entrada == 1:
                        print('Peça aceita')
                        led.color = (0, 1, 0)  # Verde para aceitar
                        sleep(1)
                        with servo_mutex:
                            ativar_servo = True  # Ativa o servo na thread separada
                    elif entrada == 2:
                        print('Peça rejeitada')
                        led.color = (1, 0, 0)  # Vermelho para rejeitar
                        sleep(1)
                    else:
                        print('Comando inválido, aceite (1) ou rejeite (2) a peça.')
                except ValueError:
                    print('Comando inválido, aceite (1) ou rejeite (2) a peça.')

            # Depois da aceitação ou rejeição, ou após a ativação manual, a esteira volta a se mover
            with esteira_mutex:
                esteira_movendo = True
                parar_manual = False  # Reseta a parada manual após o fim do processo

        # Controle da movimentação da esteira e LED PWM
        with esteira_mutex:
            if esteira_movendo:
                ledpwm.pulse()  # LED PWM pulsando enquanto a esteira se move
            else:
                ledpwm.on()  # LED PWM ligado constantemente quando a esteira está parada

        sleep(0.1)

# Função para parar a esteira e mover o servo manualmente
def parada_manual():
    global parar_manual, ativar_servo, emergency_triggered
    # Verifica se o botão de emergência já foi acionado
    if not emergency_triggered:
        led.color = (1, 1, 1)
        with esteira_mutex:
            parar_manual = True  # Ativa a parada manual da esteira
        print("Parada manual da esteira. Servo será ativado.")
        with servo_mutex:
            ativar_servo = True  # Ativa o servo para movimentação manual
        emergency_triggered = True  # Define que o ciclo foi acionado

# Função para resetar o estado de emergência após o botão ser liberado
def resetar_parada_emergencia():
    global emergency_triggered
    emergency_triggered = False  # Reseta o estado, permitindo outro acionamento no futuro

# Criação e inicialização das threads
thread_distancia = threading.Thread(target=monitorar_distancia)
thread_esteira = threading.Thread(target=controlar_esteira)
thread_servo = threading.Thread(target=mover_servo)

# Inicializa as threads
thread_distancia.start()
thread_esteira.start()
thread_servo.start()

# Configura o botão para ativar a parada manual apenas uma vez por pressionamento
button.when_pressed = parada_manual
button.when_released = resetar_parada_emergencia  # Reseta o ciclo quando o botão é liberado

# Limpa o terminal e prepara a exibição
limpar_terminal()

# Mantém o programa rodando até que as threads terminem (nunca terminam neste caso)
thread_distancia.join()
thread_esteira.join()
thread_servo.join()
