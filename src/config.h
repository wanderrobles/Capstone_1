#pragma once

// ── WiFi ──────────────────────────────────
#define WIFI_SSID      "ARRIS-D8C5"
#define WIFI_PASSWORD  "B05DD445D8C5"

// ── MQTT Broker ───────────────────────────
#define MQTT_BROKER    "192.168.1.50"
#define MQTT_PORT      1883
#define MQTT_USER      "capstone"
#define MQTT_PASS      "secret"
#define MQTT_CLIENT_ID "esp32-monitor-01"

// ── Topics ────────────────────────────────
#define TOPIC_TEMP     "home/env/temperature"
#define TOPIC_HUMIDITY "home/env/humidity"
#define TOPIC_STATUS   "home/env/status"

// ── Hardware Pins ─────────────────────────
#define TEMP_PIN       4      // DS18B20 data → GPIO4
#define MOIST_PIN      34     // Soil moisture analog → GPIO34

#define LCD_RS         19
#define LCD_EN         23
#define LCD_D4         18
#define LCD_D5         17
#define LCD_D6         16
#define LCD_D7         15
#define LCD_BL         32     // Backlight PWM → GPIO32
#define LCD_BL_CH      0      // PWM channel backlight

// ── Timing ────────────────────────────────
#define READ_INTERVAL_MS  10000  // 30 seconds
