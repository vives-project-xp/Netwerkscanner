# Uitleg
Dit is de map met de Flask‑API en de MariaDB‑database.  
De API ontvangt Wi‑Fi scans van de ESP32, slaat ze op en bepaalt automatisch een locatie via Wi‑Fi fingerprinting.  
Je kan via de API ook alle opgeslagen scans, networks en dev‑data gewoon opvragen.

## Hoe het werkt
- **scans**: één rij per scanmoment (incl. x,y locatie).
- **networks**: alle gevonden netwerken per scan (fingerprint‑data).
- **dev**: extra tabel met alle data samen voor eenvoudig uitlezen.
- Fingerprinting vergelijkt nieuwe scans met manual‑scans en vult automatisch x,y in.

## Docker
Je kan de containers starten:
- in deze map: `docker compose up`
- of vanuit de root van het project
