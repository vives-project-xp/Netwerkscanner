# Dit is een Flask API voor het beheren van WiFi-scans en opslaan van waardes in een MariaDB database.
# Het biedt endpoints voor het uploaden van scans, ophalen van scans en netwerken.
# Het Importeert ook de fingerprinting code om nieuwe scans een voorspelde locatie in de database te steken.
from flask_cors import CORS
import mysql.connector
import re

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": [
    "http://localhost:3000",
    "http://127.0.0.1:5500",
    "http://localhost"
]}})



# Helper: verwijder corrupte UTF-8
def safe_text(value):
    if value is None:
        return ""
    if not isinstance(value, str):
        value = str(value)
    return re.sub(r'[^\x00-\x7F]+', '', value)


# -----------------------------
# POST: Upload scan + networks
# -----------------------------
@app.route('/upload', methods=['POST'])
def upload():
    data = request.get_json()

    if not data:
        return jsonify({"status": "error", "message": "Invalid JSON"}), 400

    try:
        db = mysql.connector.connect(
            host="mariadb",
            user="student",
            password="student",
            database="scan_resultaten"
        )
        cursor = db.cursor()

        # -----------------------------
        # Zoek sterkste netwerk
        # -----------------------------
        strongest = None
        for net in data.get("networks", []):
            if strongest is None or net.get("rssi", -999) > strongest.get("rssi", -999):
                strongest = net

        if strongest is None:
            strongest = {}

        # -----------------------------
        # Insert in scans
        # -----------------------------
        sql_scan = """
            INSERT INTO scans (
                device_id, scan_time_start, scan_time_end, x, y, rssi,
                ssid, bssid, primary_channel, secondary_channel,
                auth_mode, pairwise_cipher, group_cipher, antenna,
                phy_modes_11b, phy_modes_11g, phy_modes_11n, phy_modes_11a,
                phy_modes_11ac, phy_modes_11ax, phy_modes_lr, wps,
                ftm_responder, ftm_initiator, bandwidth, vht_freq1, vht_freq2,
                he_ap_bss_color, he_ap_partial_bss_color,
                he_ap_bss_color_disabled, he_ap_bssid_index, reserved, country
            )
            VALUES (%s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s)
        """

        cursor.execute(sql_scan, (
            safe_text(data.get("device_id")),
            data.get("scan_time_start", 0),
            data.get("scan_time_end", 0),
            data.get("x", 0),
            data.get("y", 0),
            strongest.get("rssi"),

            safe_text(strongest.get("ssid")),
            safe_text(strongest.get("bssid")),
            strongest.get("primary_channel", 0),
            strongest.get("secondary_channel", 0),
            strongest.get("auth_mode", 0),
            strongest.get("pairwise_cipher", 0),
            strongest.get("group_cipher", 0),
            strongest.get("antenna", 0),
            strongest.get("phy_modes_11b", 0),
            strongest.get("phy_modes_11g", 0),
            strongest.get("phy_modes_11n", 0),
            strongest.get("phy_modes_11a", 0),
            strongest.get("phy_modes_11ac", 0),
            strongest.get("phy_modes_11ax", 0),
            strongest.get("phy_modes_lr", 0),
            strongest.get("wps", 0),
            strongest.get("ftm_responder", 0),
            strongest.get("ftm_initiator", 0),
            strongest.get("bandwidth", 0),
            strongest.get("vht_freq1", 0),
            strongest.get("vht_freq2", 0),
            strongest.get("he_ap_bss_color", 0),
            strongest.get("he_ap_partial_bss_color", 0),
            strongest.get("he_ap_bss_color_disabled", 0),
            strongest.get("he_ap_bssid_index", 0),
            strongest.get("reserved", 0),
            safe_text(strongest.get("country"))
        ))

        scan_id = cursor.lastrowid

        # -----------------------------
        # Insert in networks
        # -----------------------------
        sql_network = """
            INSERT INTO networks (
                scan_id, ssid, bssid, primary_channel, secondary_channel, rssi,
                auth_mode, pairwise_cipher, group_cipher, antenna,
                phy_modes_11b, phy_modes_11g, phy_modes_11n, phy_modes_11a,
                phy_modes_11ac, phy_modes_11ax, phy_modes_lr, wps,
                ftm_responder, ftm_initiator, bandwidth, vht_freq1, vht_freq2,
                he_ap_bss_color, he_ap_partial_bss_color,
                he_ap_bss_color_disabled, he_ap_bssid_index, reserved, country
            )
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
        """

        for net in data.get("networks", []):
            cursor.execute(sql_network, (
                scan_id,
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

        # -----------------------------
        # Het commit eerst de waardes
        # -----------------------------
        db.commit()

        # -----------------------------
        # Nu plaatst het de waardes van de fingerpint in de database
        # -----------------------------
        from fingerprint import update_scan_with_prediction
        update_scan_with_prediction(scan_id)

        return jsonify({"status": "success"}), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500


       



# -----------------------------
# GET: Heel de table scans
# -----------------------------
@app.route('/scans', methods=['GET'])
def get_scans():
    try:
        db = mysql.connector.connect(
            host="mariadb",
            user="student",
            password="student",
            database="scan_resultaten"
        )
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT * FROM scans")
        result = cursor.fetchall()

        return jsonify(result), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500



# -----------------------------
# GET: Eén scan + networks
# -----------------------------
@app.route('/scans/<int:scan_id>', methods=['GET'])
def get_scan(scan_id):
    try:
        db = mysql.connector.connect(
            host="mariadb",
            user="student",
            password="student",
            database="scan_resultaten"
        )
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT * FROM scans WHERE id = %s", (scan_id,))
        scan = cursor.fetchone()

        if not scan:
            return jsonify({"status": "error", "message": "Scan not found"}), 404

        cursor.execute("SELECT * FROM networks WHERE scan_id = %s", (scan_id,))
        scan["networks"] = cursor.fetchall()

        return jsonify(scan), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500



# -----------------------------
# GET: Alleen networks van scan
# -----------------------------
@app.route('/scans/<int:scan_id>/networks', methods=['GET'])
def get_networks(scan_id):
    try:
        db = mysql.connector.connect(
            host="mariadb",
            user="student",
            password="student",
            database="scan_resultaten"
        )
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT * FROM networks WHERE scan_id = %s", (scan_id,))
        result = cursor.fetchall()

        return jsonify(result), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500



# -----------------------------
# GET: Alle x,y coördinaten
# -----------------------------
@app.route('/coordinates', methods=['GET'])
def get_coordinates():
    try:
        db = mysql.connector.connect(
            host="mariadb",
            user="student",
            password="student",
            database="scan_resultaten"
        )
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT id, x, y, rssi FROM scans")
        result = cursor.fetchall()

        return jsonify(result), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500



# -----------------------------
# GET: Laatste scan
# -----------------------------
@app.route('/last', methods=['GET'])
def get_last_scan():
    try:
        db = mysql.connector.connect(
            host="mariadb",
            user="student",
            password="student",
            database="scan_resultaten"
        )
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT * FROM scans ORDER BY id DESC LIMIT 1")
        scan = cursor.fetchone()

        if not scan:
            return jsonify({"status": "error", "message": "No scans found"}), 404

        cursor.execute("SELECT * FROM networks WHERE scan_id = %s", (scan["id"],))
        scan["networks"] = cursor.fetchall()

        return jsonify(scan), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500



# -----------------------------
# GET: Alle devices
# -----------------------------
@app.route('/devices', methods=['GET'])
def get_devices():
    try:
        db = mysql.connector.connect(
            host="mariadb",
            user="student",
            password="student",
            database="scan_resultaten"
        )
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT DISTINCT device_id FROM scans")
        result = cursor.fetchall()

        return jsonify(result), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

# -----------------------------
# GET: Alles uit dev tabel
# -----------------------------
@app.route('/dev', methods=['GET'])
def get_dev():
    try:
        db = mysql.connector.connect(
            host="mariadb",
            user="student",
            password="student",
            database="scan_resultaten"
        )
        cursor = db.cursor(dictionary=True)

        cursor.execute("SELECT * FROM dev")
        result = cursor.fetchall()

        return jsonify(result), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

# -----------------------------
# get id en fingerprinten
# -----------------------------

from fingerprint import predict_location

@app.route('/predict/<int:scan_id>', methods=['GET'])
def api_predict(scan_id):
    try:
        point, distance = predict_location(scan_id)

        if point is None:
            return jsonify({"error": "No manual points found"}), 404

        return jsonify({
            "x": point["x"],
            "y": point["y"],
            "distance": distance
        }), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500


# -----------------------------
# Start server
# -----------------------------
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80)
