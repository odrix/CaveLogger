# Env Cave Logger

un logger d'environnement de cave.

Le but est d'enregistrer puis visualiser les données (témperature, hygromètrie, lumière) d'une cave (à vin par exemple) dans le temps.

## Materiel
 * un arduino
 * un capteur DHT11
 * une diode potoresistante
 * un lecteur de carte SD
 * un module RTC
 * un module bluetooth

## Logiciel
* du code pour arduino (Arduino_prog) qui enregistre les infos des capteur sur une carte SD
* du code JS pour visualiser les données récolter préalablement (utilisation de bootstrap et D3.js)
