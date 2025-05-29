/*
==============================================
=============== Projet Capteur ===============
==============================================
Authors: Yoann LAI KOUN SING, Viet Hoang PHAM, Ly Hai HOANG
Last update: 29/05/2025
*/


//====================================================
//==================== Libraries =====================
//====================================================

#include <Adafruit_SSD1306.h>  // Library of the OLED screen
#include <Wire.h>              // Library of the OLED screen
// #include <Servo.h>             // Library of the servo motor     BIBLIOTHEQUE Servo PAS UTILISABLE AVEC SoftwareSerial
#include <SPI.h>               // Library for SPI to control Digital Potentiometer
#include <SoftwareSerial.h>    // Library for HC-05 bluetooth module
#include <stdlib.h>


//====================================================
//==================== Declaration ===================
//====================================================

// _____ OLED Screen _____
#define nombreDePixelsEnLargeur 128
#define nombreDePixelsEnHauteur 64
#define brocheResetOLED -1
#define adresseI2CecranOLED 0x3C
Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);
// _____ Rotary Encoder _____
#define pinEncoder_CLK 2  // CLK output
#define pinEncoder_DT 3   // DT Output
#define pinEncoder_SW 4   // Switch connection
volatile int encoderPos = 0;
volatile int encoderPosBefore = 0;
volatile int encoderButton = 0;
volatile int encoderButtonBefore = 0;
volatile int MenuPos = -1;
volatile int MenuPosBefore = -1;
int buttonState;
int lastButtonState = HIGH;
long lastDebounceTime = 0;
long debounceDelay = 50;
// _____ Digital Potentiometer _____
// #define pinPot_CS 10      //pin 10 to control Digital Potentiometer
const byte pinPot_CS = 10;
#define pinPot_SCK 13
#define pinPot_SDI 11
const int  maxPositions = 256;
const long rAB = 52700;
const byte rWiper = 125; 
const byte pot0 = 0x11;
const byte pot0Shutdown = 0x21;
volatile float R_pot = 0 ;
// _____ Flex Sensor _____
#define pinFlexSensor A2            // Pin connected to voltage divider output
const float VCC = 5.0;                   // Voltage at Ardunio 5V line
const float R_DIV = 1000;                // Resistor [ohm] used to create a voltage divider
const float flatResistance = 35994.36;   // Resistance when flat
const float bendResistance = 120000.0;   // Resistance at 90 deg bending (80000)
// _____ Graphite Sensor _____
#define pinGraphiteSensor A0
volatile float R_graphite = 0 ;
const float R1 = 100000;      // Resistance in [ohm]
const float R3 = 100000;      // Resistance in [ohm]
const float R5 = 10000;       // Resistance in [ohm]
const float R6 = 1000;        // Resistance in [ohm]
const float C1 = 0.0000001;   // Capa in [F]
const float C2 = 0.0000001;   // Capa in [F]
const float C4 = 0.000001;   // Capa in [F]
// _____ Bluetooth _____
#define pinBT_TXD 6
#define pinBT_RXD 5
#define BTbuffer 8
SoftwareSerial MyBT(pinBT_RXD, pinBT_TXD) ;
volatile float DataToSend = 0.0 ;
// _____ Motor _____
#define pinMoteur 9
                       // Create an object My_servo to communicate with the Servo-motor
// _____ Other Parameters _____
#define baudrate 9600
const int DeltaTime = 200 ;
unsigned long currentTime;
unsigned long previousTime = 0;


//====================================================
//====================== Setup =======================
//====================================================
void setup() {
  Serial.begin(baudrate);
  // _____ OLED Screen _____
  Set_OLED();
  // _____ Bluetooth _____
  MyBT.begin(9600);
  InitBluetooth();
  // _____ Rotary Encoder _____
  Set_RotaryEncoder();
  attachInterrupt(digitalPinToInterrupt(pinEncoder_CLK), doEncoder, RISING);
  // _____ Digital Potentiometer _____
  Set_DigitalPotentiometer();
  // _____ Flex Sensor _____
  pinMode(pinFlexSensor, INPUT);
  digitalWrite(pinFlexSensor, LOW);
  // _____ Graphite Sensor _____
  pinMode(pinGraphiteSensor, INPUT);
  // attachInterrupt(digitalPinToInterrupt(pinBT_RXD), ReceiveDataBluetooth, RISING);
  // _____ Servo Moteur _____
  pinMode(pinMoteur, OUTPUT);
  digitalWrite(pinMoteur, LOW);
  // 
  Serial.println();
  Serial.println("----- Programme Capteur Start -----");
}


//====================================================
//==================== Main Loop =====================
//====================================================
void loop() {
  // Button of the Rotary encoder
  doEncoderButton();
  
  // Refresh OLED screen at each iteration
  DisplayOLED();
  
  // Get sensor mesurement
  currentTime = millis();
  if ((currentTime - previousTime >= DeltaTime) && ( (ChoixCapteur == 110) || (ChoixCapteur == 111) )  ){
    previousTime = currentTime ;
    Sensor_Mesurement(ChoixCapteur);
  }
  
  // Receive message from Bluetooth
  char received_message[32]={0};
  if (MyBT.available() > 0){
    int i=0;
    while (MyBT.available() > 0 && i < 31) { 
        received_message[i++] = MyBT.read();
        delay(4);
    }
    //received_message[i] = '\0';

    Serial.print(received_message);

    if (strstr(received_message, "F")){
      float R_flex= Flex_Mesure();
      MyBT.print(R_flex);
    }
    if (strstr(received_message, "G")){
      int R_graph = Graphite_Mesure();
      MyBT.print(R_graph);
    }

  delay(10);
  }
}



//====================================================
//==================== Functions =====================
//====================================================

//==================== Function for OLED Screen ====================
      // The initailise the OLED screen's parameters
void Set_OLED() {
  if (!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED)) {
    Serial.println("Initialisation OLED screen NO");
  }
  ecranOLED.clearDisplay();         // Effacage de l'intégralité du buffer
  InitOLED();
  ecranOLED.println(F("Main Menu:"));
  ecranOLED.println(F(">Fonction")) ;
  ecranOLED.println(F(">Capteur")) ;
  ecranOLED.display();
}
void InitOLED(){
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);
  ecranOLED.setCursor(0, 0);
  ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
}
      // OLED_CouleurInverse(bool Inverser): Si vrai alors inverse la couleur: texte en noir en fond en blanc
void OLED_CouleurInverse(bool Inverser) {
  if (Inverser == true) {
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);       // (Couleur du texte: BLACK, Couleur du fond: WHITE)
  }
  else {
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  }
}
      // 1er indice indique le niveau du menu, 2nd indice indique le choix
void OLED_Menu0_0(int Val) {
  InitOLED();
  ecranOLED.println(F("Main Menu:"));
  switch (Val){
    case 0 :
      OLED_CouleurInverse(true) ;
      ecranOLED.println(F(">Fonction")) ;
      OLED_CouleurInverse(false) ;
      ecranOLED.println(F(">Capteur")) ;
      MenuPos = 0;
      break;
    case 1 :
      ecranOLED.println(F(">Fonction")) ;
      OLED_CouleurInverse(true) ;
      ecranOLED.println(F(">Capteur")) ;
      OLED_CouleurInverse(false) ;
      MenuPos = 1;
      break;
    default :
      ecranOLED.println(F(">Fonction")) ;
      ecranOLED.println(F(">Capteur")) ;
      MenuPos = -1;
      break;
  }
  ecranOLED.display();
}
      // Menu de Niveau 1, choix 0 du Niveau 0.
void OLED_Menu1_0(int Val) {
  InitOLED();
  ecranOLED.println(F("Fonction:"));
  switch (Val){
    case 0 :
      OLED_CouleurInverse(true) ;
      ecranOLED.println(F(">Fonction1")) ;        // Prise de Mesure
      OLED_CouleurInverse(false) ;
      ecranOLED.println(F(">Fonction2")) ;
      ecranOLED.println(F("<<Exit")) ;
      MenuPos = 100;
      break;
    case 1 :
      ecranOLED.println(F(">Fonction1")) ;
      OLED_CouleurInverse(true) ;
      ecranOLED.println(F(">Fonction2")) ;
      OLED_CouleurInverse(false) ;
      ecranOLED.println(F("<<Exit")) ;
      MenuPos = 101;
      break;
    case 2 :
      ecranOLED.println(F(">Fonction1")) ;
      ecranOLED.println(F(">Fonction2")) ;
      OLED_CouleurInverse(true) ;
      ecranOLED.println(F("<<Exit")) ;
      OLED_CouleurInverse(false) ;
      MenuPos = 102;
      break;
    default :
      ecranOLED.println(F(">Fonction1")) ;
      ecranOLED.println(F(">Fonction2")) ;
      ecranOLED.println(F("<<Exit")) ;
  }
  ecranOLED.display();
}
      // Menu de Niveau 1, choix 1 du Niveau 0.
void OLED_Menu1_1(int Val) {
  InitOLED();
  ecranOLED.println(F("Capteur:"));
  switch (Val){
    case 0 :
      OLED_CouleurInverse(true) ;
      ecranOLED.println(F(">Capteur1")) ;
      OLED_CouleurInverse(false) ;
      ecranOLED.println(F(">Capteur2")) ;
      ecranOLED.println(F("<<Exit")) ;
      MenuPos = 110;
      break;
    case 1 :
      ecranOLED.println(F(">Capteur1")) ;
      OLED_CouleurInverse(true) ;
      ecranOLED.println(F(">Capteur2")) ;
      OLED_CouleurInverse(false) ;
      ecranOLED.println(F("<<Exit")) ;
      MenuPos = 111;
      break;
    case 2 :
      ecranOLED.println(F(">Capteur1")) ;
      ecranOLED.println(F(">Capteur2")) ;
      OLED_CouleurInverse(true) ;
      ecranOLED.println(F("<<Exit")) ;
      OLED_CouleurInverse(false) ;
      MenuPos = 112;
      break;
    default :
      ecranOLED.println(F(">Capteur1")) ;
      ecranOLED.println(F(">Capteur2")) ;
      ecranOLED.println(F("<<Exit")) ;
      break;
  }
  ecranOLED.display();
}
void ExitMenu(){
  Serial.println("Exit");
  encoderButton = 0 ;
  encoderPos = 0 ;
  MenuPos = -1 ;
  MenuPosBefore = -1;
}
      // DisplayOLED(): Fonction d'affichage de l'écran OLED 
void DisplayOLED() {
  if ( (encoderPosBefore != encoderPos) || (encoderButtonBefore != encoderButton) ) {
    if (encoderButtonBefore != encoderButton) {
      MenuPosBefore = MenuPos ;
      encoderButtonBefore = encoderButton;
      encoderPos = 0;
    }

    switch (encoderButton) {
      case 0 :
        OLED_Menu0_0(abs(encoderPos % 2));     // encoder modulo nb de choix - 1
        break;
      case 1 :
        if (MenuPosBefore == 0) {               // Menu Fonction
          OLED_Menu1_0(abs(encoderPos % 3));
        }
        else if (MenuPosBefore == 1) {          // Menu Capteur
          OLED_Menu1_1(abs(encoderPos % 3));
        }
        break;
      case 2 :
        switch (MenuPosBefore) {
          case 100 :                            // Fonction 1: Prise de mesure
            InitOLED();
            OLED_CouleurInverse(true) ;
            ecranOLED.println(F(">Fonction1")) ;
            OLED_CouleurInverse(false) ;
            ecranOLED.display();
            // setPotWiper(pot0, 128);
            MenuPosBefore = 100 ;
            break;
          case 101 :                            // Fonction 2
            InitOLED();
            OLED_CouleurInverse(true) ;
            Serial.println(F("Fonction 2"));
            OLED_CouleurInverse(false) ;
            ecranOLED.display();
            // setPotWiper(pot0, 255);
            MenuPosBefore = 101 ;
            break;
          case 102 :
            ExitMenu();
            break;
          case 110 :                             // Capteur1: Flex Sensor
            InitOLED();
            OLED_CouleurInverse(true) ;
            ecranOLED.println(F("FlexSensor")) ;
            OLED_CouleurInverse(false) ;
            ecranOLED.display();
            MenuPosBefore = 110 ;
        
            ChoixCapteur = 110 ;
            
            break;
          case 111 :                             // Capteur2: Graphite Sensor
            InitOLED();
            OLED_CouleurInverse(true) ;
            ecranOLED.println(F("Graphite")) ;
            OLED_CouleurInverse(false) ;
            ecranOLED.display();
            MenuPosBefore = 111 ;
            ChoixCapteur = 111 ;
            
            break;
          case 112 :
            ExitMenu();
            break;
        }
        break;
      case 3:
        break;
      default:
        InitOLED();
        ecranOLED.println(F("Main Menu:")) ;
        ecranOLED.println(F(">Fonction")) ;
        ecranOLED.println(F(">Capteur")) ;
        ecranOLED.display();
        encoderButton = 0 ;
        encoderButtonBefore = encoderButton;
        encoderPos = 0;
        MenuPos = -1 ;
        MenuPosBefore = -1;
        ChoixCapteur = 0;
    }
    encoderPosBefore = encoderPos;
  }
}

//==================== Function for Rotary encoder ====================
      // Initialise the Rotary encoder
void Set_RotaryEncoder(){
  pinMode(pinEncoder_CLK, INPUT);
  digitalWrite(pinEncoder_CLK, HIGH);
  pinMode(pinEncoder_DT, INPUT);
  digitalWrite(pinEncoder_DT, HIGH);
  pinMode(pinEncoder_SW, INPUT);
}
      // To get the encoder position
void doEncoder() {
  if (digitalRead(pinEncoder_CLK) == HIGH && digitalRead(pinEncoder_DT) == HIGH) {
    encoderPos-- ;
  }
  else if (digitalRead(pinEncoder_CLK) == HIGH && digitalRead(pinEncoder_DT) == LOW) {
    encoderPos++ ;
  }

  Serial.print("encoderPos=");
  Serial.print(encoderPos);
  Serial.print(" ; encoderButtonBefore=");
  Serial.print(encoderButtonBefore);
  Serial.print(" ; encoderButton=");
  Serial.print(encoderButton);
  Serial.print(" ; MenuPos=");
  Serial.println(MenuPos);
}
      // To get the position of the encoder's button
void doEncoderButton() {
  int valeur = digitalRead(pinEncoder_SW) ;
  if (valeur != lastButtonState){
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime)> debounceDelay){
    if ( (valeur == LOW) && (valeur != buttonState) ){
      encoderButton++ ;
    }
    buttonState = valeur ;
  }
  lastButtonState = valeur ;
}

//==================== Function for Sensors ====================
        //========== flex Sensor ==========
float Flex_Mesure(){
  //Calculation of the flex sensor's resistance
  int ADC_flex = analogRead(pinFlexSensor);
  float V_flex = (ADC_flex * VCC) / 1023.0;
  float R_flex = R_DIV * ( V_flex / (VCC-V_flex));
  long angle  = map(R_flex, flatResistance, bendResistance, 0, 90);

  return R_flex; 
}
        //========== Graphite Sensor ==========
float Graphite_Mesure(){
  //Calculation of the Graphite sensor's resistance
  int mesure = analogRead(pinGraphiteSensor);
  float V_ADC = mesure * VCC / 1023.0 ;
  float R_graph = (1+R3/R_pot)*R1*(VCC/V_ADC)-(R1+R5) ;
  
  return R_graph;
}
        //========== Capteur Global Function ==========
void Sensor_Mesurement(int PositionMenu){
  switch (PositionMenu){
    case 110:                             // Flex Sensor
      DataToSend = Flex_Mesure();
      break;
    case 111:                             // Graphite Sensor
      DataToSend = Graphite_Mesure();
      break;
    default:
      break;
  }
}

//==================== Digital Potentiometer ====================
      // Initialise the Digital Potentiometer
void Set_DigitalPotentiometer(){
  pinMode(pinPot_CS, OUTPUT);
  digitalWrite(pinPot_CS, HIGH);
  SPI.begin();
  setPotWiper(pot0, 128);
}
void setPotWiper(int addr, int pos){
  pos = constrain(pos, 0, 255);            // limit wiper setting to range of 0 to 255
  digitalWrite(pinPot_CS, LOW);                // select chip
  SPI.transfer(addr);                      // configure target pot with wiper position
  SPI.transfer(pos);
  digitalWrite(pinPot_CS, HIGH);               // de-select chip

  // print pot resistance between wiper and B terminal
  R_pot = ((rAB * pos) / maxPositions ) + rWiper ;
}

//==================== Function for Bluetooth ====================
      // Initialise the Bluetooth
void InitBluetooth(){
  pinMode(pinBT_RXD, INPUT);
  pinMode(pinBT_TXD, OUTPUT);
  MyBT.begin(baudrate);
}

