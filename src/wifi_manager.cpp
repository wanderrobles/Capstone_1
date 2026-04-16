#include "wifi_manager.h"

wifi_manager::wifi_manager() : _isConnected(false), _lastSendTime(0) {}

void wifi_manager::begin() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.printf("[Boot] Reset reason: %d\n", esp_reset_reason());
    Serial.println("[WiFi] Starting...");
    Serial.printf("[WiFi] Connecting to: %s\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void wifi_manager::sendData(float temperature, int soil_moisture) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    String body = "{\"temperature\":" + String(temperature) +
                  ",\"soil_moisture\":" + String(soil_moisture) + "}";

    int responseCode = http.POST(body);

    if (responseCode == 200) {
        Serial.printf("[HTTP] Data sent — temp=%.1f soil=%d\n", temperature, soil_moisture);
    } else {
        Serial.printf("[HTTP] POST failed, response code: %d\n", responseCode);
    }

    http.end();
}

void wifi_manager::update(float temperature, int moisture) {
    // Fires once on transition: disconnected -> connected
    if (WiFi.status() == WL_CONNECTED && !_isConnected) {
        _isConnected = true;
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("[WiFi] Connected!");
        Serial.printf("[WiFi] SSID: %s\n", WIFI_SSID);
        Serial.printf("[WiFi] IP Address: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("[WiFi] Signal (RSSI): %d dBm\n", WiFi.RSSI());
    }

    // Send on first connection and every SAMPLE_INTERVAL_MS (3 hours)
    if (_isConnected && (_lastSendTime == 0 || millis() - _lastSendTime >= SAMPLE_INTERVAL_MS)) {
        _lastSendTime = millis();
        sendData(temperature, moisture);
    }

    // Handle disconnection or waiting for initial connection
    if (WiFi.status() != WL_CONNECTED) {
        if (_isConnected) {
            Serial.println("[WiFi] Connection lost. Reconnecting...");
        }
        _isConnected = false;
        Serial.print(".");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Blink while connecting
        delay(500);
    }
}
