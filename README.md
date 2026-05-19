# UNO Q - Minuteur de cuisine hybride (MPU + MCU + Bridge)

![Arduino UNO Q](https://img.shields.io/badge/Board-Arduino%20UNO%20Q-green)
![Arduino App Lab](https://img.shields.io/badge/Arduino%20App%20Lab-Python%20%2B%20Sketch-blue)
![Bridge](https://img.shields.io/badge/Communication-Bridge-orange)
![LCD](https://img.shields.io/badge/UI-LCD%2020x4-lightgrey)
![Modulino](https://img.shields.io/badge/Modulino-Buttons%20%2B%20Buzzer-red)

## Présentation

Petit projet pédagogique réalisé avec l'Arduino UNO Q.

L'objectif n'est pas de créer un minuteur complexe, mais de démontrer simplement la communication bidirectionnelle entre :

- le **MPU Linux** (Python)
- le **MCU STM32** (C++)
- via le mécanisme **Bridge**

Le projet simule un minuteur de cuisine avec :

- sélection de la durée via boutons Modulino
- affichage sur écran LCD 20x4 I2C
- décompte temps réel
- signal sonore de fin

---

## Objectif pédagogique

Ce projet permet de comprendre concrètement l’architecture hybride de l’UNO Q :

- **Le MCU** gère le matériel :
  - boutons
  - buzzer
  - écran LCD

- **Le MPU** gère la logique applicative :
  - calcul du temps
  - décompte
  - état du minuteur

- **Bridge** assure les appels de fonctions entre les deux mondes.

---

## Architecture

```text
+---------------------------+
|        MCU (STM32)        |
|---------------------------|
| Modulino Buttons          |
| Modulino Buzzer           |
| LCD 20x4 I2C             |
| Interface utilisateur     |
+-------------+-------------+
              |
              | Bridge.call()
              |
+-------------v-------------+
|       MPU (Linux)         |
|---------------------------|
| Python                    |
| Classe Minuteur           |
| time.monotonic()          |
| Logique de décompte       |
+-------------+-------------+
              |
              | Bridge.call()
              |
+-------------v-------------+
|        MCU (STM32)        |
|---------------------------|
| Mise à jour LCD           |
| Signal sonore de fin      |
+---------------------------+

```

---

## Fonctionnement

### Bouton B

Ajoute 10 secondes.

Limite maximale :

600 secondes (10 minutes)

---

### Bouton A

Lance le minuteur.

Le MCU appelle :

```c++
Bridge.call("api_temps", secondes);
```

Le MPU reçoit la demande et démarre le décompte.

---

### Bouton C

Arrête le minuteur.

Le MCU appelle :

```c++
Bridge.call("stop_timer");
```

Le MPU réinitialise complètement le minuteur.

---

## Communication Bridge utilisée

### MCU → MPU

```c++
Bridge.call("api_temps", secondes);
Bridge.call("stop_timer");
```

---

### MPU → MCU

```python
Bridge.call("minuteur", ecoule, restant)
Bridge.call("fin")
```

---

## Choix techniques

### Pourquoi time.monotonic() ?

Le minuteur utilise :

```python
time.monotonic()
```

car cette horloge : 

- n’est pas affectée par les changements d’heure
- ne dépend pas de l’horloge système
- est parfaitement adaptée à un décompte simple
 
Pour un minuteur de cuisine, cette solution est largement suffisante.

---

### Pourquoi `Bridge.call()` et non `notify()` ?

Dans ce projet, `Bridge.call()` a été volontairement utilisé dans les deux sens.

Même si `notify()` aurait pu convenir pour certains événements sans retour attendu,  
l’objectif pédagogique était ici de manipuler explicitement les appels de fonctions entre MPU et MCU.

---

## Matériel utilisé :

- Arduino UNO Q
- Modulino Buttons
- Modulino Buzzer
- écran LCD 20x4 I2C (3,3V)
- bibliothèque Arduino_RouterBridge
- bibliothèque IskakINO_LiquidCrystal_I2C

---

## Structure du projet

```
minuteur-unoq/
├── python/
│   └── main.py
│
├── sketch/
│   └── sketch.ino
│
└── README.md
```

---

## Démonstration

### Séquence typique :  

1. démarrage de l’application
2. preuve de vie sonore
3. ajout de secondes avec le bouton B
4. lancement avec A
5. décompte affiché sur LCD
6. fin du minuteur
7. alerte sonore

---

## Ce que montre ce projet

Même un projet simple permet de comprendre :  

- séparation logique / matériel
- architecture hybride UNO Q
- RPC entre MCU et MPU
- synchronisation d’état
- pilotage temps réel d’un périphérique

  ---

## Conclusion

Ce projet est volontairement simple.  
Son intérêt principal est pédagogique : montrer comment répartir proprement les responsabilités entre le  
Linux embarqué et le microcontrôleur de l’UNO Q.
