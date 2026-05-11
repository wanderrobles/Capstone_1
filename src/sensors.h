#pragma once
#include <stdint.h>
#include <esp32-hal-adc.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class sensor_reader {
private:
    const int DRY_VALUE = 1456;
    const int WET_VALUE = 860;
    int _moistPin;
    int _pwrPin;
    int _tempPin;
    OneWire _oneWire;
    DallasTemperature _tempSensor;

public:
    sensor_reader(int moistPin, int pwrPin, int tempPin);
    int readMoistValue();
    float readTemp();
};
