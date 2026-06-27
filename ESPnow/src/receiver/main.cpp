//ESPnow receiver

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <ArduinoJson.h>    //für Datenbearbeitung als JSON-Objekt
#include <SD_MMC.h>

#define SDMMC_CLK_PIN 39
#define SDMMC_CMD_PIN 38
#define SDMMC_DATA0_PIN 40

struct Config{
    String ssid;
    String pass;
    String whitelist[20];
};

JsonDocument data;
Config config;

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

void loadConfig(){
    Serial.println("Loading config from SD card...");

    File configFile = SD_MMC.open("/config.json", "r");

    if (!configFile) {
        Serial.println("Failed to open config file");
        return;
    }

    DeserializationError error = deserializeJson(data, configFile);
    if (error){
        Serial.println(F("Failed to read file"));
        Serial.println(error.f_str());
    }

    config.ssid = data["ssid"].as<String>();
    config.pass = data["pass"].as<String>();

    for(int i=0; i<data["whitelist"].size(); i++){
        config.whitelist[i] = data["whitelist"][i].as<String>();
    }

    Serial.println("------------------------------");   
    Serial.println("Config loaded:");
    Serial.print("SSID: ");
    Serial.println(config.ssid);
    Serial.print("Password: ");
    Serial.println(config.pass);
    Serial.println("Whitelist:");
    for(int i=0; i<data["whitelist"].size(); i++){
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

void setup(){
    Serial.begin(115200);
    WiFi.mode(WIFI_STA); 
    delay(1000);                    // Verzögerung für serielle Verbindung
    showMacAddress();
    initSD();
    loadConfig();
}

void loop(){

    delay(1000);                    // Verzögerung für serielle Verbindung
}
