#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Firebase configuratie
#define API_KEY "UW_FIREBASE_API_KEY"
#define DATABASE_URL "UW_FIREBASE_DATABASE_URL"

// WiFi configuratie
#define WIFI_SSID "UW_WIFI_NAAM"
#define WIFI_PASSWORD "UW_WIFI_WACHTWOORD"

// Firebase objecten
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// NTP configuratie voor tijdsynchronisatie
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Variabele voor laatste upload tijd
unsigned long lastUploadTime = 0;
const unsigned long uploadInterval = 60000; // Upload elke 1 minuut

// LCD setup (2004 I2C LCD)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// DHT22 setup
#define DHTPIN_L D3
#define DHTPIN_R D4
#define DHTTYPE DHT22
DHT dhtL(DHTPIN_L, DHTTYPE);
DHT dhtR(DHTPIN_R, DHTTYPE);

// Dallas DS18B20 setup
#define ONE_WIRE_BUS_L D5
#define ONE_WIRE_BUS_R D6
OneWire oneWireL(ONE_WIRE_BUS_L);
OneWire oneWireR(ONE_WIRE_BUS_R);
DallasTemperature sensorsL(&oneWireL);
DallasTemperature sensorsR(&oneWireR);

// Relay module
#define RELAY_L D7
#define RELAY_R D8

// RGB LED setup
#define LED1_R D9
#define LED1_G D10
#define LED1_B D11
#define LED2_R D12
#define LED2_G D13
#define LED2_B D14

// Push button for mode selection
#define BUTTON_PIN D0
int mode = 1;

void setup() {
    Serial.begin(115200);
    
    // WiFi verbinding
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Verbinding maken met WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println("\nVerbonden met WiFi");

    // Firebase configuratie
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // Start NTP Client
    timeClient.begin();
    timeClient.setTimeOffset(3600); // Tijdzone instellen (UTC+1 voor Nederland)
    
    dhtL.begin();
    dhtR.begin();
    sensorsL.begin();
    sensorsR.begin();
    
    pinMode(RELAY_L, OUTPUT);
    pinMode(RELAY_R, OUTPUT);
    digitalWrite(RELAY_L, LOW);
    digitalWrite(RELAY_R, LOW);
    
    pinMode(LED1_R, OUTPUT);
    pinMode(LED1_G, OUTPUT);
    pinMode(LED1_B, OUTPUT);
    pinMode(LED2_R, OUTPUT);
    pinMode(LED2_G, OUTPUT);
    pinMode(LED2_B, OUTPUT);
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("L");
    lcd.setCursor(12, 0);
    lcd.print("R");
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        mode = (mode % 3) + 1;
        delay(500);
    }
    
    float humidityL = dhtL.readHumidity();
    float tempL_DHT = dhtL.readTemperature();
    float humidityR = dhtR.readHumidity();
    float tempR_DHT = dhtR.readTemperature();
    
    sensorsL.requestTemperatures();
    sensorsR.requestTemperatures();
    float tempL_DS = sensorsL.getTempCByIndex(0);
    float tempR_DS = sensorsR.getTempCByIndex(0);
    
    updateLCD(tempL_DHT, tempR_DHT, tempL_DS, tempR_DS, humidityL, humidityR);
    
    // Upload data naar Firebase elke minuut
    if (millis() - lastUploadTime >= uploadInterval) {
        uploadToFirebase(tempL_DHT, tempR_DHT, tempL_DS, tempR_DS, humidityL, humidityR);
        lastUploadTime = millis();
    }
    
    updateTemperatureControl(tempL_DS, tempR_DS);
    
    delay(2000);
}

void updateLCD(float tempL_DHT, float tempR_DHT, float tempL_DS, float tempR_DS, float humL, float humR) {
    lcd.setCursor(0, 1);
    lcd.print("DHT Temp L: "); lcd.print(tempL_DHT); lcd.print("C ");
    lcd.setCursor(12, 1);
    lcd.print("DHT Temp R: "); lcd.print(tempR_DHT); lcd.print("C ");
    
    lcd.setCursor(0, 2);
    lcd.print("Dallas Temp L: "); lcd.print(tempL_DS); lcd.print("C ");
    lcd.setCursor(12, 2);
    lcd.print("Dallas Temp R: "); lcd.print(tempR_DS); lcd.print("C ");
    
    lcd.setCursor(0, 3);
    lcd.print("Humidity L: "); lcd.print(humL); lcd.print("% ");
    lcd.setCursor(12, 3);
    lcd.print("Humidity R: "); lcd.print(humR); lcd.print("% ");
}

void uploadToFirebase(float tempL_DHT, float tempR_DHT, float tempL_DS, float tempR_DS, float humL, float humR) {
    if (WiFi.status() != WL_CONNECTED) return;

    timeClient.update();
    String timestamp = String(timeClient.getEpochTime());
    String modePath = (mode == 1) ? "Day" : (mode == 2) ? "Night" : "Shedding";

    FirebaseJson json;
    json.set("timestamp", timestamp);
    json.set("mode", modePath);
    json.set("tempL_DHT", tempL_DHT);
    json.set("tempR_DHT", tempR_DHT);
    json.set("tempL_DS", tempL_DS);
    json.set("tempR_DS", tempR_DS);
    json.set("humidity_L", humL);
    json.set("humidity_R", humR);

    String path = "/terrarium_data/" + modePath + "/" + timestamp;
    
    if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
        Serial.println("Data succesvol geüpload naar Firebase");
    } else {
        Serial.println("Fout bij uploaden naar Firebase: " + fbdo.errorReason());
    }
}

void updateTemperatureControl(float tempL_DS, float tempR_DS) {
    switch (mode) {
        case 1: // Day mode
            lcd.setCursor(0, 3);
            lcd.print("Ideaal: 50-60%H");
            controlTemperature(tempL_DS, RELAY_L, LED1_R, LED1_G, LED1_B, 24, 27);
            controlTemperature(tempR_DS, RELAY_R, LED2_R, LED2_G, LED2_B, 30, 32);
            break;
        case 2: // Night mode
            lcd.setCursor(0, 3);
            lcd.print("Ideaal: 50-60%H");
            controlTemperature(tempL_DS, RELAY_L, LED1_R, LED1_G, LED1_B, 22, 24);
            controlTemperature(tempR_DS, RELAY_R, LED2_R, LED2_G, LED2_B, 27, 30);
            break;
        case 3: // Shedding mode
            lcd.setCursor(0, 3);
            lcd.print("Vervelling: 70-80%H");
            controlTemperature(tempL_DS, RELAY_L, LED1_R, LED1_G, LED1_B, 24, 27);
            controlTemperature(tempR_DS, RELAY_R, LED2_R, LED2_G, LED2_B, 30, 32);
            break;
    }
}

void controlTemperature(float temp, int relay, int ledR, int ledG, int ledB, int minTemp, int maxTemp) {
    if (temp >= minTemp && temp <= maxTemp) {
        setLEDColor(ledR, ledG, ledB, 0, 255, 0);
        digitalWrite(relay, LOW);
    } else if (temp < minTemp) {
        setLEDColor(ledR, ledG, ledB, 0, 0, 255);
        digitalWrite(relay, HIGH);
    } else {
        setLEDColor(ledR, ledG, ledB, 255, 0, 0);
        digitalWrite(relay, LOW);
    }
}

void setLEDColor(int r, int g, int b, int red, int green, int blue) {
    analogWrite(r, red);
    analogWrite(g, green);
    analogWrite(b, blue);
}