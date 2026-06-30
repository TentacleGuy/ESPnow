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


JsonDocument data;

uint8_t receiverAddress[] = {0x20, 0x6E, 0xF1, 0xAA, 0xB6, 0x70};  // MAC-Adresse des Empfängers

typedef struct data {
    char text[64];
    int intVal;
    float floatVal;
} messageToBeSent;

messageToBeSent data; 

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

    if (esp_now_init() != ESP_OK) {
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

    // Read data from AHT20
    aht.getEvent(&humidity, &temp);

    // Read data from BMP280
    float pressure = bmp.readPressure() / 100.0F; // Convert to hPa


    char textMsg[] = "Hi, here's my data for you: ";
    memcpy(&data.text, textMsg, sizeof(textMsg));

    esp_now_send(receiverAddress, (uint8_t *) &data, sizeof(data));
    delay(5000);

    // Print sensor data to Serial Monitor
    Serial.print("AHT20 - Temperature: ");
    Serial.print(temp.temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity.relative_humidity);
    Serial.println(" %");

    Serial.print("BMP280 - Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");

}