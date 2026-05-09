# Plant Environment Monitor

An ESP32-based system that reads soil moisture and temperature every 30 seconds, buffers readings with real timestamps (via NTP), and sends them in batches over WiFi to a Raspberry Pi server. The Pi stores all readings in a CSV file and serves a live web dashboard.

---

## Hardware

| Component | Part |
|---|---|
| Microcontroller | ESP32 NodeMCU-32S |
| Temperature sensor | DS18B20 (1-Wire, waterproof) |
| Soil moisture sensor | Capacitive analog module |
| Display | 16×2 LCD (HD44780 parallel) |
| Server | Raspberry Pi (any model with WiFi/Ethernet) |

---

## Wiring

### DS18B20 Temperature Sensor → ESP32
```
DS18B20 VCC  →  3.3 V
DS18B20 GND  →  GND
DS18B20 DATA →  GPIO 4      (+ 4.7 kΩ pull-up to 3.3 V)
```

### Soil Moisture Sensor → ESP32
```
Sensor VCC   →  3.3 V
Sensor GND   →  GND
Sensor AOUT  →  GPIO 34     (analog in)
```

### 16×2 LCD → ESP32 (parallel 4-bit mode)
```
LCD RS  →  GPIO 19      LCD EN  →  GPIO 23
LCD D4  →  GPIO 18      LCD D5  →  GPIO 17
LCD D6  →  GPIO 16      LCD D7  →  GPIO 15
LCD BL  →  GPIO 32      (PWM backlight, 80 % brightness)
LCD VCC →  5 V          LCD GND →  GND
LCD V0  →  10 kΩ pot    (contrast adjust)
```

---

## Repository Layout

```
Capstone Dev/
├── src/
│   ├── main.cpp          # Arduino entry point
│   ├── config.h          # All pins, WiFi creds, server URL, timings
│   ├── sensors.h/.cpp    # DS18B20 + soil moisture reader
│   ├── display.h         # LCD manager
│   └── wifi_manager.h/.cpp  # WiFi, NTP, HTTP batch sender
├── server/
│   ├── pi_server.py      # ← Raspberry Pi server (Flask)
│   └── server.py         # Legacy laptop server
├── platformio.ini
└── README.md
```

---

## Firmware Setup (ESP32)

### Prerequisites
- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)

### 1. Configure WiFi and server IP

Edit [src/config.h](src/config.h):

```cpp
#define WIFI_SSID      "YourNetwork"
#define WIFI_PASSWORD  "YourPassword"
#define SERVER_URL     "http://<raspberry-pi-ip>:5000/data"
```

Find your Pi's IP with `hostname -I` on the Pi, or check your router's DHCP table.

### 2. Flash

```bash
pio run --target upload
pio device monitor          # 115200 baud — watch live logs
```

### Timing defaults (`config.h`)

| Setting | Value | Meaning |
|---|---|---|
| `READ_INTERVAL_MS` | 30 000 ms | Sensor read + LCD update every 30 s |
| `SAMPLE_INTERVAL_MS` | 60 000 ms | HTTP batch sent every 60 s |

---

## Server Setup (Raspberry Pi)

### Prerequisites

```bash
sudo apt update && sudo apt install python3-pip -y
pip3 install flask
```

### Run manually

```bash
cd ~/Capstone\ Dev/server
python3 pi_server.py
```

The server:
- Listens on **port 5000**, all interfaces
- Saves data to `~/capstone_data/data.csv`
- Writes logs to `~/capstone_data/server.log`

### Run on boot (systemd)

```bash
sudo nano /etc/systemd/system/plant-monitor.service
```

Paste:

```ini
[Unit]
Description=Plant Monitor Server
After=network.target

[Service]
ExecStart=/usr/bin/python3 /home/pi/Capstone Dev/server/pi_server.py
WorkingDirectory=/home/pi/Capstone Dev/server
Restart=always
User=pi

[Install]
WantedBy=multi-user.target
```

Enable:

```bash
sudo systemctl daemon-reload
sudo systemctl enable plant-monitor
sudo systemctl start plant-monitor
sudo systemctl status plant-monitor
```

---

## API Endpoints

| Method | Path | Description |
|---|---|---|
| `POST` | `/data` | Receive readings from ESP32 (JSON array or object) |
| `GET` | `/` | Live HTML dashboard (auto-refreshes every 30 s) |
| `GET` | `/latest` | Most recent reading as JSON |
| `GET` | `/history?limit=N` | Last N readings as JSON (default 100) |
| `GET` | `/health` | Server health + CSV row count |

### Payload format (ESP32 → Pi)

```json
[
  { "timestamp": "2026-05-09 14:00:00", "temperature": 25.5, "soil_moisture": 63 },
  { "timestamp": "2026-05-09 14:00:30", "temperature": 25.6, "soil_moisture": 62 }
]
```

`temperature` is in **°C**. The server stores both °C and the converted °F value.

### CSV columns

```
timestamp, temperature_c, temperature_f, soil_moisture_pct
```

---

## Dashboard

Open `http://<raspberry-pi-ip>:5000` in any browser on the same network.

- Current temperature (°F and °C) and soil moisture cards
- Scrollable table of the last 50 readings
- Color-coded cells: orange = hot (>90 °F), blue = cold (<50 °F), green = moist (>60 %), yellow = dry (<20 %)
- Auto-refreshes every 30 seconds

---

## Calibration

The moisture sensor raw ADC range is set in [src/sensors.h](src/sensors.h):

```cpp
const int DRY_VALUE = 2546;   // raw ADC reading in air
const int WET_VALUE = 1294;   // raw ADC reading submerged in water
```

To recalibrate: read the ADC raw value from the serial monitor (`analogRead` result before `map()`) in both conditions and update these constants.

---

## Timezone

The NTP offset is set for **Puerto Rico (UTC−4)** in [src/config.h](src/config.h):

```cpp
#define NTP_OFFSET_SEC  (-4 * 3600)
```

Change to match your location (e.g., `-5 * 3600` for Eastern Standard Time).
