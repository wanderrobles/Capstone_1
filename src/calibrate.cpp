#include <Arduino.h>
#include "config.h"

// ── Moisture Sensor Calibration ───────────────────────────────────────────────
// 1. Flash with:  pio run -e calibrate --target upload
// 2. Open serial monitor (115200 baud)
// 3. Hold sensor in AIR  → note the raw value  → set DRY_VALUE in sensors.h
// 4. Submerge sensor in WATER → note the raw value → set WET_VALUE in sensors.h
// 5. Reflash the main firmware: pio run -e nodemcu-32s --target upload
// ─────────────────────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    pinMode(MOIST_PWR_PIN, OUTPUT);
    digitalWrite(MOIST_PWR_PIN, LOW);
    analogSetAttenuation(ADC_11db);
    delay(500);
    Serial.println("\n=== Moisture Sensor Calibration ===");
    Serial.println("Hold sensor in AIR, then submerge in WATER.");
    Serial.println("Reading every second...\n");
}

void loop() {
    digitalWrite(MOIST_PWR_PIN, HIGH);
    delay(50);
    int raw = analogRead(MOIST_PIN);
    digitalWrite(MOIST_PWR_PIN, LOW);

    Serial.printf("Raw ADC: %4d   (current DRY=%d  WET=%d)\n",
                  raw, 2546, 1294);
    delay(1000);
}
