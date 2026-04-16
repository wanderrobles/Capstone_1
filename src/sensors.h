#pragma once
#include <stdint.h>
#include <esp32-hal-adc.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class sensor_reader {
private:
    const int DRY_VALUE = 2546;
    const int WET_VALUE = 1294;
    int _moistPin;
    int _tempPin;
    OneWire _oneWire;
    DallasTemperature _tempSensor;

public:
    sensor_reader(int moistPin, int tempPin);
    int readMoistValue();
    float readTemp();
};
