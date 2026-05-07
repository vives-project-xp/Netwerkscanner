# Fingerprinting
De code bepaalt de locatie van het apparaat op basis van RSSI en BSSID waarden.
Wanneer we het apparaat RSSI en BSSID waarden scant vergelijkt het met alle opgeslagen referentiewaarden uit de database. De referentiewaarde dat het meest overeenkomt de gescande waarden wordt onze nieuwe positie.

Deze code is gebaseerd op 3 hoofdfuncties:
-findLocation(scan_id): Vergelijkt een nieuwe scan met alle opgeslagen referentiescans in de database. Het start met de BSSID en RSSI waarden te halen van de nieuwe scan. Het berekent het verschil met alle referentiewaarden en zet het in de lijst fingerprintScores.

-findLowest(fingerprintScores): Het zoekt de referentiescan met het kleinste verschil, dat is dan waarschijn lijke locatie.

-updateLocation(x, y, scan_id): schrijft de gevonden coördinaten naar de heatmap-tabel in de database.
