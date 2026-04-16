#pragma once

// ── WiFi ──────────────────────────────────
#define WIFI_SSID             "Angel (2)"
#define WIFI_PASSWORD         "dimelomami"
#define WIFI_CONNECT_TIMEOUT_MS  10000  // Max ms to wait for connection
#define WIFI_RETRY_DELAY_MS      5000   // Ms to wait before retrying

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
#define READ_INTERVAL_MS      10000     // 10 seconds between sensor reads
#define SAMPLE_INTERVAL_MS    10800000  // 3 hours between HTTP sends

// Local Python Server
// Run `ifconfig` (Mac/Linux) to find your laptop's IP
#define SERVER_URL      "http://10.11.3.255:5000/data"
