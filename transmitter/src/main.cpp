//ESPnow transmitter

#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <espnow.h>
#include <ArduinoJson.h>    //für Datenbearbeitung als JSON-Objekt
#include <Adafruit_AHTX0.h> // Library for AHT20
#include <Adafruit_BMP280.h> // Library for BMP280

// Create sensor objects
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

sensors_event_t humidity, temp;

uint8_t receiverAddress[] = {0x20, 0x6E, 0xF1, 0xAA, 0xB6, 0x70};  // MAC-Adresse des Empfängers

JsonDocument jsonData; // JSON-Dokument für die zuletzt empfangenen Daten

String sensorValuesToSent; // String für die zu sendenden Sensordaten


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

void messageSent(uint8_t *macAddr, uint8_t status) {
    Serial.print("Send status: ");
    if(status == 0){
        Serial.println("Success");
    }
    else{
        Serial.println("Error");
    }
}

void setup(){
    
    Serial.begin(9600);
    WiFi.mode(WIFI_STA); 
    delay(1000);     
    showMacAddress();

    if (esp_now_init() != 0) {
        Serial.println("ESP-NOW init failed");
        return;
    }
    
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    uint8_t result = esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_COMBO, 0, NULL, 0);

    if(result != 0){
        Serial.println("Failed to add peer");
    }
    
    if (!aht.begin()) {
        Serial.println("Failed to initialize AHT20 sensor!");
        while (1);
        Serial.println("AHT20 sensor initialized.");
    }

    if (!bmp.begin(0x77)) { 
        Serial.println("Failed to initialize BMP280 sensor!");
        while (1);
        Serial.println("BMP280 sensor initialized.");
    }

    esp_now_register_send_cb(messageSent);  
}

void loop(){
    sensors_event_t humidity, temperature;
    aht.getEvent(&humidity, &temperature); // Get new readings

    jsonData["name"] = "Wetterstation"; 
    jsonData["temperature"] = temperature.temperature;
    jsonData["humidity"] = humidity.relative_humidity;
    jsonData["pressure"] = bmp.readPressure() / 100.0F; // Convert to hPa

    serializeJson(jsonData, sensorValuesToSent);

    esp_now_send(receiverAddress, (uint8_t *) &sensorValuesToSent, sizeof(sensorValuesToSent));
    delay(5000);

    Serial.println("Sent sensor data: " + sensorValuesToSent);
    // Print sensor data to Serial Monitor
    Serial.print("AHT20 - Temperature: ");
    Serial.print(jsonData["temperature"].as<float>());
    Serial.print(" °C, Humidity: ");
    Serial.print(jsonData["humidity"].as<float>());
    Serial.println(" %");

    Serial.print("BMP280 - Pressure: ");
    Serial.print(jsonData["pressure"].as<float>());
    Serial.println(" hPa");

}