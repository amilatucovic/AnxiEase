#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
 
// WiFi podaci
#define WIFI_SSID "mojatv_full_4658"
#define WIFI_PASSWORD "123456123456"
 
// Firebase podaci
#define FIREBASE_HOST "anxiease-c3e32-default-rtdb.firebaseio.com"
#define FIREBASE_API_KEY "AIzaSyBX3O7ZELMoCOndSXpOquJdoLf5JX7pVIE"
 
// Firebase objekti
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;
FirebaseData firebaseData;
 
MAX30105 particleSensor;
 
int red=2;
int green =0;
long lastBeat = 0;
float beatsPerMinute = 0.0;
bool fingerDetected = false;
bool measuring = false;
const long MIN_IR_VALUE = 5000;
const unsigned long measurementDuration = 15000; // 15 sekundi
unsigned long measurementStart = 0;
float bpmSum = 0.0;
int bpmCount = 0;
 
 
void setup() {

  pinMode(red, OUTPUT);   
  pinMode(green, OUTPUT);
  Serial.begin(115200);
  Serial.println("Initializing...");
 
  // Inicijalizacija senzora
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 nije pronađen. Provjerite veze.");
    while (1);
  }
 
  particleSensor.setup(0x1F, 8, 3, 100, 411, 4096);
 
  // Povezivanje na WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Povezivanje na WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi povezan!");
 
  // Konfiguracija Firebase-a
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.api_key = FIREBASE_API_KEY;
 
  if (!Firebase.signUp(&firebaseConfig, &firebaseAuth, "", "")) {
    Serial.printf("Autentifikacija nije uspjela: %s\n", firebaseConfig.signer.signupError.message.c_str());
    while (1);
  }
 
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Serial.println("Firebase povezan!");
}
 
void loop() {
 
 digitalWrite(red,LOW);
 digitalWrite(green,LOW);
 
  // Provjera IR vrijednosti za detekciju prsta
  long irValue = particleSensor.getIR();
  fingerDetected = (irValue > MIN_IR_VALUE);
 
  if (fingerDetected) {
    // Slanje statusa senzora na Firebase
    if (!Firebase.setString(firebaseData, "/device/sensorStatus", "Finger Detected")) {
      Serial.println("Greška pri slanju statusa senzora.");
    }
 
    // Provjera signala za početak mjerenja
    if (Firebase.getBool(firebaseData, "/device/startMeasurement")) {
      if (firebaseData.boolData() && !measuring) {
        Firebase.setBool(firebaseData, "/device/startMeasurement", false); // Reset signala
        startMeasurement();
      }
    }
  } else {
    // Ako nema prsta, šaljemo "No Finger Detected"
    if (!Firebase.setString(firebaseData, "/device/sensorStatus", "No Finger Detected")) {
      Serial.println("Greška pri slanju statusa senzora.");
    }
  }
 
  // Ako je mjerenje u toku, vršimo izračune
  if (measuring) {
    performMeasurement();
  }
 
  delay(100); // Stabilizacija petlje
}
 
void startMeasurement() {
  measuring = true;
  measurementStart = millis();
  bpmSum = 0.0;
  bpmCount = 0;
  Serial.println("Pokrenuto mjerenje...");
}
 
void performMeasurement() {
  long irValue = particleSensor.getIR();
 
  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
 
    if (delta > 300) { // Izbjegavamo prebrze intervale
      float bpm = 60 / (delta / 1000.0);
      if (bpm > 40 && bpm < 200) { // Validni BPM opseg
        bpmSum += bpm;
        bpmCount++;
        Serial.printf("Detektovan otkucaj: %.2f BPM\n", bpm);
      }
    }
  }
 
  // Provjera je li mjerenje završeno
  if (millis() - measurementStart >= measurementDuration) {
    measuring = false;
    float bpmAverage = (bpmCount > 0) ? bpmSum / bpmCount : 0.0;
    Serial.printf("Prosječan BPM: %.2f\n", bpmAverage);
    if(bpmAverage>=60 && bpmAverage<=100)
    {
      digitalWrite(green,HIGH);
      digitalWrite(red,LOW);
      delay(1500);
    }
    else{
       digitalWrite(green,LOW);
      digitalWrite(red,HIGH);
       delay(1500);
    }
    // Slanje rezultata na Firebase
    if (!Firebase.setFloat(firebaseData, "/device/bpmAverage", bpmAverage)) {
      Serial.println("Greška pri slanju prosječnog BPM-a.");
    } else {
      Serial.println("Rezultat uspješno poslan na Firebase!");
    }
  }
}
 
// Funkcija za detekciju otkucaja srca (koristi postojeću logiku)
boolean checkForBeat(long irValue) {
  static long prevIrValue = 0;
  static boolean prevState = false;
 
  boolean beatDetected = (irValue > prevIrValue + 20) && prevState && (millis() - lastBeat > 300);
  prevIrValue = irValue;
  prevState = (irValue > MIN_IR_VALUE);
 
  return beatDetected;
}