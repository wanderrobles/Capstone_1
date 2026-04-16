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

    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    temperature = payload.get("temperature", "")
    soil_moisture = payload.get("soil_moisture", "")

    # Append the reading to the CSV
    with open(CSV_PATH, mode="a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([timestamp, temperature, soil_moisture])

    print(f"[{timestamp}] temperature={temperature} soil_moisture={soil_moisture}")
    return jsonify({"status": "ok"}), 200

if __name__ == "__main__":
    # Runs on all network interfaces so the ESP32 can reach it
    app.run(host="0.0.0.0", port=5000)
