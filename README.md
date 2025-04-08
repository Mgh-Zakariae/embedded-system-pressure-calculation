### **Introduction**

Bonjour à tous, aujourd’hui nous allons présenter **l’architecture d’un système embarqué de calcul de la pression dans un avion**. Cette présentation est divisée en plusieurs parties. Nous allons d’abord voir comment fonctionne un véritable système de gestion de la pression dans un avion et pourquoi il est essentiel. Ensuite, nous parlerons des risques liés à une défaillance de ce système. Par la suite, nous allons présenter l’architecture de notre propre système embarqué, la simulation que nous avons réalisée, les composants utilisés et le code qui permet son fonctionnement. 

### **Le rôle du système de gestion de la pression dans un avion**

Dans un avion, la gestion de la pression est un élément fondamental pour la sécurité et le confort des passagers et de l’équipage. En altitude, la pression atmosphérique est trop basse pour permettre une respiration normale. Sans un système adapté, les passagers et l’équipage risquent de souffrir d’hypoxie, un manque d’oxygène qui peut entraîner une perte de conscience.

Le système de gestion de la pression fonctionne grâce à des **capteurs de pression** qui surveillent en temps réel les variations de pression dans la cabine ou le  **Zone sûre (pression normale) est entre 750 - 800 hPa** et **Zone d'alerte (pression limite) :** **565 - 700 hPa** et un **avertissement** est émis aux pilotes et le **Zone de danger (hypoxie)  lorsque le pression descend sous 565 hPa**  les **masques à oxygène** sont automatiquement déployés. . Ces données sont envoyées à un **contrôleur électronique**, qui ajuste la pression via des **soupapes de régulation**. Ces soupapes permettent de maintenir une pression stable et supportable à l’intérieur de l’avion.

### **Les risques en cas de défaillance du système**

Si ce système venait à tomber en panne, les conséquences pourraient être très graves. Tout d’abord, une perte soudaine de pression pourrait provoquer un manque d’oxygène, entraînant des malaises ou une perte de conscience chez les passagers et l’équipage. Ensuite, une trop forte variation de pression pourrait provoquer des douleurs aux oreilles et des lésions pulmonaires. De plus, certains composants électroniques de l’avion sont sensibles à la pression et pourraient être endommagés en cas d’instabilité.

Dans les situations les plus critiques, une défaillance complète du système pourrait forcer l’équipage à effectuer une descente d’urgence pour retrouver une altitude où l’air est respirable. C’est pourquoi il est essentiel de garantir le bon fonctionnement et la fiabilité d’un tel système embarqué.

---
### **Architecture de notre système embarqué**

Dans notre projet, nous avons conçu un **système embarqué capable de surveiller en temps réel la pression et de déclencher une alerte en cas d’anomalie**. Mon architecture repose sur plusieurs composants clés :

- **Un capteur de pression** : Il mesure la pression en temps réel et transmet les données au microcontrôleur.
- **Un microcontrôleur (arduino Uno)** : Il analyse les données reçues et décide si une alerte doit être déclenchée.
- **Un affichage ou une interface** : Il permet de visualiser en direct les valeurs de pression.
- **Un système d’alerte** : Il prend la forme d’une LED, d’un speaker ou d’un message d’alerte affiché sur un écran.

Le fonctionnement est simple : le capteur envoie des données au microcontrôleur, qui compare la pression mesurée aux seuils prédéfinis. Si la pression est anormalement basse ou haute, une alerte est activée.

### **Simulation et composants utilisés**

Pour tester notre système, nous avons réalisé une **simulation sur Proteus**.
Les composants utilisés sont :

- Un **capteur de pression** BMP180 qui capte le pression dans l’avion.
- Un **microcontrôleur**, tel qu’un Arduino Uno.
- Un **écran LM044L** pour afficher les mesures de pression en temps réel .
- Un **Speaker et une LED** pour signaler les alertes.
![[2025-03-23 00_03_42-presDetect - Proteus 8 Professional - Schematic Capture.png]]

Voici un extrait du code utilisé pour lire la pression et générer une alerte en cas de valeur anormale :
>(code)

### 📚 **1. Bibliothèques utilisées :**
>dans notre code on utilise les Bibliothèques
```cpp
#include <Wire.h>
#include <SFE_BMP180.h>
#include <LiquidCrystal_I2C.h>
```

- **Wire.h** : Gère la communication I²C entre l'Arduino et les périphériques (capteur BMP180 et l'afficheur LCD).
- **SFE_BMP180.h** : Permet d'utiliser le capteur **BMP180** pour mesurer la **pression** et la **température**.
- **LiquidCrystal_I2C.h** : Bibliothèque pour gérer un **écran LCD** via l'interface I²C.

---

### 📊 **2. Déclaration des objets et des broches :**

```cpp
SFE_BMP180 bmp180;
LiquidCrystal_I2C lcd(0x27, 20, 4);

const int greenLED   = 3;
const int yellowLED  = 5;
const int redLED     = 4;
const int speakerPin = 8;
```

- dans ces lines , on cree un objet pour interagir avec le capteur BM180 , et initialise l'ecran LCD , et declare les variable pour les led et le speaker 

---

### 🔊 **3. Fonction `playDangerSiren()` :**

```cpp
void playDangerSiren(unsigned int duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    digitalWrite(redLED, HIGH);
    tone(speakerPin, 2200);  // Son aigu
    delay(100);
    digitalWrite(redLED, LOW);
    tone(speakerPin, 1800);  // Son plus grave
    delay(100);
  }
  noTone(speakerPin);
}
```

- Cette fonction joue une **sirène** dans les cas critiques de basse pression ,et elle clignote la led rouge 

---

### ⚙️ **4. Fonction `setup()` :**

```cpp
void setup() {

  Serial.begin(9600);

  // Initialize LED and speaker pins

  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(speakerPin, OUTPUT);

  lcd.begin(20, 4);
  lcd.backlight();
  Wire.begin();  
  // Start the BMP180 sensor

  bool ok = bmp180.begin();

  if (ok) {
    Serial.println("BMP180 initialized successfully!");
    lcd.setCursor(0, 0);
    lcd.print("BMP180 initialized successfully!");
  } else {
    Serial.println("BMP180 initialization failed!");
    lcd.setCursor(0, 0);
    lcd.print("BMP180 initialization failed!");
    while (1);  

  }

}
```

- on Configure les **LEDs** et le **haut-parleur** en **sortie (OUTPUT)**. et Initialise le capteur BMP180. Si l'initialisation échoue, le programme affiche le message d'erreur. 
---

### 🔁 **5. Fonction `loop()` :**

1. **Lecture de la pression :**

```cpp
status = bmp180.startPressure(3);
delay(status);
status = bmp180.getPressure(pressure, temp);
```

- **startPressure(3)** démarre une lecture de la **pression** 
- **getPressure()** récupère la **pression (en Pa)** 


---

2. **Affichage des données :**

```cpp
Serial.print("Pressure: ");
Serial.print(pressure);
Serial.println(" Pa");
```

- ces lines pour l'affichage le pression sur le **moniteur série** 

---

- pour la gestion des zones et des alertes on ajoute ces lines : 

### ✅ **a) Zone de sécurité (pression ≥ 750 hPa)** :

```cpp
if (pressure >= 750) {
    lcd.print("Safe Zone");
    digitalWrite(greenLED, HIGH);
}
```

- Affiche "Safe Zone" sur le LCD.
- La **LED verte** clignote pour indiquer une **pression normale**.


---

### ⚠️ **b) Zone d'alerte (550 hPa ≤ pression < 750 hPa)** :

```cpp
else if ((pressure >= 550) && (pressure < 750)) {
    lcd.print("Warning!");
    digitalWrite(yellowLED, HIGH);
}
```

- Affiche "Warning!" et "Oxygen dropping" sur le **LCD**.
- La **LED jaune** clignote pour signaler un **risque**.
- Aucun son n'est émis.

---

### 🛑 **c) Zone de danger (pression < 550 hPa)** :

```cpp
else {
    lcd.print("Danger!");
    playDangerSiren(1000);
}
```

- Affiche "Danger!" et "Oxygen CRITICAL" sur le LCD.
- Active la **sirène** et fait **clignoter** la **LED rouge**. par la fonction `playDangerSiren`

---
## présente la simulation 
---
### **Sécurité du système**

Un tel système embarqué pose également des **questions de cybersécurité**. En effet, toute manipulation des données de pression pourrait avoir de graves conséquences.
alors :
- **Comment protéger la communication entre les composants contre des attaques extérieures ?
- **Quels mécanismes peut-on mettre en place pour garantir l’intégrité et la disponibilité des données ?**

et cela que nous allons expliquer dans *lasuivante / autre* presentation *de la sécurité de system embarque*

### **Conclusion**

Pour résumer, nous avons vu :

- L’importance du système de gestion de la pression dans un avion.
- Les risques en cas de défaillance de ce système.
- L’architecture de notre système embarqué et sa simulation.
- Les défis de sécurité associés à ce type de système.

