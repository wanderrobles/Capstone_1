#pragma once

// ── WiFi ──────────────────────────────────
#define WIFI_SSID             "Sebastian's S26 Ultra"
#define WIFI_PASSWORD         "Luxeterna99"
#define WIFI_CONNECT_TIMEOUT_MS  10000  // Max ms to wait for connection
#define WIFI_RETRY_DELAY_MS      5000   // Ms to wait before retrying

// ── Topics ────────────────────────────────
#define TOPIC_TEMP     "home/env/temperature"
#define TOPIC_HUMIDITY "home/env/humidity"
#define TOPIC_STATUS   "home/env/status"

// ── Hardware Pins ─────────────────────────
#define TEMP_PIN       4      // DS18B20 data → GPIO4
#define MOIST_PIN      32     // Soil moisture analog → GPIO32

// I2C LCD (SDA=GPIO21, SCL=GPIO22 — ESP32 defaults)
#define LCD_I2C_ADDR   0x27
#define LCD_COLS       16
#define LCD_ROWS       2

// ── Timing ────────────────────────────────
#define READ_INTERVAL_MS      1000      // 1 second between sensor reads
#define SAMPLE_INTERVAL_MS    5000  // 3 hours between HTTP sends

// NTP (Puerto Rico = UTC-4)
#define NTP_OFFSET_SEC   (-4 * 3600)

// Local Python Server
// Run `ifconfig` (Mac/Linux) to find your laptop's IP
#define SERVER_URL      "http://192.168.0.13:5001/data"
