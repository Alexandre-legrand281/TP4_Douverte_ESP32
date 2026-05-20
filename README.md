# TP4_Douverte_ESP32
## Visual Studio Code
### 3.1 Allumer une LED sur GPIO

### 3.2 Allumer une LED RGB

## Micro Python Code

Afin de développer mon code, j'ai du d'installer Thonny et Flasher le logiciel pour pouvoir coder en MicroPython. Apres avoir branché l'ESP32, j'ai du d'acceder aux options de Thonny, puis interpreter le type "MicroPython (ESP32)", et séléctionner le port ou est branché l'ESP32. Pour savoir a quel port est brancher l'ESP32, j'ai du d'acceder au Gestionnaire de périphériques (dans mon cas c'est le COM14).

Image Options

Image Gestionnaire de périphérique

Une fois cette etape effectué, il faut cliquer sur "Installer ou metre a jour MicroPython". Une fenetre va allors s'ouvrir, et il va faloir selectionner a nouveau le port ou ce situe mon ESP32 dans "Target port". Il faudras egalement selectionner la famille de MicroPython, la varainte et la version, au quel j'ai respectivement configurer de la maniere ci-dessous.

Image Install MicroPython

ATTENTION: Il ce peut que l'installation ne puisse pas etre effectué, car il y aurais deja un autre programme installer dessus. Si ce probleme surgit alors faut Resset l'ESP32. Pour faire ceci, il faut maintenir appuyer sur le bouton "BOOT" puis appuyer sur le bouton "RESET". L'ESP32 va alors s'eteindre, puis ce reallumeras quelques secondes apres. Vous pouvez maintenant poursuivre l'installation.

### 3.1 Allumer une LED sur GPIO

Pour cette partie la, je devais selon le cahier des charges allumer la LED D1 de la carte lorsque j'appuie sur le bouton S1. La premiere etape que j'ai du effectuer, c'etait d'importer la librairie nécessaire pour intéragir avec les GPIO, ainsi qu'une deuxieme librairie pour ajouter un systéme d'Antirebond sur les intérrupteur.

    # Librairies Importé
    from machine import Pin
    import time

Par la suite, j'ai du de regarder dans le Schéma de la carte quels GPIO etait relier a S1 et D1. Dans mon cas S1 est relier au GPIO4 et D1 à GPIO6.

Image du Datasheet



### 3.2 Allumer une LED RGB

### 3.3 Connexion sans fil à un autre ESP
