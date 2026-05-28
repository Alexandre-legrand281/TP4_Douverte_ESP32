# TP4_Douverte_ESP32
## Visual Studio Code
### 3.1 Allumer une LED sur GPIO

### 3.2 Allumer une LED RGB

## Micro Python Code

Afin de développer mon code, j'ai du d'installer Thonny et Flasher le logiciel pour pouvoir coder en MicroPython. Apres avoir branché l'ESP32, j'ai du d'acceder aux options de Thonny, puis interpreter le type "MicroPython (ESP32)", et séléctionner le port ou est branché l'ESP32. Pour savoir a quel port est brancher l'ESP32, j'ai du d'acceder au Gestionnaire de périphériques (dans mon cas c'est le COM14).

![Options](/Images/Option_de_Thonny.png)

![Peripheriques](/Images/Gestionnaire_de_peripheriques.png)

Une fois cette etape effectué, il faut cliquer sur "Installer ou metre a jour MicroPython". Une fenetre va allors s'ouvrir, et il va faloir selectionner a nouveau le port ou ce situe mon ESP32 dans "Target port". Il faudras egalement selectionner la famille de MicroPython, la varainte et la version, au quel j'ai respectivement configurer de la maniere ci-dessous.

![Install](/Images/Installation_MicroPython.png)

ATTENTION: Il ce peut que l'installation ne puisse pas etre effectué, car il y aurais deja un autre programme installer dessus. Si ce probleme surgit alors faut Resset l'ESP32. Pour faire ceci, il faut maintenir appuyer sur le bouton "BOOT" puis appuyer sur le bouton "RESET". L'ESP32 va alors s'eteindre, puis ce reallumeras quelques secondes apres. Vous pouvez maintenant poursuivre l'installation.

### 3.1 Allumer une LED sur GPIO

Pour cette partie la, je devais selon le cahier des charges allumer la LED D1 de la carte lorsque j'appuie sur le bouton S1. La premiere etape que j'ai du effectuer, c'etait d'importer la librairie nécessaire pour intéragir avec les GPIO, ainsi qu'une deuxieme librairie pour ajouter un systéme d'Antirebond sur les intérrupteur.

    # Librairies Importé
    from machine import Pin
    import time

Par la suite, j'ai du de regarder dans le Schéma de la carte quels GPIO etait relier a S1 et D1. Dans mon cas S1 est relier au GPIO4 et D1 à GPIO6.

![Schema](/Images/LED_et_Switch_Schema.png)

Une fois ceci determiné, je vais définir mon switch S1 et LED D1, puis initialiser les GPIO grace à la fonction "Pin", et avec comme parametre la définition du switch, ou de la LED, puis "Pin.IN" pour configurer une entrée ou "Pin.OUT" pour configurer une sortie.

    # Entrée des Switchs
    PIN_S1 = 4    # Switch S1

    # Sortie des LEDs
    PIN_D1 = 6    # LED D1

    # Initialisation des GPIOs
    s1 = Pin(PIN_S1, Pin.IN)
    led_d1 = Pin(PIN_D1, Pin.OUT)

Par la suite, je vais devoir initialiser une variable pour la detection de flanc de S1, une autre pour le systéme d'antirebonde de S1 et également une derniere pour le changement d'état de D1.

    # Variables pour la logique
    d1_state = False

    # Variables de detection de flanc et l'antirebond
    last_S1_Value = 0
    debounce_s1 = 0

Maintenant dans ma boucle infinit, je vais avant tout placer un point de repere temporelle. Cella permetras de metre en oeuvre mon systeme d'antirebond.

    # Boucle Infinie
    While True:
        # Point de repere du temps actuelle en ms
        now = time.ticks_ms()

Pour la detection de flanc de S1, je vais devoir utiliser la variable "last_S1_Value" ainsi que la variable "s1" et de la fonction ".value()". Mécaniquement lorsque j'appuie sur le bouton S1, il y a un flanc descendant, donc en language programmation il faut que mon ancien état de S1 soit à 1 et l'état actuelle à 0. La fonction ".value()" va determiner quel est l'état actuelle de "s1" et à la fin de ma boucle infinit je dois enregistré l'état actuelle dans "last_S1_Value", pour que au debut de la prochaine boucle "last_S1_Value" contienne l'ancien état de "s1". J'ajoute également a la fin de ma boucle infinit la fonction "time.sleep_ms(1)", qui permet a de stabiliser mon ESP32.

    # Boucle Infinie
    While True:
        # Point de repere du temps actuelle en ms
        now = time.ticks_ms()

        # Detection de flanc sur S1
        if last_S1_Value == 1 and s1.value() == 0:

        ...

        # Mise à jour des états
        last_S1_Value = s1.value()

        # Temps de repos de 1 ms
        time.sleep_ms(1)

Le systéme d'antirebond ce fait a l'interieur de mon "if" pour la detection de flanc. Pour ceci je vais utiliser la fonction "time.ticks_diff", la variable "debounce_s1" et "now". Ce que je vais faire ce seras de differentier le temps entre l'instant ou il y a un premier flanc descendant a l'instant present, et lorsque la difference de temps est supérieur a 10ms, je valide alors la condition et je peux rentrer dans le code de la consigne 3.1.

        # Detection de flanc sur S1
        if last_S1_Value == 1 and s1.value() == 0:
            # Antirebond sur S1
            if time.ticks_diff(now, debounce_s1) > 10:

            ...

A l'intérieur de mes deux "if", je change l'état de la LED D1, grace a la variable "d1_state", "led_d1" et la fonction ".value". Puis une fois le changement d'état effectué, je resster le systéme d'anti rebond pour la prochaine fois ou j'appuierais dessus. Voila a quoi ressemble le code entier pour cette partie la:

    # Librairies Importé
    from machine import Pin
    import time

    # Entrée des Switchs
    PIN_S1 = 4    # Switch S1

    # Sortie des LEDs
    PIN_D1 = 6    # LED D1

    # Initialisation des GPIOs
    s1 = Pin(PIN_S1, Pin.IN)
    led_d1 = Pin(PIN_D1, Pin.OUT)

    # Variables pour la logique
    d1_state = False

    # Variables de detection de flanc et l'antirebond
    last_S1_Value = 0
    debounce_s1 = 0

    # Boucle Infinie
    While True:
        # Point de repere du temps actuelle en ms
        now = time.ticks_ms()

        # Detection de flanc sur S1
        if last_S1_Value == 1 and s1.value() == 0:
            # Antirebond sur S1
            if time.ticks_diff(now, debounce_s1) > 10:
                # Changement d'état de D1
                d1_state = not d1_state
                led_d1.value(d1_state)
                # Reset du temps d'Anti rebond sur S1
                debounce_s1 = now

        # Mise à jour des états
        last_S1_Value = s1.value()

        # Temps de repos de 1 ms
        time.sleep_ms(1)

### 3.2 Allumer une LED RGB

Pour cette partie la, on nous demande de pouvoir changer la couleur de la LED RGB a chaque fois qu'on appuierais sur l'interrupteur S2. Pour ceci, je dois d'abord importer la librairie "neopixel" pour pouvoir varier les couleurs RGB de ma LED D2.

    # Librairies Importé
    from machine import Pin
    import time
    import neopixel

La deuxieme étape est tout comme pour la partie précédente, est de pouvoir crée des définitions pour mon Switch S2, ainsi que pour ma LED RGB. Donc je dois d'abord regarder le Schéma pour connaitre le GPIO branché au Switch. Dans mon cas S2 est sur le GPIO5 selon le schéma et la LED RGB sur le GPIO48 (L'information est directement ecrit sur le PCB avec l'ESP32-S3)



### 3.3 Connexion sans fil à un autre ESP
