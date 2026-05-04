import os

from flask import Flask, request, jsonify
from flask_cors import CORS
import mysql.connector
import re

from fingerprint import update_scan_with_prediction,findLocation,updateLocation


previousScanId = 0
previousScanTimeStart = 0
previousScanTimeEnd = 0


app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": [
    "http://localhost:3000",
    "http://127.0.0.1:5500",
    "http://localhost"
]}})

# -----------------------------
# Helpers
# -----------------------------
def safe_text(value):
    if value is None:
        return ""
    if not isinstance(value, str):
        value = str(value)
    return re.sub(r'[^\x00-\x7F]+', '', value)


def get_db():
    return mysql.connector.connect(
        host="mariadb",
        user=os.getenv("MYSQL_USER"),
        password=os.getenv("MYSQL_PASSWORD"),
        database=os.getenv("MYSQL_DATABASE")
    )

def get_new_scan_id(cursor):
    cursor.execute("SELECT IFNULL(MAX(scan_id), 0) + 1 FROM heatmap")
    return cursor.fetchone()[0]

# -----------------------------
# POST: Upload → heatmap
# -----------------------------
@app.route('/upload', methods=['POST'])
def upload():
    data = request.get_json()

    if not data:
        return jsonify({"status": "error", "message": "Invalid JSON"}), 400

    try:
        db = get_db()
        cursor = db.cursor()

        device_id = safe_text(data.get("device_id"))
        scanTimeStart = data.get("scan_time_start", 0)
        scanTimeEnd = data.get("scan_time_end", 0)
        x = data.get("x", 0)
        y = data.get("y", 0)
        manual = data.get("manual", 0)

        if scanTimeStart == previousScanTimeStart and scanTimeEnd == previousScanTimeEnd:
            scan_id = previousScanId
        else:
            # Nieuwe scan_id voor deze scan
            scan_id = get_new_scan_id
            previousScanId = scan_id
            previousScanTimeStart = scanTimeStart
            previousScanTimeEnd = scanTimeEnd

        sql = """
            INSERT INTO heatmap (
                scan_id,
                device_id, scan_time_start, scan_time_end,
                x, y, manual,
                net_ssid, net_bssid,
                primary_channel, secondary_channel, net_rssi,
                auth_mode, pairwise_cipher, group_cipher, antenna,
                phy_modes_11b, phy_modes_11g, phy_modes_11n, phy_modes_11a,
                phy_modes_11ac, phy_modes_11ax, phy_modes_lr, wps,
                ftm_responder, ftm_initiator, bandwidth, vht_freq1, vht_freq2,
                he_ap_bss_color, he_ap_partial_bss_color,
                he_ap_bss_color_disabled, he_ap_bssid_index,
                reserved, country
            )
            VALUES (
                %s,
                %s, %s, %s,
                %s, %s, %s,
                %s, %s,
                %s, %s, %s,
                %s, %s, %s, %s,
                %s, %s, %s, %s,
                %s, %s, %s, %s,
                %s, %s, %s, %s, %s,
                %s, %s,
                %s, %s,
                %s, %s
            )
        """

        for net in data.get("networks", []):
            cursor.execute(sql, (
                scan_id,
                device_id, scanTimeStart, scanTimeEnd,
                x, y, manual,
                safe_text(net.get("ssid")),
                safe_text(net.get("bssid")),
                net.get("primary_channel", 0),
                net.get("secondary_channel", 0),
                net.get("rssi", 0),
                net.get("auth_mode", 0),
                net.get("pairwise_cipher", 0),
                net.get("group_cipher", 0),
                net.get("antenna", 0),
                net.get("phy_modes_11b", 0),
                net.get("phy_modes_11g", 0),
                net.get("phy_modes_11n", 0),
                net.get("phy_modes_11a", 0),
                net.get("phy_modes_11ac", 0),
                net.get("phy_modes_11ax", 0),
                net.get("phy_modes_lr", 0),
                net.get("wps", 0),
                net.get("ftm_responder", 0),
                net.get("ftm_initiator", 0),
                net.get("bandwidth", 0),
                net.get("vht_freq1", 0),
                net.get("vht_freq2", 0),
                net.get("he_ap_bss_color", 0),
                net.get("he_ap_partial_bss_color", 0),
                net.get("he_ap_bss_color_disabled", 0),
                net.get("he_ap_bssid_index", 0),
                net.get("reserved", 0),
                safe_text(net.get("country"))
            ))

        db.commit()

        # Fingerprinting alleen voor niet-manual scans
        if not manual:
            x,y = findLocation(scan_id)
            updateLocation(x,y,scan_id)

        return jsonify({"status": "success", "scan_id": scan_id,"x":x,"y":y}), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

# -----------------------------
# GET: Entire Heatmap table
# -----------------------------
@app.route('/heatmap', methods=['GET'])
def get_heatmap():
    try:
        db = get_db()
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT * FROM heatmap")
        return jsonify(cursor.fetchall()), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

# -----------------------------
# GET: Entire Fingerprint table
# -----------------------------
@app.route('/fingerprint', methods=['GET'])
def get_fingerprint():
    try:
        db = get_db()
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT * FROM fingerprint")
        return jsonify(cursor.fetchall()), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

# -----------------------------
# GET: Entire Dev table 
# -----------------------------
@app.route('/dev', methods=['GET'])
def get_dev():
    try:
        db = get_db()
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT * FROM dev")
        return jsonify(cursor.fetchall()), 200

    except Exception as e:
        if "doesn't exist" in str(e) and "dev" in str(e):
            return jsonify([]), 200
        return jsonify({"status": "error", "message": str(e)}), 500

# -----------------------------
# GET: Predict location
# -----------------------------
@app.route('/predict/<int:scan_id>', methods=['GET'])
def api_predict(scan_id):
    try:
        x,y= findLocation(scan_id)

        if x is None:
            return jsonify({"error": "No manual points found"}), 404

        return jsonify({
            "x": x,
            "y": y,
        }), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500
    
# -----------------------------
# Real Time
# -----------------------------
import time

@app.route('/time', methods=['GET'])
def get_time():
    return jsonify({
        "timestamp": int(time.time())
    }), 200

# -----------------------------
# Start server
# -----------------------------
if __name__ == "__main__":
    db = get_db()
    cursor = db.cursor()

    cursor.execute("SELECT IFNULL(MAX(scan_id), 0) FROM heatmap")
    previousScanId = cursor.fetchone()[0]

    if previousScanId > 0:
        cursor.execute("SELECT time_start, time_end FROM heatmap WHERE scan_id = %s", (previousScanId,))
        row = cursor.fetchone()
        previousScanTimeStart = row[0]
        previousScanTimeEnd = row[1]
    else:
        previousScanTimeStart = None
        previousScanTimeEnd = None

    app.run(host="0.0.0.0", port=80)