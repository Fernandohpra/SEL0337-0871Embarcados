from gpiozero import DistanceSensor
from gpiozero import RGBLED



led = RGBLED(17, 27, 22) 
sensor = DistanceSensor(23, 24) 

# Loop contínuo para monitorar a distância e alterar a cor do LED
while True:
    # Exibe a distância do objeto mais próximo no terminal, sobrescrevendo a linha anterior
    print('Distance to nearest object is', sensor.distance, 'm', end='\r')
    
    # Se a distância for menor ou igual a 0.5 metros, define a cor do LED como vermelho
    if sensor.distance <= 0.5:
        led.color = (1, 0, 0)  # Cor vermelha
    
    # Se a distância estiver entre 0.5 e 1 metro, define a cor do LED como amarelo
    elif sensor.distance > 0.5 and sensor.distance <= 1:
        led.color = (1, 1, 0)  # Cor amarela
    
    # Se a distância for maior que 1 metro, define a cor do LED como verde
    else:
        led.color = (0, 1, 0)  # Cor verde

            
         

        
