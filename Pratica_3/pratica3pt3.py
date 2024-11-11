from gpiozero import DistanceSensor, RGBLED, Servo, PWMLED, Button
from time import sleep
import threading

# Configurações do hardware
servo = Servo(18)
led = RGBLED(17, 27, 22)
sensor = DistanceSensor(23, 24)
ledpwm = PWMLED(15)
button = Button(4)  # Botão no pino 4

# Variáveis e mutex para proteger o acesso à distância e estado da esteira
distancia = 0.0
distancia_mutex = threading.Lock()
esteira_movendo = True  # Inicialmente a esteira está se movendo
esteira_mutex = threading.Lock()
ativar_servo = False  # Variável para ativar o servo
servo_mutex = threading.Lock()
parar_manual = False  # Variável para parada manual da esteira

# Função para monitorar a distância e atualizar continuamente
def monitorar_distancia():
    global distancia
    while True:
        with distancia_mutex:
            distancia = sensor.distance
        print('Distância:', distancia, 'm', end='\r')
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
                    entrada = int(input('Aceitar peça (1) ou Rejeitar peça (2)?: '))
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
    global parar_manual, ativar_servo
    led.color = (1, 1, 1)
    with esteira_mutex:
        parar_manual = True  # Ativa a parada manual da esteira
    print("Parada manual da esteira. Servo será ativado.")
    with servo_mutex:
        ativar_servo = True  # Ativa o servo para movimentação manual
        

# Criação e inicialização das threads
thread_distancia = threading.Thread(target=monitorar_distancia)
thread_esteira = threading.Thread(target=controlar_esteira)
thread_servo = threading.Thread(target=mover_servo)

# Inicializa as threads
thread_distancia.start()
thread_esteira.start()
thread_servo.start()

# Configura o botão para ativar a parada manual quando pressionado
button.when_pressed = parada_manual

# Mantém o programa rodando até que as threads terminem (nunca terminam neste caso)
thread_distancia.join()
thread_esteira.join()
thread_servo.join()
