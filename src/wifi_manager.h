#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_system.h>
#include "config.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // GPIO2 on most ESP32 dev boards
#endif

class wifi_manager {
private:
    bool _isConnected;
    unsigned long _lastSendTime;

    void sendData(float temperature, int soil_moisture);

public:
    wifi_manager();
    void begin();
    void update(float temperature, int moisture);
};
