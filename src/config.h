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
#define READ_INTERVAL_MS      30000     // 30 seconds between sensor reads
#define SAMPLE_INTERVAL_MS    60000  // 3 hours between HTTP sends

// NTP (Puerto Rico = UTC-4)
#define NTP_OFFSET_SEC   (-4 * 3600)

// Local Python Server
// Run `ifconfig` (Mac/Linux) to find your laptop's IP
#define SERVER_URL      "http://127.0.0.1:5000/data"
