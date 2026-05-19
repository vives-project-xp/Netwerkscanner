# Fingerprinting

Deze code bepaalt de locatie van het apparaat op basis van RSSI- en BSSID-waarden.

Wanneer het apparaat RSSI- en BSSID-waarden scant, vergelijkt het systeem deze met alle opgeslagen referentiewaarden uit de database.

De referentiemeting die het meest overeenkomt met de gescande waarden wordt gebruikt als nieuwe positie van het apparaat.

---

# Hoofdfuncties

## `findLocation(scan_id)`

Vergelijkt een nieuwe scan met alle opgeslagen referentiescans in de database.

### Werking

- Haalt de BSSID- en RSSI-waarden op van de nieuwe scan
- Vergelijkt deze met alle referentiewaarden
- Berekent het verschil tussen de scans
- Slaat de resultaten op in de lijst `fingerprintScores`

---

## `findLowest(fingerprintScores)`

Zoekt de referentiescan met het kleinste verschil.

Deze scan wordt beschouwd als de meest waarschijnlijke locatie van het apparaat.

---

## `updateLocation(x, y, scan_id)`

Schrijft de gevonden coördinaten weg naar de `heatmap`-tabel in de database.