#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "sensors.h"
#include "display.h"

sensor_reader sensors(MOIST_PIN, TEMP_PIN);
display_manager display;


void connectWiFi() {
    display.showMessage("Connecting...", WIFI_SSID);
    delay(2000);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        attempts++;
        if (attempts > 20) {
            display.showMessage("WiFi failed!", "Check config.h");
            Serial.println("WiFi connection failed");
            return;
        }
    }

    String ip = WiFi.localIP().toString();
    display.showMessage("Connected!", ip);
    Serial.println("Connected! IP: " + ip);
    delay(2000);           
}

void setup() {
    Serial.begin(115200);
    display.begin();
    display.setBrightness(100);
    connectWiFi(); 
}

void loop() {
    int moisture = sensors.readMoistValue();
    float temp   = sensors.readTemp();

    display.showReadings(moisture, temp);
    int new_temp = temp * 9/5 + 32.00;
    Serial.print("Temp: ");
    Serial.print(new_temp, 1);
    Serial.print(" F  |  Moisture: ");
    Serial.print(moisture);
    Serial.println("%");

    delay(READ_INTERVAL_MS);
}
