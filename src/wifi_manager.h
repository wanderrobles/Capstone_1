#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_system.h>
#include <time.h>
#include <vector>
#include "config.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // GPIO2 on most ESP32 dev boards
#endif

struct Reading {
    char timestamp[20];
    float temperature;
    int soil_moisture;
};

class wifi_manager {
private:
    bool _isConnected;
    bool _ntpSynced;
    unsigned long _lastSendTime;
    std::vector<Reading> _buffer;

    void syncNTP();
    bool getTimestamp(char* buf, size_t len);
    void sendBatch();

public:
    wifi_manager();
    void begin();
    void update(float temperature, int moisture);
};
