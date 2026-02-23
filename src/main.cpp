#include <Arduino.h> 
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <INA226.h>
#include <INA3221.h>

// --- Definicje Pinów I2C ---
#define SDA_0 21
#define SCL_0 22

#define SDA_1 16
#define SCL_1 17

// --- Obiekty dla magistrali Wire (21/22) ---
Adafruit_AHTX0 aht20_0;
INA226 ina226_0(0x40, &Wire);      
INA226 ina226_1(0x41, &Wire);      
INA3221 ina3221_0(0x43, &Wire);

// --- Obiekty dla magistrali Wire1 (16/17) ---
Adafruit_AHTX0 aht20_1;
INA226 ina226_2(0x40, &Wire1);     
INA3221 ina3221_1(0x43, &Wire1); 

// --- Zmienne globalne do przechowywania wyników ---

// Dane z Wire (Bus 0)
float bus0_aht_temp = 0.0;
float bus0_aht_hum = 0.0;
float bus0_ina226_0x40_vbus = 0.0;
float bus0_ina226_0x41_vbus = 0.0;
float bus0_ina3221_ch1_v = 0.0;
float bus0_ina3221_ch2_v = 0.0;
float bus0_ina3221_ch3_v = 0.0;

// Dane z Wire1 (Bus 1)
float bus1_aht_temp = 0.0;
float bus1_aht_hum = 0.0;
float bus1_ina226_0x40_vbus = 0.0;
float bus1_ina3221_ch1_v = 0.0;
float bus1_ina3221_ch2_v = 0.0;
float bus1_ina3221_ch3_v = 0.0;

//FLAGI
bool errorKrecenia = false;

//Piny
const int restartKrecenia = 15;
const int errorPin = 2;
const int gitPiN = 13;

//Wyjścia do przekaźników
const int sanieOnPin = 14;
const int sanieOffPin = 27;
const int rozrusznikOnPin = 26;
const int gaszeniePin = 25;
const int Pin5 = 33;
const int Pin6 = 32;

void przelocz(int time, int what){
  digitalWrite(what, LOW);
  delay(time);
  digitalWrite(what, HIGH);
}

void setup() {
  pinMode(restartKrecenia, INPUT_PULLUP);
  pinMode(gitPiN, OUTPUT);
  pinMode(errorPin, OUTPUT);
  pinMode(sanieOnPin, OUTPUT);
  pinMode(sanieOffPin, OUTPUT);
  pinMode(rozrusznikOnPin, OUTPUT);
  pinMode(gaszeniePin, OUTPUT);
  pinMode(Pin5, OUTPUT);
  pinMode(Pin6, OUTPUT);

  digitalWrite(sanieOnPin, HIGH);
  digitalWrite(sanieOffPin, HIGH);
  digitalWrite(rozrusznikOnPin, HIGH);
  digitalWrite(gaszeniePin, HIGH);
  digitalWrite(Pin5, HIGH);
  digitalWrite(Pin6, HIGH);

  Serial.begin(115200);
  while (!Serial);
  Serial.println("\nStartuje system pomiarowy...");

  // 1. Inicjalizacja fizyczna magistral I2C
  Wire.begin(SDA_0, SCL_0);
  Wire1.begin(SDA_1, SCL_1);

  // ---------------------------------------------------------
  // 2. Inicjalizacja czujników na Wire (21/22)
  // ---------------------------------------------------------

  Serial.println("--- Inicjalizacja Wire (21/22) ---");
  
  if (!aht20_0.begin(&Wire)) {
    Serial.println("Blad: Nie znaleziono AHT21 na Wire!");
  } else {
    Serial.println("AHT21 na Wire OK");
  }

  if (!ina226_0.begin()) {
    Serial.println("Blad: Nie znaleziono INA226 (0x40) na Wire!");
  } else {
    Serial.println("INA226 (0x40) na Wire OK");
  }

  if (!ina226_1.begin()) {
    Serial.println("Blad: Nie znaleziono INA226 (0x41) na Wire!");
  } else {
    Serial.println("INA226 (0x41) na Wire OK");
  }

  ina3221_0.begin();
  if (ina3221_0.getManufacturerID() == -1) { 
    Serial.println("Blad: Nie znaleziono INA3221 (0x43) na Wire!");
  } else {
    Serial.println("INA3221 (0x43) na Wire OK");
  }

  // ---------------------------------------------------------
  // 3. Inicjalizacja czujników na Wire1 (16/17)
  // ---------------------------------------------------------

  Serial.println("--- Inicjalizacja Wire1 (16/17) ---");

  if (!aht20_1.begin(&Wire1)) { 
    Serial.println("Blad: Nie znaleziono AHT21 na Wire1!");
  } else {
    Serial.println("AHT21 na Wire1 OK");
  }

  if (!ina226_2.begin()) {
    Serial.println("Blad: Nie znaleziono INA226 (0x40) na Wire1!");
  } else {
    Serial.println("INA226 (0x40) na Wire1 OK");
  }

  ina3221_1.begin();
  if (ina3221_1.getManufacturerID() == -1) {
    Serial.println("Blad: Nie znaleziono INA3221 (0x43) na Wire1!");
  } else {
    Serial.println("INA3221 (0x43) na Wire1 OK");
  }
  
  Serial.println("-----------------------------------");
  delay(1000);
}

void loop() {
  sensors_event_t humidity, temp;

  // ========================================================
  // ODCZYT DANYCH Z WIRE (Bus 0)
  // ========================================================
  
  aht20_0.getEvent(&humidity, &temp);
  bus0_aht_temp = temp.temperature;
  bus0_aht_hum = humidity.relative_humidity;

  bus0_ina226_0x40_vbus = ina226_0.getBusVoltage();
  bus0_ina226_0x41_vbus = ina226_1.getBusVoltage();

  bus0_ina3221_ch1_v = ina3221_0.getBusVoltage(0);
  bus0_ina3221_ch2_v = ina3221_0.getBusVoltage(1);
  bus0_ina3221_ch3_v = ina3221_0.getBusVoltage(2);

  // ========================================================
  // ODCZYT DANYCH Z WIRE1 (Bus 1)
  // ========================================================

  aht20_1.getEvent(&humidity, &temp);
  bus1_aht_temp = temp.temperature;
  bus1_aht_hum = humidity.relative_humidity;

  bus1_ina226_0x40_vbus = ina226_2.getBusVoltage();

  bus1_ina3221_ch1_v = ina3221_1.getBusVoltage(0);
  bus1_ina3221_ch2_v = ina3221_1.getBusVoltage(1);
  bus1_ina3221_ch3_v = ina3221_1.getBusVoltage(2);

  // ========================================================
  // ODCZYTYWANIE Z PIN
  // ========================================================

  if (digitalRead(restartKrecenia) == LOW) {
    errorKrecenia = false;
  }

  // ========================================================
  // WYPISYWANIE WYNIKÓW
  // ========================================================

  Serial.println("\n===================================");
  Serial.println("        --- ODCZYT DANYCH ---");
  Serial.println("===================================");
  
  Serial.print("Flaga 'errorKrecenia': "); 
  Serial.println(errorKrecenia ? "TAK" : "NIE"); 
  Serial.println("-----------------------------------");

  // Wypisywanie dla Magistrali 0
  Serial.println("[MAGISTRALA 0 (Piny 21/22)]");
  Serial.print("AHT21:        Temp: "); Serial.print(bus0_aht_temp); Serial.print(" °C, Wilg: "); Serial.print(bus0_aht_hum); Serial.println(" %");
  Serial.print("INA226(0x40): Napięcie (VBUS): "); Serial.print(bus0_ina226_0x40_vbus, 3); Serial.println(" V");
  Serial.print("INA226(0x41): Napięcie (VBUS): "); Serial.print(bus0_ina226_0x41_vbus, 3); Serial.println(" V");
  Serial.print("INA3221:      CH1: "); Serial.print(bus0_ina3221_ch1_v, 3); 
  Serial.print(" V | CH2: "); Serial.print(bus0_ina3221_ch2_v, 3); 
  Serial.print(" V | CH3: "); Serial.print(bus0_ina3221_ch3_v, 3); Serial.println(" V");

  Serial.println("-----------------------------------");

  // Wypisywanie dla Magistrali 1
  Serial.println("[MAGISTRALA 1 (Piny 16/17)]");
  Serial.print("AHT21:        Temp: "); Serial.print(bus1_aht_temp); Serial.print(" °C, Wilg: "); Serial.print(bus1_aht_hum); Serial.println(" %");
  Serial.print("INA226(0x40): Napięcie (VBUS): "); Serial.print(bus1_ina226_0x40_vbus, 3); Serial.println(" V");
  Serial.print("INA3221:      CH1: "); Serial.print(bus1_ina3221_ch1_v, 3); 
  Serial.print(" V | CH2: "); Serial.print(bus1_ina3221_ch2_v, 3); 
  Serial.print(" V | CH3: "); Serial.print(bus1_ina3221_ch3_v, 3); Serial.println(" V");
  
  Serial.println("===================================");

  // ========================================================
  // LOGIKA STEROWANIA I ALARMY
  // ========================================================
  
  digitalWrite(gitPiN, HIGH);
  delay(1000);
  digitalWrite(gitPiN, LOW);
  delay(1000);

  if (!errorKrecenia && bus0_ina3221_ch2_v > 12) {
      digitalWrite(errorPin, HIGH);
      delay(1000);
      digitalWrite(errorPin, LOW);
      delay(1000);
  }

  if (errorKrecenia) {
    digitalWrite(errorPin, HIGH);
  } else {
    digitalWrite(errorPin, LOW);
  }

  // --- Sekcja sterowania agregatem ---

  if (!errorKrecenia && bus0_ina3221_ch1_v < 12.2 && bus0_ina3221_ch1_v > 0) {

    digitalWrite(gitPiN, HIGH);

    for (int i = 0; i < 5; i++) {
      Serial.print("Próba odpalenia nr: "); Serial.println(i + 1);

      przelocz(1000, sanieOnPin);      
      przelocz(5000, rozrusznikOnPin); 

      bus0_ina3221_ch2_v = ina3221_0.getBusVoltage(1);
      Serial.print("Napięcie po kręceniu (CH2): ");
      Serial.println(bus0_ina3221_ch2_v);

      if (bus0_ina3221_ch2_v > 13.0) {
        Serial.println("Sukces! Silnik pracuje.");
        break; 
      }
      
      Serial.println("Nie odpalił, czekam na kolejną próbę...");
      delay(3000); 
    }

    delay(5000); 
    przelocz(1000, sanieOffPin); 

    digitalWrite(gitPiN, LOW);

    bus0_ina3221_ch2_v = ina3221_0.getBusVoltage(1);
    if (bus0_ina3221_ch2_v > 12.5) {
      Serial.println("Działa!");
    } else {
        errorKrecenia = true; 
        Serial.println("ALARM: Agregat nie odpalił po 5 próbach!");
    }
  }

  if (bus0_ina3221_ch1_v > 14) {
    przelocz(5000, gaszeniePin);
  }
}