# Librairies Importé
from machine import Pin
import time
import neopixel
import network, espnow

# Entrée des Switchs
PIN_S1 = 4     # Switch S1
PIN_S2 = 5     # Switch S2

# Sortie des LEDs
PIN_D1 = 6     # LED D1
PIN_RGB = 48   # LED RGB

# Initialisation des GPIOs
s1 = Pin(PIN_S1, Pin.IN)
s2 = Pin(PIN_S2, Pin.IN)
led_d1 = Pin(PIN_D1, Pin.OUT)

# Initialisation de la LED RGB
np = neopixel.NeoPixel(Pin(PIN_RGB), 1)

# Variables pour la logique et l'antirebond
d1_state = 0
rgb_index = 0
colors = [(16, 0, 0), (0, 16, 0), (0, 0, 16)] # R, G, B
debounce_s1 = 0
debounce_s2 = 0
debounce_delay = 50 # 50 ms d'antirebond
time_out = 0 

# Variables de detection de flanc
last_S1_Value = 0
last_S2_Value = 0

# Variable du mode Remote
Remote = False
blink_time = 0
blink = False
send = 0 

# Initialisation réseau
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
e = espnow.ESPNow()
e.active(True)

# Adresse de diffusion (Broadcast)
broadcast_mac = b'\xff\xff\xff\xff\xff\xff'
e.add_peer(broadcast_mac)
PASSWORD = b"ESP_32_REMOTE"
COLLOR = b"CHANGE"

# Boucle Infinie
while True:
    now = time.ticks_ms()

    # Envoi du Mot de passe toutes les secondes
    if time.ticks_diff(now, send) > 1000:
        e.send(broadcast_mac, PASSWORD, False)
        send = now
        
    # Reception du message
    host, msg = e.recv(0) 
    
    # Si le message reçu est le Mot de passe
    if msg == PASSWORD:
        # Passage en Mode Remote
        Remote = True
        # Reset du Timer
        time_out = now
        
    # Si le message reçu est le changement de couleur
    if msg == COLLOR:
        # Changement de couleur de D2
        rgb_index = (rgb_index + 1) % 3
        np[0] = colors[rgb_index]
        np.write()
                
    # Detection de flanc sur S1
    if last_S1_Value == 1 and s1.value() == 0:
        # Antirebond sur S1
        if time.ticks_diff(now, debounce_s1) > debounce_delay:
            # Changement d'état de D1
            d1_state = not d1_state
            led_d1.value(d1_state)
            # Reset du temps d'Anti rebond sur S1
            debounce_s1 = now

    # Mode Remote
    if Remote:
        # Détection de flanc sur S2
        if last_S2_Value == 1 and s2.value() == 0:
            # Antirebond sur S2
            if time.ticks_diff(now, debounce_s2) > debounce_delay:
                # Envoi le message de changement de couleur
                e.send(broadcast_mac, COLLOR, False)
                # Reset du temps d'Anti rebond sur S2
                debounce_s2 = now
        
        # Clignotement 2Hz 
        if time.ticks_diff(now, blink_time) > 250:
            blink = not blink
            if blink:
                np[0] = colors[rgb_index]
            else:
                np[0] = (0, 0, 0)
            np.write()    
            blink_time = now
            
        # Au bout de 1.5 secondes
        if time.ticks_diff(now, time_out) > 1500:
            # Passage en Mode Local
            Remote = False
            # LED S2 Fixer a la derniere couleur pendant le Mode Remote
            np[0] = colors[rgb_index] 
            np.write()
        
    # Mode Local
    else:
        # Détection de flanc sur S2
        if last_S2_Value == 1 and s2.value() == 0:
            # Antirebond sur S2
            if time.ticks_diff(now, debounce_s2) > debounce_delay:
                # Changement de couleur de D2
                rgb_index = (rgb_index + 1) % 3
                np[0] = colors[rgb_index]
                np.write()
                # Reset du temps d'Anti rebond sur S2
                debounce_s2 = now

    # Mise à jour des états
    last_S1_Value = s1.value()
    last_S2_Value = s2.value()
    time.sleep_ms(1)