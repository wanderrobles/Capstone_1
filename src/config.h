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
//#define READ_INTERVAL_MS      60000      // 60 seconds between sensor reads
//#define SAMPLE_INTERVAL_MS    3600000   // 1 hour between HTTP sends
#define READ_INTERVAL_MS      30000
#define SAMPLE_INTERVAL_MS    60000

// NTP (Puerto Rico = UTC-4)
#define NTP_OFFSET_SEC   (-4 * 3600)

// Google Sheets Apps Script endpoint
#define SERVER_URL      "https://script.google.com/macros/s/AKfycbwk7F2_EtrKiodowwwKJaPa3esedsgn5UEnoQ22y5wa-yYVJvwa9Q_glU0rKv2jvuij/exec"
