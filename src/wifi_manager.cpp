#include "wifi_manager.h"

wifi_manager::wifi_manager()
    : _isConnected(false), _ntpSynced(false), _lastSendTime(0), _hasReading(false) {}

void wifi_manager::begin() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.printf("[Boot] Reset reason: %d\n", esp_reset_reason());
    Serial.println("[WiFi] Starting...");
    Serial.printf("[WiFi] Connecting to: %s\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void wifi_manager::syncNTP() {
    configTime(NTP_OFFSET_SEC, 0, "pool.ntp.org");
    struct tm timeinfo;
    int retries = 0;
    while (!getLocalTime(&timeinfo) && retries < 10) {
        delay(500);
        retries++;
    }
    _ntpSynced = (retries < 10);
    if (_ntpSynced) Serial.println("[NTP] Time synced");
    else            Serial.println("[NTP] Sync failed — readings will not be buffered");
}

bool wifi_manager::getTimestamp(char* buf, size_t len) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return false;
    strftime(buf, len, "%Y-%m-%d %H:%M:%S", &timeinfo);
    return true;
}

void wifi_manager::sendReading() {
    if (!_hasReading) {
        Serial.println("[HTTP] No reading to send yet");
        return;
    }

    HTTPClient http;
    http.begin(SERVER_URL);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // required for Apps Script
    http.addHeader("Content-Type", "application/json");

    String body = "{\"timestamp\":\"" + String(_lastReading.timestamp) +
                  "\",\"temperature\":"  + String(_lastReading.temperature, 1) +
                  ",\"soil_moisture\":"  + String(_lastReading.soil_moisture) + "}";

    int responseCode = http.POST(body);

    if (responseCode == 200) {
        Serial.printf("[HTTP] Sent → %s\n", body.c_str());
    } else {
        Serial.printf("[HTTP] Failed, code: %d\n", responseCode);
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
        syncNTP();
    }

    // Always overwrite with the latest reading
    if (_isConnected && _ntpSynced) {
        if (getTimestamp(_lastReading.timestamp, sizeof(_lastReading.timestamp))) {
            _lastReading.temperature  = temperature;
            _lastReading.soil_moisture = moisture;
            _hasReading = true;
            Serial.printf("[Reading] Updated — %s  temp=%.1f  soil=%d\n",
                          _lastReading.timestamp, _lastReading.temperature, _lastReading.soil_moisture);
        }
    }

    // Send latest reading every SAMPLE_INTERVAL_MS
    if (_isConnected && (_lastSendTime == 0 || millis() - _lastSendTime >= SAMPLE_INTERVAL_MS)) {
        _lastSendTime = millis();
        sendReading();
    }

    // Handle disconnection
    if (WiFi.status() != WL_CONNECTED) {
        if (_isConnected) {
            Serial.println("[WiFi] Connection lost. Reconnecting...");
        }
        _isConnected = false;
        Serial.print(".");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(500);
    }
}
