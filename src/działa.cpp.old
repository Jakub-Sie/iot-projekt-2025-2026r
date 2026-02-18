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

// POPRAWKA: Przekazujemy &Wire w konstruktorze
INA226 ina226_0(0x40, &Wire);
INA3221 ina3221_0(0x43, &Wire);

// --- Obiekty dla magistrali Wire1 (16/17) ---
Adafruit_AHTX0 aht20_1;

// POPRAWKA: Przekazujemy &Wire1 w konstruktorze
INA3221 ina3221_1(0x43, &Wire1); 

// --- Zmienne globalne do przechowywania wyników ---

// Dane z Wire (Bus 0)
float bus0_aht_temp = 0.0;
float bus0_aht_hum = 0.0;
float bus0_ina226_vbus = 0.0;
float bus0_ina3221_ch1_v = 0.0;
float bus0_ina3221_ch2_v = 0.0;
float bus0_ina3221_ch3_v = 0.0;

// Dane z Wire1 (Bus 1)
float bus1_aht_temp = 0.0;
float bus1_aht_hum = 0.0;
float bus1_ina3221_ch1_v = 0.0;
float bus1_ina3221_ch2_v = 0.0;
float bus1_ina3221_ch3_v = 0.0;

//FLAGI

bool byloKrecone=false;

//Piny

const int restartKrecenia= 15;
const int errorPin= 2;
const int gitPiN= 13;

//Wyjścia do przekaźników

const int sanieOnPin = 14;
const int sanieOffPin = 27;
const int rozrusznikOnPin = 26;
const int gaszeniePin = 25;
const int Pin5 = 33;
const int Pin6 = 32;

void przelocz(int time, int what){
  digitalWrite(what,LOW);
  delay(time);
  digitalWrite(what,HIGH);
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
  
  // AHT21 (Adafruit wymaga &Wire w begin)
  if (!aht20_0.begin(&Wire)) {
    Serial.println("Blad: Nie znaleziono AHT21 na Wire!");
  } else {
    Serial.println("AHT21 na Wire OK");
  }

  // INA226 (Rob Tillaart wymaga pustego begin, Wire podano wyżej)
  if (!ina226_0.begin()) {
    Serial.println("Blad: Nie znaleziono INA226 (0x40) na Wire!");
  } else {
    ina226_0.setMaxCurrentShunt(20, 0.002);
    Serial.println("INA226 (0x40) na Wire OK");
  }

  // INA3221 (Rob Tillaart wymaga pustego begin)
  ina3221_0.begin();
  // Sprawdzamy ID producenta, żeby potwierdzić połączenie
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

  // INA3221 na Wire1 (zdefiniowane w konstruktorze)
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
  
  // AHT21
  aht20_0.getEvent(&humidity, &temp);
  bus0_aht_temp = temp.temperature;
  bus0_aht_hum = humidity.relative_humidity;

  // INA226
  bus0_ina226_vbus = ina226_0.getBusVoltage();

  // INA3221
  bus0_ina3221_ch1_v = ina3221_0.getBusVoltage(0);
  bus0_ina3221_ch2_v = ina3221_0.getBusVoltage(1);
  bus0_ina3221_ch3_v = ina3221_0.getBusVoltage(2);

  // ========================================================
  // ODCZYT DANYCH Z WIRE1 (Bus 1)
  // ========================================================

  // AHT21
  aht20_1.getEvent(&humidity, &temp);
  bus1_aht_temp = temp.temperature;
  bus1_aht_hum = humidity.relative_humidity;

  // INA3221
  bus1_ina3221_ch1_v = ina3221_1.getBusVoltage(0);
  bus1_ina3221_ch2_v = ina3221_1.getBusVoltage(1);
  bus1_ina3221_ch3_v = ina3221_1.getBusVoltage(2);

  // ========================================================
  // ODCZYTYWANIE Z PIN
  // ========================================================

  // PIN restartKrecenia

  if (digitalRead(restartKrecenia)==LOW){
    byloKrecone=false;
  }

  // ========================================================
  // WYPISYWANIE WYNIKÓW
  // ========================================================

  Serial.println("\n--- ODCZYT DANYCH ---");
  
  Serial.println("Flagi: ");
  Serial.println("-byloKrecone"); Serial.println(byloKrecone); 



  Serial.println("[MAGISTRALA 0 (21/22)]");
  Serial.print("AHT21:   Temp: "); Serial.print(bus0_aht_temp); Serial.print(" C, Wilg: "); Serial.print(bus0_aht_hum); Serial.println(" %");
  Serial.print("INA226:  VBS:  "); Serial.print(bus0_ina226_vbus, 3); Serial.println(" V");
  Serial.print("INA3221: CH1:  "); Serial.print(bus0_ina3221_ch1_v, 3); Serial.print(" V, CH2: "); 
  Serial.print(bus0_ina3221_ch2_v, 3); Serial.print(" V, CH3: "); Serial.print(bus0_ina3221_ch3_v, 3); Serial.println(" V");

  Serial.println("[MAGISTRALA 1 (16/17)]");
  Serial.print("AHT21:   Temp: "); Serial.print(bus1_aht_temp); Serial.print(" C, Wilg: "); Serial.print(bus1_aht_hum); Serial.println(" %");
  Serial.print("INA3221: CH1:  "); Serial.print(bus1_ina3221_ch1_v, 3); Serial.print(" V, CH2: "); 
  Serial.print(bus1_ina3221_ch2_v, 3); Serial.print(" V, CH3: "); Serial.print(bus1_ina3221_ch3_v, 3); Serial.println(" V");


  
  digitalWrite(gitPiN,HIGH);
  delay(1000);
  digitalWrite(gitPiN,LOW);
  delay(1000);

  if (!byloKrecone&&bus0_ina3221_ch2_v>12){
      digitalWrite(errorPin,HIGH);
      delay(1000);
      digitalWrite(errorPin,LOW);
      delay(1000);
  }




  if (byloKrecone){
    digitalWrite(errorPin,HIGH);

  } else {
    digitalWrite(errorPin,LOW);
  }


  // --- Sekcja sterowania agregatem ---

  // Jeśli nie było jeszcze próby (lub zresetowano przyciskiem) i napięcie jest niskie
  if (!byloKrecone && bus0_ina3221_ch1_v < 12.2) { 

    digitalWrite(gitPiN,HIGH);


    for (int i = 0; i < 5; i++) {
      Serial.print("Próba odpalenia nr: "); Serial.println(i + 1);

      przelocz(1000, sanieOnPin);      // Ssanie
      przelocz(5000, rozrusznikOnPin); // Kręcenie 5s

      // KLUCZOWE: Musisz odczytać napięcie PONOWNIE po kręceniu, 
      // inaczej pętla używa starej wartości sprzed 'for'
      bus0_ina3221_ch2_v = ina3221_0.getBusVoltage(1);

      Serial.print(bus0_ina3221_ch2_v);

      // Jeśli napięcie na CH2 (alternator) wzrosło > 13V, znaczy że pracuje
      

      if (bus0_ina3221_ch2_v > 13.0) {
        Serial.println("Sukces! Silnik pracuje.");
        break; // Wychodzimy z pętli for, bo odpalił
      }
      
      Serial.println("Nie odpalił, czekam na kolejną próbę...");
      delay(3000); // Przerwa dla akumulatora
    }

    // Po zakończeniu pętli (niezależnie czy sukces, czy 5 prób)
    delay(5000); 
    przelocz(1000, sanieOffPin); // Cofnięcie ssania

    // Finalne sprawdzenie czy działa
    digitalWrite(gitPiN,LOW);

    bus0_ina3221_ch2_v = ina3221_0.getBusVoltage(1);
    if (bus0_ina3221_ch2_v > 12.5) {
      Serial.print("dzałaaa");
    } else {
        // Jeśli po 5 próbach nie ma napięcia na alternatorze
        byloKrecone = true; // Blokujemy dalsze próby, by nie spalić rozrusznika
        Serial.println("ALARM: Agregat nie odpalił po 5 próbach!");
    }
  }


  if (bus0_ina3221_ch1_v>14){
    przelocz(5000,gaszeniePin);
  }


}