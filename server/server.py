from flask import Flask, request, jsonify
import csv
import os
from datetime import datetime

app = Flask(__name__)

# Path where the CSV will be saved — synced to OneDrive automatically
CSV_PATH = "/Users/javigermosen/Library/CloudStorage/OneDrive-pupr.edu/Capstone/Data/data.csv"

# Write CSV header if file doesn't exist yet
if not os.path.exists(CSV_PATH):
    with open(CSV_PATH, mode="w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "temperature", "soil_moisture"])

@app.route("/data", methods=["POST"])
def receive_data():
    payload = request.get_json()

    if not payload:
        return jsonify({"error": "No JSON received"}), 400

    # Accept either a single reading {} or a batch [...]
    if isinstance(payload, list):
        readings = payload
    else:
        readings = [{
            "timestamp":    datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "temperature":  payload.get("temperature", ""),
            "soil_moisture": payload.get("soil_moisture", "")
        }]

    with open(CSV_PATH, mode="a", newline="") as f:
        writer = csv.writer(f)
        for r in readings:
            writer.writerow([r["timestamp"], r["temperature"], r["soil_moisture"]])
            print(f"[{r['timestamp']}] temperature={r['temperature']} soil_moisture={r['soil_moisture']}")

    return jsonify({"status": "ok", "count": len(readings)}), 200

if __name__ == "__main__":
    # Runs on all network interfaces so the ESP32 can reach it
    app.run(host="0.0.0.0", port=5000)
