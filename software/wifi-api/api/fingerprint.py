# deze code is om x en y te voorspellen op basis van de RSSI-waarden en bssid's als om nieuwe scans een locatie te geven.
import math
import os

import mysql.connector
#terminal van container
#docker exec -it wifi_api /bin/bash 
#hermaken van de container als je een aanpassing hebt gemaakt
#docker compose up -d --build wifi_api 
#docker logs wifi_api

# Functie om verbinding te maken met de database
def get_db():
    return mysql.connector.connect(
        host="mariadb",
        user=os.getenv("MYSQL_USER"),
        password=os.getenv("MYSQL_PASSWORD"),
        database=os.getenv("MYSQL_DATABASE")
    )
    
#data komt binnen
#kijken of de positie 0,0 is
#nee-> gewoon opslaan
#ja-> de positie moet brekend worden met fingerprinting.

#fingerprinting()
#vergelijk scan met met fingerprinting database
#tel de som van de delta rssi waarden op.
#enkel voor overeenkomstige bssid.
#elke locatie in fingerprinting database heeft een som variable
#zoek uit de lijst van som de kleinste 
#is het verschil groter dan x -> markeren als komt niet overeen
#(punt komt met niets overeen in de fingerprinting database)

#Tables in DB: [('bluetooth',), ('dev',), ('fingerprint',), ('heatmap',)]
#id, scan_id, device_id, scan_time_start, scan_time_end, x, y, manual, net_ssid, net_bssid, primary_channel, secondary_channel, net_rssi, auth_mode, pairwise_cipher, group_cipher, antenna, phy_modes_11b, phy_modes_11g, phy_modes_11n, phy_modes_11a, phy_modes_11ac, phy_modes_11ax, phy_modes_lr, wps, ftm_responder, ftm_initiator, bandwidth, vht_freq1, vht_freq2, he_ap_bss_color, he_ap_partial_bss_color, he_ap_bss_color_disabled, he_ap_bssid_index, reserved, country
#http://10.20.10.24:8081/fingerprint

def findLowest(fingerprintScores):
    if not fingerprintScores:
        return None
    lowest_item = fingerprintScores[0]
    id = None
    for entry in fingerprintScores:
        if entry['differenceAverage'] < lowest_item['differenceAverage']:
            lowest_item = entry
    print("lowest: "+str(lowest_item['scan_id']))
    print("differenceAverage: "+str(lowest_item['differenceAverage']))
    return lowest_item['scan_id']

def updateLocation(x,y,scan_id):
    try:
        db = get_db()
        cursor = db.cursor()
        query = """
                UPDATE heatmap 
                SET x = %s, y = %s 
                WHERE scan_id = %s
            """
        data = (x, y, scan_id)
        cursor.execute(query, data)
        db.commit()
        print(f"Successfully updated scan_id {scan_id} to coordinates ({x}, {y})")
        return True
    except Exception as e:
        db.rollback()
        print(f"Error updating database: {e}")
        return False

def findLocation(scan_id):#returns x,y
    db = get_db()
    cursor = db.cursor()

    cursor.execute("SELECT net_bssid, net_rssi FROM heatmap WHERE scan_id = %s", (scan_id,))
    scanDict = dict(cursor.fetchall())

    cursor.execute("SELECT DISTINCT scan_id FROM fingerprint WHERE manual = 1")
    bestaandeScan_ids = [row[0] for row in cursor.fetchall()]

    fingerprintScores = []

    #overloop alle posities in fingerprint table
    for i in bestaandeScan_ids:
        print("loop fingerprint table. scan_id="+str(i))

        differenceSum = 0
        matches = 0
        #overloop alle netwerken op deze locatie
        query = """
            SELECT net_bssid, net_rssi 
            FROM fingerprint 
            WHERE scan_id = %s AND manual = 1
        """
        
        cursor.execute(query, (i,))
        scanFingerprintDict = dict(cursor.fetchall())
        for bssidFP, rssiFP in scanFingerprintDict.items():
            if bssidFP in scanDict:
                rssiScan = scanDict[bssidFP]
                differenceSum += abs(rssiFP - rssiScan)
                matches +=1
        if matches > 0:
        # We voegen een dictionary toe aan de array
            fingerprintScores.append({
                'scan_id': i,
                'differenceAverage': differenceSum/matches
            })


    print(fingerprintScores)
    location = findLowest(fingerprintScores)
    print(location)

    query = """
            SELECT x, y 
            FROM fingerprint 
            WHERE scan_id = %s AND manual = 1
        """
    cursor.execute(query, (location,))
    results = cursor.fetchone()
    x = results[0]
    y = results[1]
    print(f"Coordinates: {x}, {y}")




    db.close()
    return x,y

#x,y =findLocation(10)
#updateLocation(x,y,1)































#!!!!!!!!!!!!!!app.py heeft dit voorlopig nog nodig!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Bereken de afstand tussen gescande netwerken en handmatige netwerken op basis van RSSI-waarden
def rssi_distance(scan_networks, manual_networks):
    dist = 0
    for bssid, rssi in scan_networks.items():
        if bssid in manual_networks:
            dist += (rssi - manual_networks[bssid]) ** 2
        else:
            dist += 100  # penalty voor ontbrekende AP
    return math.sqrt(dist)

# Voorspel de locatie door de dichtstbijzijnde handmatige scan te vinden
def predict_location(scan_id):
    db = get_db()
    cursor = db.cursor(dictionary=True)

    # Haal netwerken van de nieuwe scan op
    cursor.execute("SELECT net_bssid, rssi FROM networks WHERE scan_id = %s", (scan_id,))
    scan_nets = cursor.fetchall()
    scan_map = {row["net_bssid"]: row["rssi"] for row in scan_nets}

    # Haal alle handmatige punten op
    cursor.execute("SELECT id, x, y FROM scans WHERE manual = 1")
    manual_points = cursor.fetchall()

    best_point = None
    best_distance = float("inf")

    for point in manual_points:
        cursor.execute("SELECT net_bssid, rssi FROM networks WHERE scan_id = %s", (point["id"],))
        nets = cursor.fetchall()
        manual_map = {row["net_bssid"]: row["rssi"] for row in nets}

        d = rssi_distance(scan_map, manual_map)

        if d < best_distance:
            best_distance = d
            best_point = point

    return best_point, best_distance
    
# Update de scan met de voorspelde locatie
def update_scan_with_prediction(scan_id):
    point, distance = predict_location(scan_id)

    if point is None:
        return False  # geen manual punten

    db = get_db()
    cursor = db.cursor()

    cursor.execute("""
        UPDATE scans
        SET x = %s, y = %s
        WHERE id = %s
    """, (point["x"], point["y"], scan_id))

    db.commit()
    return True
print("hello van fingerprint.py")