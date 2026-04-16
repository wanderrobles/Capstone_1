#include "sensors.h"

sensor_reader::sensor_reader(int moistPin, int tempPin)
    : _oneWire(tempPin), _tempSensor(&_oneWire) {
    _moistPin = moistPin;
    _tempPin  = tempPin;
    pinMode(_moistPin, INPUT);
    _tempSensor.begin();
}

int sensor_reader::readMoistValue() {
    analogSetAttenuation(ADC_11db);
    int raw      = analogRead(_moistPin);
    int moisture = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
    return constrain(moisture, 0, 100);
}

float sensor_reader::readTemp() {
    _tempSensor.requestTemperatures();
    return _tempSensor.getTempCByIndex(0);
}
