#include "wifi_manager.h"

wifi_manager::wifi_manager()
    : _isConnected(false), _ntpSynced(false), _lastSendTime(0) {}

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

void wifi_manager::sendBatch() {
    if (_buffer.empty()) {
        Serial.println("[HTTP] Nothing to send yet");
        return;
    }

    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    String body = "[";
    for (size_t i = 0; i < _buffer.size(); i++) {
        if (i > 0) body += ",";
        body += "{\"timestamp\":\"" + String(_buffer[i].timestamp) +
                "\",\"temperature\":"   + String(_buffer[i].temperature, 1) +
                ",\"soil_moisture\":"   + String(_buffer[i].soil_moisture) + "}";
    }
    body += "]";

    int responseCode = http.POST(body);

    if (responseCode == 200) {
        Serial.printf("[HTTP] Batch sent — %d readings\n", _buffer.size());
        _buffer.clear();
    } else {
        Serial.printf("[HTTP] Batch failed, code: %d — will retry next cycle\n", responseCode);
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

    // Buffer each reading with a real timestamp
    if (_isConnected && _ntpSynced) {
        Reading r;
        if (getTimestamp(r.timestamp, sizeof(r.timestamp))) {
            r.temperature   = temperature;
            r.soil_moisture = moisture;
            _buffer.push_back(r);
            Serial.printf("[Buffer] Reading #%d stored — %s  temp=%.1f  soil=%d\n",
                          _buffer.size(), r.timestamp, r.temperature, r.soil_moisture);
        }
    }

    // Send batch every SAMPLE_INTERVAL_MS
    if (_isConnected && (_lastSendTime == 0 || millis() - _lastSendTime >= SAMPLE_INTERVAL_MS)) {
        _lastSendTime = millis();
        sendBatch();
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
