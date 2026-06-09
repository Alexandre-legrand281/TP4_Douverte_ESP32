/*
Application Emsy TP4 
langage C
autheur: Mermoud Valentin
*/
#include <FastLED.h>
#include <esp_now.h>
#include <WiFi.h>

#define NUM_LEDS 1 
#define LED_PIN 48 //LED RGB connectée au GPIO 48

/*Prototypes de fonctions*/
void on_data_recv(const uint8_t* mac, const uint8_t* data, int len);
void init_esp_now();
void send_message(const uint8_t* msg, size_t len);

//structure de données pour les leds
CRGBArray<NUM_LEDS> leds;

// Broadcast MAC address
uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Mots de passe et messages
const uint8_t PASSWORD[] = {'E', 'S', 'P', '_', '3', '2', '_', 'R', 'E', 'M', 'O', 'T', 'E'};// Message en ASCII pour activer le mode distant
const uint8_t COLLOR[] = {'C', 'H', 'A', 'N', 'G', 'E'};// Message en ASCII pour changer la couleur de la LED

// déffinition des couleurs par rapport à la librairie FastLED
CRGB colors[] = {
    CRGB::Red,
    CRGB::Green,
    CRGB::Blue
};
// Index pour suivre la couleur actuelle
int colorIndex = 0;

// Variables pour la gestion du mode et du clignotement
unsigned long last_send = 0;
unsigned long last_receive = 0;
bool remote_mode = false;
bool blink_state = false;
unsigned long last_blink = 0;
// Variables statiques pour suivre l'état des boutons et de la LED
int boutonS1 = 0;
int boutonS2 = 0;
int boutonOldS1 = 0;
int boutonOldS2 = 0;
bool ledState = false;
//boucle infinie
void loop() {

    //prendre le temps actuel pour la gestion des délais et des modes
    unsigned long now = millis();
    
    // envoi du message PASSWORD toutes les secondes pour maintenir la connexion
    if (now - last_send > 1000) {
        send_message(PASSWORD, sizeof(PASSWORD));
        last_send = now;
    }
    
    // verifier si le mode distant doit être désactivé après 1.5 secondes d'inactivité
    if (remote_mode && (now - last_receive > 1500)) {
        remote_mode = false;
        leds[0] = colors[colorIndex];  // Fix RGB to last color
        FastLED.show(); // Uptade la LED pour refléter le changement de mode (désactivation du clignotement)
    }
    
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
    
    boutonS1 = digitalRead(4);// Lire l'état du bouton S1 connecté au GPIO 4
    boutonS2 = digitalRead(5);// Lire l'état du bouton S2 connecté au GPIO 5
    
    // Button S1: Toggle GPIO 6 LED
    if (boutonS1 == 1 && boutonOldS1 == 0) {
        ledState = !ledState;
        digitalWrite(6, ledState);
        delay(10);
    }
    
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
    // Mettre à jour l'état précédent des boutons pour la détection de front
    boutonOldS1 = boutonS1;
    boutonOldS2 = boutonS2;
    delay(1);
}

/*Fonctions*/

// Fonction d'initialisation
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

// Fonction pour envoyer un message via ESP-NOW
void send_message(const uint8_t* msg, size_t len) {// Envoyer un message à tous les appareils en utilisant l'adresse MAC de diffusion
    esp_now_send(broadcast_mac, (uint8_t*) msg, len);// Cast du message en uint8_t* pour correspondre au type attendu par esp_now_send
}

// Fonction d'initialisation d'ESP-NOW
void init_esp_now() {
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

//fonction pour la réception de données via ESP-NOW
void on_data_recv(const uint8_t* mac, const uint8_t* data, int len) {
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