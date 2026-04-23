# Uitleg
Dit is de map met de Flask‑API en de MariaDB‑database.  
De API ontvangt Wi‑Fi scans van de ESP32, slaat ze op en bepaalt automatisch een locatie via Wi‑Fi fingerprinting.  
Je kan via de API posten naar de database en waardes kunnen opvragen.

## Hoe het werkt
- **Fingerprint**: een table waar al onze manueele scans inzitten die we gebruiken voor locatie bepaling
- **heatmap**: alle gevonden netwerken per scan gaan in deze table.
- **dev**: een table die je alleen kan opvragen met data dat niet wordt verandert, wordt gebruikt mocht een table corrupt wordt dat we nog waardes kunnen opvragen.
- Fingerprinting vergelijkt nieuwe scans met manual‑scans en vult automatisch x,y in.

## Docker
Je kan de containers starten:
- vanuit de projectroot: `docker compose up`
