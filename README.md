# ESP8266 Terrarium Controller met Firebase 🦎

Een geavanceerde terrarium controller gebaseerd op de ESP8266 microcontroller met real-time monitoring, Firebase dataopslag en dual-zone temperatuurregeling.

## 📝 Inhoudsopgave

- [Functies](#-functies)
- [Hardware Benodigdheden](#-hardware-benodigdheden)
- [Software Vereisten](#-software-vereisten)
- [Firebase Setup](#-firebase-setup)
- [Installatie](#-installatie)
- [Configuratie](#-configuratie)
- [Gebruiksmodi](#-gebruiksmodi)
- [Probleemoplossing](#-probleemoplossing)

## ✨ Functies

- **Dual-zone monitoring**:
  - 2x DHT22 sensoren voor temperatuur en luchtvochtigheid
  - 2x DS18B20 sensoren voor nauwkeurige temperatuurmeting
  - Gescheiden controle voor linker en rechter zone

- **Real-time monitoring**:
  - 20x4 I2C LCD display
  - Firebase integratie voor data logging
  - Minuutlijkse data-uploads
  - NTP tijdsynchronisatie

- **Intelligente temperatuurregeling**:
  - Automatische verwarming per zone
  - RGB LED statusindicatie
  - 3 verschillende modi (Dag/Nacht/Vervelling)

## 🛠️ Hardware Benodigdheden

- ESP8266 ontwikkelbord (NodeMCU of Wemos D1 Mini)
- 2x DHT22 temperatuur- en luchtvochtigheidssensor
- 2x DS18B20 temperatuursensor
- 20x4 I2C LCD display
- 2x Relais module
- 2x RGB LED
- Drukknop
- 4.7kΩ pull-up weerstanden (voor DS18B20)
- Breadboard en jumper draden

## 💻 Software Vereisten

1. **Arduino IDE**
   - [Download Arduino IDE](https://www.arduino.cc/en/software)
   - Installeer ESP8266 board package
   - Minimum versie: 1.8.19

2. **Benodigde Bibliotheken**:
   - Wire (ingebouwd)
   - LiquidCrystal_I2C
   - DHT sensor library
   - OneWire
   - DallasTemperature
   - ESP8266WiFi
   - Firebase ESP Client
   - NTPClient
   - WiFiUdp

## 🔥 Firebase Setup

### 1. Project Aanmaken
1. Ga naar [Firebase Console](https://console.firebase.google.com/)
2. Klik op "Project toevoegen"
3. Geef uw project een naam (bijv. "terrarium-controller")
4. Accepteer de voorwaarden
5. Klik op "Doorgaan"

### 2. Realtime Database Instellen
1. In het linker menu, klik op "Realtime Database"
2. Klik op "Database aanmaken"
3. Kies "Beginnen in testmodus"
4. Selecteer een locatie dicht bij Nederland (bijv. eur3)
5. Klik op "Inschakelen"

### 3. Project Instellingen Verkrijgen
1. Klik op het tandwiel (⚙️) rechtsboven
2. Selecteer "Projectinstellingen"
3. Ga naar "Service accounts"
4. Scroll naar "Database geheimen"
5. Kopieer de database URL
6. Kopieer de API-sleutel

### 4. Database Regels Instellen
```json
{
  "rules": {
    "terrarium_data": {
      ".read": true,
      ".write": true,
      "$mode": {
        ".read": true,
        ".write": true
      }
    }
  }
}
```

## 🔧 Installatie

1. **Hardware Aansluiten**:
   ```
   ESP8266 Pinout:
   - D3: DHT22 Links
   - D4: DHT22 Rechts
   - D5: DS18B20 Links
   - D6: DS18B20 Rechts
   - D7: Relais Links
   - D8: Relais Rechts
   - D9-D11: RGB LED 1
   - D12-D14: RGB LED 2
   - D0: Mode knop
   ```

2. **Code Configuratie**:
   - Open `esp8266_terrarium_controller.ino`
   - Vul uw WiFi credentials in:
     ```cpp
     #define WIFI_SSID "UW_WIFI_NAAM"
     #define WIFI_PASSWORD "UW_WIFI_WACHTWOORD"
     ```
   - Vul Firebase credentials in:
     ```cpp
     #define API_KEY "UW_FIREBASE_API_KEY"
     #define DATABASE_URL "UW_FIREBASE_DATABASE_URL"
     ```

3. **Code Uploaden**:
   - Selecteer juiste board in Arduino IDE
   - Selecteer correcte COM poort
   - Upload de code

## ⚙️ Configuratie

### Temperatuur Instellingen
```cpp
// Mode 1 (Dag)
Links: 24-27°C
Rechts: 30-32°C

// Mode 2 (Nacht)
Links: 22-24°C
Rechts: 27-30°C

// Mode 3 (Vervelling)
Links: 24-27°C
Rechts: 30-32°C
```

### Firebase Data Structuur
```
terrarium_data/
  ├── Day/
  │   └── timestamp1/
  │       ├── mode: "Day"
  │       ├── tempL_DHT: value
  │       ├── tempR_DHT: value
  │       └── ...
  ├── Night/
  └── Shedding/
```

## 🌡️ Gebruiksmodi

1. **Dag Modus (1)**:
   - Normale dagtemperaturen
   - Luchtvochtigheid: 50-60%

2. **Nacht Modus (2)**:
   - Verlaagde temperaturen
   - Luchtvochtigheid: 50-60%

3. **Vervelling Modus (3)**:
   - Normale temperaturen
   - Verhoogde luchtvochtigheid: 70-80%

## 🔍 Probleemoplossing

### WiFi Problemen
- Controleer WiFi credentials
- Controleer WiFi signaalsterkte
- ESP8266 moet binnen WiFi bereik zijn

### Firebase Problemen
- Controleer API key en Database URL
- Controleer internetverbinding
- Bekijk serial monitor voor foutmeldingen

### Sensor Problemen
- Controleer bedrading
- Controleer pull-up weerstanden
- Controleer voedingsspanning

## 🔄 Updates

- v2.0.0: Firebase integratie toegevoegd
- v2.0.1: Verbeterde datastructuur
- v2.0.2: NTP tijdsynchronisatie

## 📄 Licentie

Dit project is gelicenseerd onder de MIT License.

## 🤝 Bijdragen

Bijdragen zijn welkom! Open een issue of pull request voor suggesties en verbeteringen.