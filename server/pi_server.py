#!/usr/bin/env python3
"""
Raspberry Pi data receiver for the ESP32 soil/temperature monitoring system.

Receives JSON POST batches from the ESP32, persists them to CSV, and serves
a live dashboard at http://<pi-ip>:5000/
"""

import csv
import logging
import os
import subprocess
import threading
from datetime import datetime
from pathlib import Path

from flask import Flask, jsonify, render_template_string, request

# ── Configuration ─────────────────────────────────────────────────────────────
PORT       = 5000
HOST       = "0.0.0.0"
DATA_DIR   = Path.home() / "capstone_data"
CSV_PATH   = DATA_DIR / "data.csv"
LOG_PATH   = DATA_DIR / "server.log"
MAX_DASHBOARD_ROWS = 50   # rows shown in the browser table

# OneDrive sync via rclone — set ONEDRIVE_REMOTE to your rclone remote name
# and ONEDRIVE_DEST to the folder path inside OneDrive where the CSV should land.
# Example: remote named "onedrive", folder "Capstone" → file ends up at
#          OneDrive/Capstone/data.csv
# Set ONEDRIVE_REMOTE = "" to disable syncing.
ONEDRIVE_REMOTE = "onedrive"          # rclone remote name (from `rclone config`)
ONEDRIVE_DEST   = "Capstone"          # destination folder inside OneDrive
# ──────────────────────────────────────────────────────────────────────────────

DATA_DIR.mkdir(parents=True, exist_ok=True)

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    handlers=[
        logging.FileHandler(LOG_PATH),
        logging.StreamHandler(),
    ],
)
log = logging.getLogger(__name__)

if not CSV_PATH.exists():
    with open(CSV_PATH, "w", newline="") as f:
        csv.writer(f).writerow(["timestamp", "temperature_c", "temperature_f", "soil_moisture_pct"])
    log.info("Created %s", CSV_PATH)

app = Flask(__name__)

# ── HTML dashboard template ───────────────────────────────────────────────────
DASHBOARD_HTML = """
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="30">
  <title>Plant Monitor Dashboard</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family: system-ui, sans-serif; background: #0f172a; color: #e2e8f0; padding: 2rem; }
    h1   { font-size: 1.6rem; margin-bottom: 0.25rem; color: #38bdf8; }
    p.sub { font-size: 0.85rem; color: #64748b; margin-bottom: 2rem; }
    .cards { display: flex; gap: 1.5rem; flex-wrap: wrap; margin-bottom: 2.5rem; }
    .card  { background: #1e293b; border-radius: 12px; padding: 1.5rem 2rem; min-width: 180px; }
    .card .label { font-size: 0.75rem; text-transform: uppercase; letter-spacing: 0.08em; color: #94a3b8; }
    .card .value { font-size: 2.5rem; font-weight: 700; margin-top: 0.25rem; }
    .card .unit  { font-size: 1rem; color: #94a3b8; }
    .temp  .value { color: #f97316; }
    .moist .value { color: #34d399; }
    .time  .value { font-size: 1rem; color: #a78bfa; margin-top: 0.5rem; }
    table { width: 100%; border-collapse: collapse; background: #1e293b; border-radius: 12px; overflow: hidden; }
    th, td { padding: 0.65rem 1rem; text-align: left; font-size: 0.875rem; }
    th { background: #0f172a; color: #94a3b8; text-transform: uppercase; font-size: 0.7rem; letter-spacing: 0.08em; }
    tr:nth-child(even) { background: #162032; }
    td.hi  { color: #f97316; }
    td.low { color: #38bdf8; }
    td.mhi { color: #34d399; }
    td.mlo { color: #fbbf24; }
    caption { caption-side: top; padding: 1rem; font-weight: 600; color: #cbd5e1; text-align: left; }
  </style>
</head>
<body>
  <h1>🌱 Plant Environment Monitor</h1>
  <p class="sub">Auto-refreshes every 30 s &nbsp;·&nbsp; Showing last {{ rows|length }} readings</p>

  <div class="cards">
    {% if latest %}
    <div class="card temp">
      <div class="label">Temperature</div>
      <div class="value">{{ "%.1f"|format(latest.temperature_f) }}<span class="unit"> °F</span></div>
      <div class="unit">({{ "%.1f"|format(latest.temperature_c) }} °C)</div>
    </div>
    <div class="card moist">
      <div class="label">Soil Moisture</div>
      <div class="value">{{ latest.soil_moisture_pct }}<span class="unit"> %</span></div>
    </div>
    <div class="card time">
      <div class="label">Last Reading</div>
      <div class="value">{{ latest.timestamp }}</div>
    </div>
    {% else %}
    <div class="card"><div class="label">No data yet</div></div>
    {% endif %}
  </div>

  <table>
    <caption>Recent Readings</caption>
    <thead>
      <tr>
        <th>Timestamp</th>
        <th>Temp (°F)</th>
        <th>Temp (°C)</th>
        <th>Soil Moisture (%)</th>
      </tr>
    </thead>
    <tbody>
      {% for r in rows %}
      <tr>
        <td>{{ r.timestamp }}</td>
        <td class="{{ 'hi' if r.temperature_f|float > 90 else 'low' if r.temperature_f|float < 50 else '' }}">{{ "%.1f"|format(r.temperature_f|float) }}</td>
        <td>{{ "%.1f"|format(r.temperature_c|float) }}</td>
        <td class="{{ 'mhi' if r.soil_moisture_pct|int > 60 else 'mlo' if r.soil_moisture_pct|int < 20 else '' }}">{{ r.soil_moisture_pct }}</td>
      </tr>
      {% endfor %}
    </tbody>
  </table>
</body>
</html>
"""


def _sync_to_onedrive():
    """Copy the CSV to OneDrive in a background thread using rclone."""
    if not ONEDRIVE_REMOTE:
        return
    dest = f"{ONEDRIVE_REMOTE}:{ONEDRIVE_DEST}"
    try:
        result = subprocess.run(
            ["rclone", "copy", str(CSV_PATH), dest],
            capture_output=True, text=True, timeout=30
        )
        if result.returncode == 0:
            log.info("OneDrive sync OK → %s/%s", dest, CSV_PATH.name)
        else:
            log.warning("OneDrive sync failed: %s", result.stderr.strip())
    except FileNotFoundError:
        log.error("rclone not found — install it with: sudo apt install rclone")
    except subprocess.TimeoutExpired:
        log.warning("OneDrive sync timed out after 30 s")


def _read_csv_rows(limit: int | None = None) -> list[dict]:
    rows = []
    if not CSV_PATH.exists():
        return rows
    with open(CSV_PATH, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append(row)
    if limit:
        rows = rows[-limit:]
    rows.reverse()
    return rows


# ── Routes ────────────────────────────────────────────────────────────────────

@app.route("/data", methods=["POST"])
def receive_data():
    payload = request.get_json(silent=True)
    if payload is None:
        return jsonify({"error": "No JSON received"}), 400

    readings = payload if isinstance(payload, list) else [payload]

    rows_written = 0
    with open(CSV_PATH, "a", newline="") as f:
        writer = csv.writer(f)
        for r in readings:
            try:
                temp_c = float(r.get("temperature", 0))
                temp_f = round(temp_c * 9 / 5 + 32, 1)
                moisture = int(r.get("soil_moisture", 0))
                ts = r.get("timestamp") or datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                writer.writerow([ts, round(temp_c, 2), temp_f, moisture])
                log.info("Stored: %s  temp=%.1f°C / %.1f°F  soil=%d%%", ts, temp_c, temp_f, moisture)
                rows_written += 1
            except (ValueError, TypeError) as exc:
                log.warning("Skipped malformed reading %s: %s", r, exc)

    if rows_written:
        threading.Thread(target=_sync_to_onedrive, daemon=True).start()

    return jsonify({"status": "ok", "stored": rows_written}), 200


@app.route("/")
def dashboard():
    rows = _read_csv_rows(limit=MAX_DASHBOARD_ROWS)
    latest = rows[0] if rows else None
    return render_template_string(DASHBOARD_HTML, rows=rows, latest=latest)


@app.route("/latest")
def latest():
    rows = _read_csv_rows(limit=1)
    if not rows:
        return jsonify({"error": "no data yet"}), 404
    return jsonify(rows[0])


@app.route("/history")
def history():
    limit_param = request.args.get("limit", default=100, type=int)
    rows = _read_csv_rows(limit=limit_param)
    return jsonify(rows)


@app.route("/health")
def health():
    return jsonify({"status": "ok", "csv": str(CSV_PATH), "rows": len(_read_csv_rows())})


# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    log.info("Starting server on %s:%d", HOST, PORT)
    log.info("Data file: %s", CSV_PATH)
    app.run(host=HOST, port=PORT, debug=False)
