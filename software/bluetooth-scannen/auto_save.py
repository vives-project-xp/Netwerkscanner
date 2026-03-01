import serial
import json
from datetime import datetime

ser = serial.Serial('COM4', 115200)  # Pas poort aan

results = []

while True:
    line = ser.readline().decode().strip()
    if 'Naam:' in line:
        device = {'naam': line.split('Naam: ')[1]}
    elif 'Adres:' in line:
        device['adres'] = line.split('Adres: ')[1]
    elif 'RSSI:' in line:
        device['rssi'] = line.split('RSSI: ')[1]
        results.append(device)

# Opslaan als JSON
with open('scan_results.json', 'w') as f:
    json.dump(results, f, indent=2)