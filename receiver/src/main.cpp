//ESPnow receiver

#include <Arduino.h>        //Standard Arduino Bibliotheken
#include <WiFi.h>           //WLAN
#include <esp_now.h>        //ESPnow
#include <ArduinoJson.h>    //für Datenbearbeitung als JSON-Objekt
#include <SD_MMC.h>         //SD-Karte

/**
 * @brief Pinbelegung für SD-Kartenmodul
 * 
 */
#define SDMMC_CLK_PIN 39
#define SDMMC_CMD_PIN 38
#define SDMMC_DATA0_PIN 40

/**
 * @brief Struktur für die Konfiguration
 * 
 */
struct Config{
    String ssid;
    String pass;
    String whitelist[20];   //es können maximal 20 Geräte mit ESPnow verbunden werden
};

Config config;

/**
 * @brief JSON-Dokument für die Konfiguration
 * 
 */
JsonDocument configData;

/**
 * @brief JSON-Dokument für die zuletzt empfangenen Daten
 * 
 */
JsonDocument lastReceivedData;


/**
 * @brief Initialisiert die SD-Karte
 * 
 */
void initSD(){
    Serial.println("Initializing SD card...");
    if(!SD_MMC.setPins(SDMMC_CLK_PIN, SDMMC_CMD_PIN, SDMMC_DATA0_PIN)){
        Serial.println("SD_MMC pin configuration failed");
        return;
    }
    
    if(!SD_MMC.begin("/sdcard", true)){
        Serial.println("Card Mount Failed");
        return;
    }
}

/**
 * @brief Lädt die Konfiguration von der SD-Karte
 * 
 */
void loadConfig(){
    Serial.println("Loading config from SD card...");

    File configFile = SD_MMC.open("/config.json", "r");

    if (!configFile) {
        Serial.println("Failed to open config file");
        return;
    }

    DeserializationError error = deserializeJson(configData, configFile);
    if (error){
        Serial.println(F("Failed to read file"));
        Serial.println(error.f_str());
    }

    config.ssid = configData["ssid"].as<String>();
    config.pass = configData["pass"].as<String>();

    for(int i=0; i<configData["whitelist"].size(); i++){
        config.whitelist[i] = configData["whitelist"][i].as<String>();
    }

    Serial.println("------------------------------");   
    Serial.println("Config loaded:");
    Serial.print("SSID: ");
    Serial.println(config.ssid);
    Serial.print("Password: ");
    Serial.println(config.pass);
    Serial.println("Whitelist:");
    for(int i=0; i<configData["whitelist"].size(); i++){
        Serial.println(config.whitelist[i]);
    }
    Serial.println("------------------------------");
    
    configFile.close();
}

/** 
 * @brief Zeigt die MAC-Adresse des Geräts an
 */
void showMacAddress(){
    Serial.print("MAC-Address: ");
    String mac = WiFi.macAddress();
    Serial.println(mac);
    
    Serial.print("Formated: ");
    Serial.print("{");
    int index = 0;
    for(int i=0; i<6; i++){
        Serial.print("0x");
        Serial.print(mac.substring(index, index+2));
        if(i<5){
            Serial.print(", ");
        }
        index += 3;
    }
    Serial.println("}");
}

void messageReceived(const uint8_t* mac, const uint8_t* incomingData, int len) {
    //TODO: Sender überprüfen und nur Daten von Geräten in der Whitelist akzeptieren
    Serial.print("From: ");
    for (int i = 0; i < 6; i++) {
        if (i) Serial.print(":");
        if (mac[i] < 16) Serial.print("0");
        Serial.print(mac[i], HEX);
    }
    Serial.print(" | configData: ");
    for (int i = 0; i < len; i++) {
        Serial.print((char)incomingData[i]);
    }
    DeserializationError error = deserializeJson(lastReceivedData, incomingData, len);
    if (error) {
        Serial.println(F("Failed to parse incoming data"));
        Serial.println(error.f_str());
    }

    Serial.println();
}


void setup(){
    Serial.begin(115200);
    WiFi.mode(WIFI_STA); 
    delay(1000);                    // Verzögerung für serielle Verbindung
    showMacAddress();

    initSD();
    loadConfig();

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }
    esp_now_register_recv_cb(messageReceived);
}

void loop(){

}
