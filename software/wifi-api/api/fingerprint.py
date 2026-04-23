# deze code is om x en y te voorspellen op basis van de RSSI-waarden en bssid's als om nieuwe scans een locatie te geven.
import math
import os

import mysql.connector

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
    cursor.execute("SELECT bssid, rssi FROM networks WHERE scan_id = %s", (scan_id,))
    scan_nets = cursor.fetchall()
    scan_map = {row["bssid"]: row["rssi"] for row in scan_nets}

    # Haal alle handmatige punten op
    cursor.execute("SELECT id, x, y FROM scans WHERE manual = 1")
    manual_points = cursor.fetchall()

    best_point = None
    best_distance = float("inf")

    for point in manual_points:
        cursor.execute("SELECT bssid, rssi FROM networks WHERE scan_id = %s", (point["id"],))
        nets = cursor.fetchall()
        manual_map = {row["bssid"]: row["rssi"] for row in nets}

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
