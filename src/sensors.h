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

// FUNCTION DEFINITIONS

sensor_reader::sensor_reader(int moistPin, int tempPin)
    : _oneWire(tempPin), _tempSensor(&_oneWire) {
    _moistPin = moistPin;
    _tempPin = tempPin;
    pinMode(_moistPin, INPUT);
    _tempSensor.begin();
}

int sensor_reader::readMoistValue() {
    analogSetAttenuation(ADC_11db);
    int raw = analogRead(_moistPin);
    int moisture = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
    return constrain(moisture, 0, 100);
}

float sensor_reader::readTemp() {
    _tempSensor.requestTemperatures();
    return _tempSensor.getTempCByIndex(0);
}
