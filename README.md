# TP4_Douverte_ESP32
## Visual Studio Code

Mise en place:

Pour mettre en place Visual Studio avec PlatformIO en extension il faut suivre ces étapes:

1. Installer Visual Studio code
2. Ouvrir le gestionaire d'extension (sur Vscode)
3. Chercher l'extension platformio ide
4. Installer PlatformIO IDE

Source: https://docs.platformio.org/en/latest/integration/ide/vscode.html#ide-vscode


Pour crée un nouveau projet il faut aller sur la fenêtre home de l'extension PlatformIO et selectioner New project:

<img width="1254" height="674" alt="image" src="https://github.com/user-attachments/assets/34f8c6b8-70fd-4d8d-a2ce-7c6c3eb0cb94" />

Dans le project wizzard il faut mettre un nom de projet et mettre la board utilisé dans ce TP ainsi que de selectioner le framework Arduino:

<img width="587" height="448" alt="image" src="https://github.com/user-attachments/assets/5f8940eb-3117-4f06-8f01-6f8492272e71" />

Le projet est maintenant crée.


### Fichier initalisation du projet PlatformIO

    [env:esp32-s3-devkitc-1]
    platform = espressif32
    board = esp32-s3-devkitc-1
    framework = arduino
    lib_deps = 
    fastled/FastLED
    monitor_speed = 115200

### 3.1 Allumer une LED sur GPIO
Cette partie a pour but de faire changer l'état de la LED D1 à chaque appuis du bouton S1. Un mecanisme de detection de flanc et d'antirebond doit être implémenté.

Dans un premier temp il faut regarder quelle son nos GPIO d'entrée et de sortie:

<img width="374" height="371" alt="LED_et_Switch_Schema" src="https://github.com/user-attachments/assets/a1c48d18-c46d-4d06-a61d-92f47ae7b41d" />

On remarque que la LED D1 est le GPIO6 et que le Bouton S1 est le GPIO4.

On peut alors remplir la fonction d'initalisation (se lançe automatiquement au début du code par l'ESP32):

	void setup() 
	{
    // Initialiser les boutons GPIO
    pinMode(6, OUTPUT);
    pinMode(5, INPUT);
    pinMode(4, INPUT);
    digitalWrite(6, LOW);
	}

Pour la detection du bouton et de son flanc on doit déclarer ces deux variables:

    int boutonS1 = 0;
    int boutonOldS1 = 0;

Il faut aussi déclarer la variable de la LED D1:

    bool ledState = 0;

 boucle infinie:

La syntaxe pour la boucle infinie est comme ceci:

    void loop() 
    {
        //code
    }

Maintenant il faut lire l'état du bouton avec la ligne suivante:

    boutonS1 = digitalRead(4);

Cette fonction vas lire l'état d'un port entré en paramètre (GPIO4 dans notre cas)

Maintenant on peut tester la valeur de notre bouton et changer l'état de la led si besoin:

    if (boutonS1 == 1 && boutonOldS1 == 0) {
        ledState = !ledState;
        digitalWrite(6, ledState);
        delay(10);
    }

si on detecte un appuis de bouton (avec flanc montant dans notre cas) on change l'état de ledSate, la fonction digitalWrite sert à changer l'état d'un port (dans notre cas par rapport à l'état de la variable ledState).
Le delay de 10ms sert à garantir que le système ne detecte pas les rebonds mécanique.

À la fin du code il ne faut pas oublier de mettre l'archivage de la valeur du bouton:

    boutonOldS1 = boutonS1;

### 3.2 Allumer une LED RGB
Cette partie a pour but de completer le programme afin que la LED RGB du ESP32-S3-DevKit-C change de couleur à chaque appuis du bouton S2 (R -> G -> B -> R -> etc.)

Librairies utilisées:

	#include <FastLED.h>

FastLed.h est une librairie arduino qui permet de piloter des LEDS adressables.

Dans un premier temp il faut regarder quelle son nos GPIO d'entrée et de sortie:

<img width="374" height="371" alt="LED_et_Switch_Schema" src="https://github.com/user-attachments/assets/a1c48d18-c46d-4d06-a61d-92f47ae7b41d" />

On remarque que le bouton S2 correspond au GPIO5.

On peut définir le nombre de LEDS pilotée et le GPIO de notre LED RGB:

	#define NUM_LEDS 1 // Nombre de LEDs dans la bande
	#define LED_PIN 48 //LED RGB connectée au GPIO 48

On peut ajouter une structure pour les données pour la LED (importée de la librairie fastLed):

	CRGBArray<NUM_LEDS> leds;

Ensuite on défini les couleurs utilisée(Par rapport à la librairie fastLed):

	CRGB colors[] = {
    CRGB::Red,
    CRGB::Green,
    CRGB::Blue
	};

On peut completer la fonction d'initalisation (se lançe automatiquement au début du code par l'ESP32):

	void setup() 
	{
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);// Initialiser la bande LED avec le type WS2812, le pin défini et l'ordre de couleur GRB
    FastLED.setBrightness(20);// Régler la luminosité des LEDs 
    leds[0] = CRGB::Red;// Initialiser la première LED à rouge
    FastLED.show();// Afficher les changements sur les LEDs
    
    // Initialiser les boutons GPIO
    pinMode(6, OUTPUT);
    pinMode(5, INPUT);
    pinMode(4, INPUT);
    digitalWrite(6, LOW);
	}

Dans cette initalisation on initalise La led RGB ainsi que les entrée et sortie GPIO.
Pour la LED RGB on initalise en indiquant le protocole utilisé (WS2812), le GPIO utilisé (LED_PIN / GPIO48), L'ordre des couleurs (G R B), le tableau stockant les données de la couleurs dans le code, le nombre de led utilisé ( NUM_LEDS 1).
On initalise aussi la luminosité de la LED ainsi que sa couleur de départ (rouge).

On déclare la variable qui vas retenir à quelle couleur on se trouve ainsi que les deux variable du bouton S2:

	int colorIndex = 0;
	int boutonS2 = 0;
	int boutonOldS2 = 0;

 Boucle infinie:

Lecture de l'état du bouton:

	boutonS2 = digitalRead(5);// Lire l'état du bouton S2 connecté au GPIO 5

On test la valeur du  bouton et de son flanc (flanc montant dans notre cas):

	if (boutonS2 == 1 && boutonOldS2 == 0) 
	{
		colorIndex = (colorIndex + 1) % 3;
        leds[0] = colors[colorIndex];
        FastLED.show();
	}
	delay(10);

Si on appuie sur S2 on incremente colorIndex de 1 et les valeurs possible sont 0 1 et 2 (grace au modulo 3).
On update ensuite leds (qui est lier à la structure CRGBArray) et la fonction FastLED.show(); vas changer l'état phisique de la LED RGB.

À la fin du code on archive la valeur du bouton S2:

	boutonOldS2 = boutonS2;

### 3.3 Connexion sans fil à un autre ESP
Cette partie a pour but de completer le programme afin que l'ESP32 fonctionne localement ou alors en mode remote, le mod remote permet de changer la couleur de la LED RGB d'un autre kit ainsi qu'un autre kit puisse changer la couleur de la LED RGB de notre kit. La LED RGB doit clignioter à une frequence de 2 Hz pour indiquer une connextion active.

Librairies utilisées:

	#include <esp_now.h>
	#include <WiFi.h>

esp_now est la librairie pour la communication entre ESP32 et WiFi est la librairie qui vas initaliser le WIFI.

Pour la communication WIFI on vas Déclarer 3 tableau:

	
	uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	const uint8_t PASSWORD[] = {'E', 'S', 'P', '_', '3', '2', '_', 'R', 'E', 'M', 'O', 'T', 'E'};
	const uint8_t COLLOR[] = {'C', 'H', 'A', 'N', 'G', 'E'};

broadcast_mac est un tableau qui definis l'adresse de diffusion, dans notre cas les 6 case de 0xFF signifie envoie à tout appareil.
PASSWORD est un tableau en message ASCII qui vas etre envoyer et comparé dans la suite du code
COLLOR est un tableau en message ASCII qui vas indiquer la commande envoyée à un autre ESP qui vas etre envoyer et comparé dans la suite du code.

La fonction d'initalisation a ausis changée:

	void setup() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);// Initialiser la bande LED avec le type WS2812, le pin défini et l'ordre de couleur GRB
    FastLED.setBrightness(20);// Régler la luminosité des LEDs 
    leds[0] = CRGB::Red;// Initialiser la première LED à rouge
    FastLED.show();// Afficher les changements sur les LEDs
    
    // Initialiser les boutons GPIO
    pinMode(6, OUTPUT);
    pinMode(5, INPUT);
    pinMode(4, INPUT);
    digitalWrite(6, LOW);
    
    // Initialiser ESP-NOW
    init_esp_now();
    
    // Initialiser les temps d'envoi et de réception pour la gestion du mode distant
    last_send = millis();
    last_receive = millis();
	}

l'appel de fonction de l'initalisation du protocole ESP-NOW est effectué ici.
Les temps d'envois et de retour sont aussi initalisé.

On peut déclarer ces variables qui seront responsable de la gestion du mode et du cligniotement de la LED RGB:

	unsigned long last_send = 0;
	unsigned long last_receive = 0;
	bool remote_mode = false;
	bool blink_state = false;
	unsigned long last_blink = 0;

Dans cette partie 3 fonction on été crée:

	/*Prototypes de fonctions*/
	
	void on_data_recv(const uint8_t* mac, const uint8_t* data, int len);
	void init_esp_now();
	void send_message(const uint8_t* msg, size_t len);

on_data_recv est une fonction pour la réception de données via ESP-NOW.
init_esp_now est une fonction qui vas initaliser le protocole ESP-NOW
send_message est une fonction qui vas envoyer des données via ESP-NOW.

Fonction on_data_recv:

	void on_data_recv(const uint8_t* mac, const uint8_t* data, int len) 
	{
    // Check si PASSWORD reçu pour activer le mode distant
    if (len == sizeof(PASSWORD) && memcmp(data, PASSWORD, len) == 0) {
        remote_mode = true;// Activer le mode distant
        last_receive = millis();// Mettre à jour le temps de la dernière réception pour éviter la désactivation du mode distant
    }
    
    // Check si COLLOR reçu pour changer la couleur de la LED
    if (len == sizeof(COLLOR) && memcmp(data, COLLOR, len) == 0) {
        // Change la couleur de la LED en fonction de l'index actuel et l'affiche
        colorIndex = (colorIndex + 1) % 3;
        leds[0] = colors[colorIndex];
        FastLED.show();
    }
	}

cette fonction qui dans un premier temp verifier si le mot de passe reçu est bien le même qui a été définis au debut de code, si oui il active le mode remote et met â jour le temp de reception pour savoir plus tard quand le dernier message a été reçu.
Ensuite on verifie que la commande COLLOR (changement de couleur) est reçue, si oui on incremente colorIndex et on actualise l'affichage de la LED RGB.

Fonction init_esp_now:

	void init_esp_now() 
	{
    WiFi.mode(WIFI_STA); // Mettre le WiFi en mode station pour utiliser ESP-NOW
    WiFi.disconnect(); // Déconnecter du WiFi pour éviter les interférences avec ESP-NOW
    
    // Initialiser ESP-NOW et vérifier si l'initialisation a réussi
    if (esp_now_init() != ESP_OK) { 
        return;
    }
    
    esp_now_register_recv_cb(on_data_recv);// Enregistrer la fonction de rappel pour la réception de données
    
    // Ajouter un peer pour la diffusion (broadcast)
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcast_mac, 6);// Copier l'adresse MAC de diffusion dans la structure du peer
    peerInfo.channel = 0;// Utiliser le canal actuel
    peerInfo.encrypt = false;// Ne pas chiffrer les données envoyées à ce peer
    
    // Ajouter le peer à ESP-NOW et vérifier si l'ajout a réussi
    esp_now_add_peer(&peerInfo);
	}

Cette fonction vas initaliser le protocole de communication comme ceci:

Mettre le mode WIFI en station ( Utilisation de la fréquence WIFI sans utiliser tout le protocole WIFI ( routeur wifi, adresse IP , protocole TCP))

Initaliser le protocole ESP-NOW

Enregister la fonction de reception de donnée

Ajouter un peer de broadcast (copie adresse de broadcast predefinie au debut du code, definition du canal utilisé, choix du cryptage)

ajout du peer au protocole ESP-NOW

Fonction send_message:

	void send_message(const uint8_t* msg, size_t len) 
	{
	// Envoyer un message à tous les appareils en utilisant l'adresse MAC de diffusion
    esp_now_send(broadcast_mac, (uint8_t*) msg, len);// Cast du message en uint8_t* pour correspondre au type attendu par esp_now_send
	}

Cette fonction vas envoyer un message (password ou commande couleur) à l'adresse de broadcast.

 Boucle infinie:

au debut on prend le temp actuel pour pouvoir ensuite gerer le delais des modes dans la suite du code:

	 //prendre le temps actuel pour la gestion des délais et des modes
    unsigned long now = millis();

On vas envoyer le mot de passe toute les 1 secondes pour maintenir la connextion(si une connexion est bien en place):

	// envoi du message PASSWORD toutes les secondes pour maintenir la connexion
    if (now - last_send > 1000) {
        send_message(PASSWORD, sizeof(PASSWORD));
        last_send = now;
    }

On verifie si le mode remote doit etre desactivé apès 1.5 secondes:

	// verifier si le mode distant doit être désactivé après 1.5 secondes d'inactivité
    if (remote_mode && (now - last_receive > 1500)) {
        remote_mode = false;
        leds[0] = colors[colorIndex];  // Fix RGB to last color
        FastLED.show(); // Uptade la LED pour refléter le changement de mode (désactivation du clignotement)
    }

Si le mode remote est activé on fais clignioter la LED RGB à une fréquence de 2Hz:

	 // mode distant: faire clignoter la LED à 2Hz
    if (remote_mode) {
        if (now - last_blink > 250) {  // 250ms = 2Hz
            blink_state = !blink_state;
            if (blink_state) {
                leds[0] = colors[colorIndex];// Afficher la couleur actuelle lorsque le clignotement est actif
            } else {
                leds[0] = CRGB::Black;// Éteindre la LED lorsque le clignotement est inactif
            }
            FastLED.show();
            last_blink = now;
        }
    }

On lis les état des boutons:

	boutonS1 = digitalRead(4);// Lire l'état du bouton S1 connecté au GPIO 4
    boutonS2 = digitalRead(5);// Lire l'état du bouton S2 connecté au GPIO 5

Suivant le mode activé (local ou remote) on envoie une commande de changement de couleur ou alors on change la couleur localement:

	// Button S2: fonctionne différemment selon le mode (local ou distant)
    if (boutonS2 == 1 && boutonOldS2 == 0) {
        if (remote_mode) {
            // Mode distant: envoyer un message pour changer la couleur à tous les autres appareils
            send_message(COLLOR, sizeof(COLLOR));
        } else {
            // Mode local: changer la couleur de la LED et l'afficher
            colorIndex = (colorIndex + 1) % 3;
            leds[0] = colors[colorIndex];
            FastLED.show();
        }
        delay(10);
    }


On archive l'état des boutons:

	boutonOldS1 = boutonS1;
    boutonOldS2 = boutonS2;

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

![Schema](/Images/Switch_Schema.png)

![LED_RGB](/Images/LED_RGB.png)

Maintenant que j'ai pu determiné les GPIO de mon switch et de la LED, il faut que je vais les définir et initialiser les GPIO non seulement avec la fonction "Pin", mais je vais aussi utiliser la fonction "neopixel.NeoPixel". Cette fonction permet d'initialiser ma LED RGB, grace a la librairie neopixel.

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

Je vais par la suite initialiser des variables pour la logique de notre LED RGB. La variable "rgb_index" serviras d'index pour le changement de couleur de ma LED, tandis que la variable "colors" est un tableau contenant l'index des differentes couleurs (Dans l'ordre ce seras Rouge; Vert et Bleu). Pourquoi est-ce que j'ai mis "16" au lieu de "255"? C'est pour eviter que la LED soit trop lumineuse.

    # Variables pour la logique
    d1_state = False
    rgb_index = 0
    colors = [(16, 0, 0), (0, 16, 0), (0, 0, 16)] # R, G, B

Tout comme pour la partie 3.1, je vais ajouter une variable de detection de flanc et une autre pour le systéme d'antirebond pour l'interrupteur S2.

    # Variables de detection de flanc et l'antirebond
    last_S1_Value = 0
    last_S2_Value = 0
    debounce_s1 = 0
    debounce_s2 = 0

Dans ma boucle infinit, je vais tout comme pour la partie 3.1 ajouter deux "if". Un pour la detection de flanc montant, un autre pour le systéme d'antirebond. Je vais egalement a la fin de mon code faire une mise a jour de l'état du switch S2

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

        # Détection de flanc sur S2
        if last_S2_Value == 1 and s2.value() == 0:
            # Antirebond sur S2
            if time.ticks_diff(now, debounce_s2) > 10:

            ...

        # Mise à jour des états
        last_S1_Value = s1.value()
        last_S2_Value = s2.value()

        # Temps de repos de 1 ms
        time.sleep_ms(1)

A l'interieur de ces deux "if", apres avoir fait la detection de flanc et le systéme d'antirebond, je vais changer la valeur de ma variable "rgb_index" grace a un calcul, puis avec cette variable je vais changer la couleur de ma LED RGB grace a "np" et le tableau "colors", puis utiliser la fonction "np.write" pour confirmer le changement de couleur. Voici le code pour la partie 3.1 et 3.2 reunni:

    # Librairies Importé
    from machine import Pin
    import time
    import neopixel

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

    # Variables pour la logique
    d1_state = False
    rgb_index = 0
    colors = [(16, 0, 0), (0, 16, 0), (0, 0, 16)] # R, G, B

    # Variables de detection de flanc et l'antirebond
    last_S1_Value = 0
    last_S2_Value = 0
    debounce_s1 = 0
    debounce_s2 = 0

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

        # Détection de flanc sur S2
        if last_S2_Value == 1 and s2.value() == 0:
            # Antirebond sur S2
            if time.ticks_diff(now, debounce_s2) > 10:
                # Changement de couleur de la LED RGB
                rgb_index = (rgb_index + 1) % 3
                np[0] = colors[rgb_index]
                np.write()
                # Reset du temps d'Anti rebond sur S2
                debounce_s2 = now

        # Mise à jour des états
        last_S1_Value = s1.value()
        last_S2_Value = s2.value()

        # Temps de repos de 1 ms
        time.sleep_ms(1)

### 3.3 Connexion sans fil à un autre ESP

Pour la partie 3.3, il faut deux ESP et ils doivent etablir une communication sans fil. Il s'agirait de crée deux modes:
    
- Le mode Local: Le mode au quel les deux ESP ne peuvent pas communiquer, et donc réalisent les memes fonctions que dans les partie 3.1 et 3.2
- Le mode Remote: Le mode au quel les deux ESP peuvent communiquer. Lorsque ce mode est activé, la LED RGB des deux ESP clignote a une fréquence de 2Hz, et lorsque le bouton S2 de mon premier ESP est appuyer la LED RGB du deuxieme change de couleur, et inversement.

Pour ceci, le datasheet de mon module ESP ou le schéma de mon PCB n'est pas nécessaire pour définir les GPIO, car les deux ESP sont identiques et auront donc les memes pins, et par conséquent le meme code.

La premiere étape pour reussir a établir une connexion entre les deux ESP c'est d'ajouter les librairies "network" et "espnow"

    # Librairies Importé
    from machine import Pin
    import time
    import neopixel
    import network, espnow

Je vais maintenant initialiser quelques variables, qui me serviront pour le clignotement; l'envoi de donnée (que j'expliquerais par la suite); pour switcher entre le mode Remote et le mode Local et pour le temps d'envoie.

    # Variable du mode Remote
    Remote = False
    blink_time = 0
    blink = False
    send = 0
    time_out = 0

J'initialise et j'active la communication sans fil via le protocole ESP-NOW. Il faut configurer l'interface Wi-Fi de l'ESP32 en mode "Station" (STA), et pour ça j'utilise la fonction "wlan = network.WLAN(network.STA_IF)", puis j'allume physiquement le module radio Wi-Fi de la carte grace a la fonction "wlan.active(True)". Par la suite, j'initialise le protocole ESP-NOW avec "e = espnow.ESPNow()", et j'active celui-ci avec "e.active(True)"

    # Initialisation réseau
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    e = espnow.ESPNow()
    e.active(True)

Je configure une variable que je nommerais "boardcast_mac", avec comme adresse MAC "b'\xff\xff\xff\xff\xff\xff'". Comme ça je ciblerais tout les ESP-32, et je 
