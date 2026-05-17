#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "display.h"
#include "wifi_manager.h"

sensor_reader sensors(MOIST_PIN, TEMP_PIN);
display_manager display;
wifi_manager wifi;

void setup() {
    Serial.begin(115200);
    delay(1000);
    display.begin();
    wifi.begin();
}

void loop() {
    int moisture = sensors.readMoistValue();
    float temp   = sensors.readTemp();

    display.showReadings(moisture, temp);

    int tempF = temp * 9 / 5 + 32;
    Serial.print("Temp: ");
    Serial.print(tempF);
    Serial.print(" F  |  Moisture: ");
    Serial.print(moisture);
    Serial.println("%");

    wifi.update(temp, moisture);

    delay(READ_INTERVAL_MS);
}
