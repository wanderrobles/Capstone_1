#include "sensors.h"

sensor_reader::sensor_reader(int moistPin, int pwrPin, int tempPin)
    : _oneWire(tempPin), _tempSensor(&_oneWire) {
    _moistPin = moistPin;
    _pwrPin   = pwrPin;
    _tempPin  = tempPin;
    pinMode(_moistPin, INPUT);
    pinMode(_pwrPin, OUTPUT);
    digitalWrite(_pwrPin, LOW);
    _tempSensor.begin();
}

int sensor_reader::readMoistValue() {
    digitalWrite(_pwrPin, HIGH);
    delay(50);                        // wait for sensor to stabilize after power-on
    analogSetAttenuation(ADC_11db);
    int raw      = analogRead(_moistPin);
    digitalWrite(_pwrPin, LOW);
    int moisture = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
    return constrain(moisture, 0, 100);
}

float sensor_reader::readTemp() {
    _tempSensor.requestTemperatures();
    return _tempSensor.getTempCByIndex(0);
}
