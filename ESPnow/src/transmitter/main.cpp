//ESPnow transmitter

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <esp_now.h>
#include <ArduinoJson.h>    //für Datenbearbeitung als JSON-Objekt

JsonDocument data;

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
    Serial.begin(9600);
    WiFi.mode(WIFI_STA); 
    delay(1000);     // Verzögerung für serielle Verbindung
    showMacAddress();
}

void loop(){

    delay(1000);

}
