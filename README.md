# Projet : Capteur Graphite à Crayon

## Table des matières
- [Projet : Capteur Graphite à Crayon](#projet--capteur-graphite-à-crayon)
  - [Table des matières](#table-des-matières)
- [Contexte](#contexte)
- [Réalisation du projet](#réalisation-du-projet)
  - [1. Matériaux utilisés](#1-matériaux-utilisés)
  - [2. Simulation électronique en utilisant LTSpice](#2-simulation-électronique-en-utilisant-ltspice)
    - [2.1 Fonctionnalité de condition nominale](#21-fonctionnalité-de-condition-nominale)
    - [2.2 Modélisation du capteur](#22-modélisation-du-capteur)
    - [2.3 Résultats visuels](#23-résultats-visuels)
    - [2.4 Simulation du signal alternatif](#24-simulation-du-signal-alternatif)
  - [3. Conception du circuit PCB en utilisant KiCad](#3-conception-du-circuit-pcb-en-utilisant-kicad)
    - [3.1 Réalisation des symboles et empreintes](#31-réalisation-des-symboles-et-empreintes)
    - [3.2 Réalisation du schéma électronique](#32-réalisation-du-schéma-électronique)
    - [3.3 Réalisation du PCB](#33-réalisation-du-pcb)
  - [4. Code Arduino](#4-code-arduino)
  - [5. Application Android codée avec MIT App Inventor](#5-application-android-codée-avec-mit-app-inventor)
    - [5.1. Initialisation](#51-initialisation)
    - [5.2. Composantes dans l'interface](#52-composantes-dans-linterface)
      - [Connection du Bluetooth:](#connection-du-bluetooth)
    - [Le bouton pour le capteur industriel (FlexSensor) et le capteur graphite](#le-bouton-pour-le-capteur-industriel-flexsensor-et-le-capteur-graphite)
    - [Le bouton "Mesure"](#le-bouton-mesure)
    - [Le bouton "Reset"](#le-bouton-reset)
    - [Bloc principal - Timer](#bloc-principal---timer)
    - [Le Graphique](#le-graphique)
  - [6. Réalisation du Shield](#6-réalisation-du-shield)
  - [7. Banc de test](#7-banc-de-test)
    - [7.1 Physique derrière le capteur en graphite](#71-physique-derrière-le-capteur-en-graphite)
    - [7.2 Résultats de mesures](#72-résultats-de-mesures)
  - [8. Datasheet](#8-datasheet)
- [Conclusion](#conclusion)
- [Références](#références)
- [Contacts](#contacts)
  - [Étudiants](#étudiants)
  - [Enseignants](#enseignants)

---

![Image de l'ensemble carte Arduino et capteur en graphite](/README_Image/Image_Shield_assemble.jpg)


# Contexte
Ce projet vise à développer un capteur basé sur du graphite de crayon de papier pour des applications en instrumentation. L'objectif est d'explorer les propriétés conductrices du graphite et de les exploiter dans un circuit électronique interactif.

---

# Réalisation du projet

---

## 1. Matériaux utilisés

Dans ce projet, nous utilisons des composants électroniques et des modules disponibles dans la salle d'instrumentation avec une carte Arduino UNO. Tous les composants sont listés ci-dessous :
   * 1 carte Arduino UNO Rev 3
   * 1 module Bluetooth (HC-05)
   * 1 encodeur rotatif (Keyes KY-040)
   * 1 écran OLED (SSD 1306)
   * 1 capteur de flexion commercial (LLC 1070)
   * 1 capteur en graphite fait à l'aide de la mine d'un crayon de papier
   * 1 amplificateur de transimpédance (LTC1050)
   * 1 potentiomètre numérique (MCP41100)
   * 2 supports IC pour le potentiomètre et l'amplificateur
   * 1 résistance 1kΩ, 1 résistance 10kΩ, 2 résistances 100kΩ
   * 3 condensateurs 100nF, 1 condensateur 1µF

---

## 2. Simulation électronique en utilisant LTSpice

### 2.1 Fonctionnalité de condition nominale

![Schema_nominale.png](/README_Image/Schema_nominale.png)

Le courant d’entrée Isens varie entre 50 nA et 100 nA, ce qui entraîne une variation de la tension Vep appliquée à l’entrée non-inverseur V+ du LTC1050, entre 5 mV et 10 mV.

Le gain de l’amplificateur du LTC1050 est défini par : G = 1 + R3/R2 = 101. Par conséquent, la valeur de la tension de sortie du LTC1050 varie entre 0.5 V et 1 V.

![Simulation gain](/README_Image/schema_simulation_gain.png)

Au départ, le signal exprimé en décibels est de 140 dB, ce qui correspond à :  
20 log(VADC / Isens).

Le microcontrôleur utilisé est un Arduino UNO, basé sur un microcontrôleur AVR avec une fréquence d’échantillonnage maximale fech = 200 kHz.  
Comme la conversion analogique-numérique se fait sur 13 bits, la fréquence d’échantillonnage réelle est limitée à 15.4 kHz.

D'après le théorème de Nyquist, la fréquence maximale du signal que l'on peut correctement numériser doit donc être inférieure à la moitié de cette valeur :  
fsignal < fech/2 = 7.7 kHz.


### 2.2 Modélisation du capteur

Le bruit à 50 Hz, généré notamment par l'écran TFT (bruit de type secteur), est clairement observé dans le spectre du signal. Pour l’atténuer, on agit sur le condensateur C4 du filtre passe-bas.

![Pic normal](/README_Image/schema_pic_normal.png)

- Lorsque la valeur de C4 est augmentée à 10 µF, le pic de bruit à 50 Hz est fortement réduit, ce qui indique une amélioration de l’atténuation dans les basses fréquences.

![Pic diminue](/README_Image/schema_pic_diminue.png)

- En revanche, si on diminue la valeur de C4, le bruit augmente, montrant que la fréquence de coupure du filtre remonte et que le bruit secteur passe plus facilement.

![Pic bruit](/README_Image/schema_pic_bruit.png)


### 2.3 Résultats visuels

Une photo démontrant que notre circuit permet une amplification efficace du signal délivré par le capteur :

![Schema OA](/README_Image/Schema_OA.png)

![Simulation OA](/README_Image/schema_simulation_OA.png)


### 2.4 Simulation du signal alternatif

Ensuite, on présente la réponse du circuit lorsque l'on simule un courant alternatif, afin de vérifier que le bruit est correctement filtré :

Le bruit du réseau est atténué d'environ 72 dB à 50 Hz.

![Schema OA dB](/README_Image/Schema_OA_dB.png)


---

## 3. Conception du circuit PCB en utilisant KiCad

Afin de concevoir le circuit électronique, le logiciel en libre accès KICAD a été utilisé. L'ensemble des fichiers KiCad est disponible dans le dossier [Capteur_KICAD](./Capteur_KICAD).


### 3.1 Réalisation des symboles et empreintes

Pour commencer notre PCB (Printed Circuit Board), il est nécessaire de créer les symboles et les empreintes des composants qui ne sont pas disponibles dans la bibliothèque de KiCad. Nous avons réalisé les symboles du module Bluetooth, de l'encodeur rotatif, de l'écran OLED, du capteur de flexion et du capteur en graphite, afin de les ajouter au schéma de connexion. Ensuite, ces symboles ont été associés à une empreinte trouvée sur des sites de libre téléchargement.

![Schéma électonique réalisé sur KiCad](/README_Image/schema_KiCad_Schematic.png)


### 3.2 Réalisation du schéma électronique

Ensuite, le schéma électronique est à construire. Dans cette partie, les différentes connexions entre composants sont définies et les pins de la carte Arduino sont attribués à chacun des composants.

![Schéma de routage du PCB sur KICAD](/README_Image/schema_KiCad_PCB.png)


### 3.3 Réalisation du PCB

L'objectif ici était d'allouer un branchement sur les entrées d'une carte Arduino UNO à chaque module. Nous avons aussi pour but de faire le moins possible de via (pont traversant permettant de chavaucher un routage).
Le circuit imprimé a été dessiné avec une attention particulière portée à la disposition des pistes pour minimiser les couplages parasites et faciliter le routage manuel.

![Schéma Conception du PCB sur KICAD](/README_Image/schema_KiCad_PCB.png)

Les contraintes de branchement ont été les suivantes:
  * Encoder: pin clk -> pin 2 (Arduino)
  * Bluetooth: pin TXD -> la pin 5 (pin RXD de l'Arduino)
               pin RXD -> la pin 6 (pin TXD de l'Arduino)
  * Potentiomètre digital: pin SCK -> pin 13 (pin SCK de l'Arduino)
                           pin SDI -> pin 11 ()
  * FlexSensor: sur une pin analogue (A2 choisi)
  * Capteur Graphite: sur une pin analogue (A0 choisi)


Ensuite, l'impression du circuit s'est ensuite faite par méthode chimie:
  * Plaque de cuivre/résine dont la face en cuivre est enduite d'une résine photosensible;
  * Insolation de la résine sur les parties du cuivre non voulue;
  * Attaque chimique dans un bain révélateur;
  * Rinçage du circuit;

![Image montrant le PCB imprimé. Les soudures des composants sont aussi visibles](/README_Image/Image_PCB_imprime.jpg)

---

## 4. Code Arduino

Le code Arduino permet de lire les valeurs du capteur en graphite, de gérer l'affichage d'un Menu sur l'écran OLED, la communication par Bluetooth avec l'application Android (voir section [5. Application Android codée avec MIT App Inventor](#5.-Application-Android-codée-avec-MIT-App-Inventor)) et le contrôle via l'encodeur rotatif.
Le code arduino est disponible dans le dossier [Capteur_Arduino](./Capteur_Arduino).

![Image du menu](/README_Image/schema_Menu_Arduino.png)

---

## 5. Application Android codée avec MIT App Inventor

Une application mobile a été développée pour :
- Recevoir les données en Bluetooth,
- Afficher les mesures en temps réel,
- Interagir avec le capteur de manière intuitive.
  
![Interface](/README_Image/Image_MIT/Interface.png)

### 5.1. Initialisation

  Nous avons déclaré les variables initiales utilisées dans le programme:

  ![Variable initial](/README_Image/Image_MIT/Initial_variable.png)

  Puis nous avons initialisé le programme:

  ![Demarrer screen](/README_Image/Image_MIT/Initilisation_screen.png)

### 5.2. Composantes dans l'interface
---
#### Connection du Bluetooth:

![Bouton BT](/README_Image/Image_MIT/Button_BT.png)
Le bouton « List_Connection » permet de sélectionner le module Bluetooth auquel on souhaite se connecter. Le bouton « Button_Disconnect » permet également de déconnecter le module Bluetooth sélectionné.

---

### Le bouton pour le capteur industriel (FlexSensor) et le capteur graphite

![Bouton Flex_Graphitesensor](/README_Image/Image_MIT/Button_Graphite_Flex.png)

Ces bouton permet de choisir le capteur que l'on veut afficher leur valeurs.

---
### Le bouton "Mesure"
![Bouton Mesure](/README_Image/Image_MIT/Button_Mesure.png)

Ce bouton permet d'envoyer un message vers l'arduino via bluetooth pour lui demande de renvoyer les valeurs de resistances dependant le mode du capteur choisi.

---
### Le bouton "Reset"
![Reset Graph](/README_Image/Image_MIT/Reset_graph.png)

Ce bouton permet de réinitialiser le graph.

---
### Bloc principal - Timer
![Timer](/README_Image/Image_MIT/Clock_screen.png)
Pour pouvoir récupérer automatiquement des données, on commande séquentiellement Arduino de nous evoyer les données. Pour cela, on utilise le composante "Clock"dans MIT. 

---
### Le Graphique
![Graph](/README_Image/Image_MIT/Graph.jpg)

Le graphique montre le variation de résistance du capteur FlexSensor et du capteur graphite à chaque mesure.

## 6. Réalisation du Shield

Pour la réalisation du shield, nous avons manuellement soudé l'ensemble des modules arduino et composants électroniques (résistances, capacités, amplificateur opérationnel) au PCB imprimé. (voir section [3.4 Réalisation du PCB](#34-réalisation-du-pcb)).

Le Shield réalisé intègre un amplificateur de signal LTC1050 pour amplifier les faibles signaux des capteurs avec précision, un encodeur rotatif permettant de naviguer dans le menu et de modifier les valeurs de résistance via un potentiomètre, un écran OLED pour afficher clairement les menus et les informations, ainsi qu’un module Bluetooth assurant la communication sans fil avec un téléphone portable pour le contrôle et la supervision à distance du système.

![shield](/README_Image/Shield.jpg)


---

## 7. Banc de test

Afin de valider le fonctionnement du système, plusieurs tests ont été réalisés :
- Mesure des signaux et tensions aux bornes des capteurs et calcul de résistance.
- Test de communication Bluetooth.
- Et prise de mesures de résistance issues du capteur en graphite.


Un modèle 3D composé de fentes circulaires de différents rayons est utilisé pour le banc de test. En insérant le capteur dans ces fentes, une déformation est induite en fonction de son orientation (traction ou compression). La déformation est inversement proportionnelle au rayon des fentes circulaires, selon la relation suivante : ε = e / R

  Où :  
  - ε : déformation 
  - e : allongement imposé  
  - R : rayon de la fente (0,2 mm)

Ce système permet de contrôler précisément la déformation du capteur en choisissant simplement un rayon de fente spécifique.

![model_test_bech](/README_Image/model_test_bench.jpg)

### 7.1 Physique derrière le capteur en graphite

Une variation de la résistance du capteur est attendue lorsque ce dernier est déformé. Cette variation est notamment dû au rapprochement ou à l'éloignement des atomes de carbone.
Lorsque le capteur est mis en tension, les atomes de carbone ont tendance à s'écarter. Ainsi, moins d'atome sont en contact afin de créer une chaîne reliant les deux extrémités du cpateur. Les électrons passent plus difficilement, la résistance augmente donc.
Lorsque le capteur est mis en compression, les atomes de carbone se rapprochent. La configuration est plus favorable à la création de chaîne reliant les deux extrémités du capteur. Les électrons passent plus facilement, la résistance diminue donc.
Par ailleurs, il est attendu à ce que la résistance diminue en même temps que la dureté du crayon de papier augmente. Cela peut simplement s'expliquer par une réduction de la présence de carbone.

![Schéma ](/README_Image/Schema_Graphite_Physics.png)


La résistance peut s'exprimer comme suivant:

![Schéma ](/README_Image/Formule_Resistance.png)

  Où :
  - "l" la longueur de la couche de nanoparticule (graphite) ;
  - "a" le paramètre de maille de la matrice cubique centrée du graphite ;
  - "w" la largeur, et "d" l'épaisseur du film.
Quant à la résistance "R12", elle est obtenue en posant les équations de mécanique quantique appliquée à des nanoparticules :

![Schéma ](/README_Image/Formule_G12.png)

![Schéma ](/README_Image/Formule_R12.png)

Pour plus d'informations, les sources utilisées sont fournies dans la rubrique [Références](#références).


### 7.2 Résultats de mesures

Par la suite, nous avons caractérisé notre capteur en graphite afin de retrouver les résultats de l'étude dont la source est donnée ci-dessus. La tension aux bornes du capteur est mesurée, puis convertie en valeur de résistance. Cette valeur de résistance est affichée en fonction de la déformation appliquée sur le capteur.
Les graphiques ci-dessus montrent la variation relative de la valeur du capteur en fonction de la déformation dans deux modes : traction et compression. Des droites de régression linéaire passant par l’origine ont été tracées. Ces lignes théoriques démontrent que la valeur de ΔR/R₀ varie linéairement avec la déformation.

![test_bench_tension](/README_Image/test_bench_tension.png)

![test_bench_compression](/README_Image/test_bench_compression.png)


---

## 8. Datasheet
Les fiches techniques des principaux composants (LTC1050, HC-05, écran OLED, etc.) sont disponibles dans le dossier [datasheet_graphit_sensor.pdf](./Datasheet/datasheet_graphit_sensor.pdf) du projet. Par ailleurs, une documentation sur notre capteur graphite est disponible sur ce Github.

---

# Conclusion
Ce projet démontre la faisabilité d’un capteur à base de graphite de crayon pour des applications d’instrumentation. Le prototype final est capable de détecter des variations de résistance liées à la pression exercée sur le graphite. Néanmoins, la faible répétabilité des mesures et de la confection du capteur en graphite ne permet pas une industrialisation. On note aussi que durant le test, le graphite du capteur est en contact avec les parois. Cela pourrait engendrer un retrait de graphite à chaque insertion.
Les points d'amélioration pouvant être apportés sont : une amélioration du banc de test, la normalisation du dépôt de graphite.

Une documentation de notre capteur est aussi disponible ([Documentation Capteur Graphite](./Datasheet/Datasheet_graphit_sensor.pdf))

---

# Références
- Fiches techniques des composants (HC-05, Keyes KY-040, SSD 1306, LLC 1070, LTC1050, MCP41100)
- Documentation Arduino
- Tutoriels MIT App Inventor
- Outils de simulation LTSpice et KiCad

<ins>Sources sur l'étude du capteur en graphite :</ins>

"Pencil Drawn Strain Gauges and Chemiresistors on Paper
Cheng-Wei Lin*, Zhibo Zhao*, Jaemyung Kim & Jiaxing Huang
Department of Materials Science and Engineering,
Northwestern University 2220 Campus Drive, Evanston, IL, 60208, USA.
SCIENTIFIC REPORTS | 4 : 3812 | DOI: 10.1038/srep03812 - 2014"

"Electron transport in nanoparticle assemblies
K.-H. Müller1, J. Herrmann2, G. Wei1, B. Raguse1, and L.Wieczorek1
1Future Manufacturing Flagship, 1CSIRO Materials Science and Engineering
Lindfield NSW2070, Australia
2NationalMeasurement Institute, Lindfield NSW 2070, Australia
978-1-4244-5262-0/10/$26.00 © 2010 IEEE - ICONN 2010"

"Percolation model for electron conduction in films of metal nanoparticles linked by organic molecules
K.-H. Müller,* J. Herrmann, B. Raguse, G. Baxter, and T. Reda
Commonwealth Scientific and Industrial Research Organization, Telecommunications and Industrial Physics,
Sydney 2070, Australia
PHYSICAL REVIEW B 66, 075417 (2002)"



---

# Contacts

## Étudiants
- **Yoann Lai Koun Sing** : laikouns@insa-toulouse.fr  
- **Viet Hoang Pham** : vpham@insa-toulouse.fr  
- **Ly Hai Hoang** : lhoang@insa-toulouse.fr  

## Enseignants
- **Jérémie Grisolia** : jeremie.grisolia@insa-toulouse.fr  
- **Arnauld Biganzoli** : arnauld.biganzoli@insa-toulouse.fr  
- **Cathy Crouzet** : crouzet@insa-toulouse.fr  
- **Benjamin Mestre** : benjamin.mestre@scalian.com
