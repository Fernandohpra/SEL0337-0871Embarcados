from gpiozero import DistanceSensor, RGBLED, Servo, PWMLED, Button
from gpiozero.pins.pigpio import PiGPIOFactory
from time import sleep
import threading
import os
import sys

# Configura��es do hardware
factory = PiGPIOFactory()
servo = Servo(18, pin_factory=factory)
led = RGBLED(17, 27, 22)
sensor = DistanceSensor(23, 24)
ledpwm = PWMLED(15)
button = Button(4, bounce_time=0.1)  # Bot�o no pino 4 com debounce

# Vari�veis e mutex para proteger o acesso � dist�ncia e estado da esteira
distancia = 0.0
distancia_mutex = threading.Lock()
esteira_movendo = True  # Inicialmente a esteira est� se movendo
esteira_mutex = threading.Lock()
ativar_servo = False  # Vari�vel para ativar o servo
servo_mutex = threading.Lock()
parar_manual = False  # Vari�vel para parada manual da esteira
emergency_triggered = False  # Vari�vel para garantir que a parada de emerg�ncia s� ocorra uma vez

# Fun��o para limpar o terminal
def limpar_terminal():
    os.system('cls' if os.name == 'nt' else 'clear')

# Fun��o para mover o cursor no terminal usando ANSI escape codes
def mover_cursor(linha, coluna):
    print(f"\033[{linha};{coluna}H", end='')

# Fun��o para monitorar a dist�ncia e atualizar continuamente
def monitorar_distancia():
    global distancia
    while True:
        with distancia_mutex:
            distancia = sensor.distance
        # Atualiza a dist�ncia na primeira linha
        mover_cursor(1, 0)
        print(f"Dist�ncia: {distancia:.2f} m")
        sleep(0.1)

# Fun��o para controlar o servo da esteira (executada em uma thread separada)
def mover_servo():
    global ativar_servo
    while True:
        with servo_mutex:
            if ativar_servo:
                servo.max()  # Move o servo para aceitar a pe�a
                sleep(1)
                servo.min()  # Retorna o servo para a posi��o original
                sleep(1)
                ativar_servo = False  # Desativa a ativa��o do servo ap�s o movimento

# Fun��o para controlar a esteira, LEDs e decis�o de aceitar/rejeitar
def controlar_esteira():
    global distancia, esteira_movendo, ativar_servo, parar_manual
    while True:
        with distancia_mutex:
            dist_atual = distancia

        # A esteira para quando a pe�a est� pr�xima (dist�ncia <= 0.5) ou em caso de parada manual
        if dist_atual <= 0.5 or parar_manual:
            with esteira_mutex:
                esteira_movendo = False  # A esteira para enquanto a pe�a est� pr�xima ou em parada manual
            led.color = (1, 1, 0)  # Amarelo quando a pe�a est� perto ou parada manual

            # Pergunta ao usu�rio se deve aceitar ou rejeitar a pe�a, exceto em parada manual
            if not parar_manual:
                try:
                    mover_cursor(2, 0)  # Move o cursor para a segunda linha
                    entrada = int(input('Aceitar pe�a (1) ou Rejeitar pe�a (2)?: '))
                    mover_cursor(2, 0)  # Limpa a linha de input ap�s a resposta
                    print(" " * 50, end='\r')

                    if entrada == 1:
                        print('Pe�a aceita')
                        led.color = (0, 1, 0)  # Verde para aceitar
                        sleep(1)
                        with servo_mutex:
                            ativar_servo = True  # Ativa o servo na thread separada
                    elif entrada == 2:
                        print('Pe�a rejeitada')
                        led.color = (1, 0, 0)  # Vermelho para rejeitar
                        sleep(1)
                    else:
                        print('Comando inv�lido, aceite (1) ou rejeite (2) a pe�a.')
                except ValueError:
                    print('Comando inv�lido, aceite (1) ou rejeite (2) a pe�a.')

            # Depois da aceita��o ou rejei��o, ou ap�s a ativa��o manual, a esteira volta a se mover
            with esteira_mutex:
                esteira_movendo = True
                parar_manual = False  # Reseta a parada manual ap�s o fim do processo

        # Controle da movimenta��o da esteira e LED PWM
        with esteira_mutex:
            if esteira_movendo:
                ledpwm.pulse()  # LED PWM pulsando enquanto a esteira se move
            else:
                ledpwm.on()  # LED PWM ligado constantemente quando a esteira est� parada

        sleep(0.1)

# Fun��o para parar a esteira e mover o servo manualmente
def parada_manual():
    global parar_manual, ativar_servo, emergency_triggered
    # Verifica se o bot�o de emerg�ncia j� foi acionado
    if not emergency_triggered:
        led.color = (1, 1, 1)
        with esteira_mutex:
            parar_manual = True  # Ativa a parada manual da esteira
        print("Parada manual da esteira. Servo ser� ativado.")
        with servo_mutex:
            ativar_servo = True  # Ativa o servo para movimenta��o manual
        emergency_triggered = True  # Define que o ciclo foi acionado

# Fun��o para resetar o estado de emerg�ncia ap�s o bot�o ser liberado
def resetar_parada_emergencia():
    global emergency_triggered
    emergency_triggered = False  # Reseta o estado, permitindo outro acionamento no futuro

# Cria��o e inicializa��o das threads
thread_distancia = threading.Thread(target=monitorar_distancia)
thread_esteira = threading.Thread(target=controlar_esteira)
thread_servo = threading.Thread(target=mover_servo)

# Inicializa as threads
thread_distancia.start()
thread_esteira.start()
thread_servo.start()

# Configura o bot�o para ativar a parada manual apenas uma vez por pressionamento
button.when_pressed = parada_manual
button.when_released = resetar_parada_emergencia  # Reseta o ciclo quando o bot�o � liberado

# Limpa o terminal e prepara a exibi��o
limpar_terminal()

# Mant�m o programa rodando at� que as threads terminem (nunca terminam neste caso)
thread_distancia.join()
thread_esteira.join()
thread_servo.join()
