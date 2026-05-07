## APP.PY
Dit is de servercode. Door deze code weet de server wat het moet doen met data dat de ESP naar het stuurt.
Deserver is ontwikkeld in PYthon met het Flask framework. Het ontvangt de scandata van de ESP en slaat het op in een MariaDB database aan de ahnd van een REST API beschikbaar voor de dashboard.

Flask maakt gebruik van routes, de server draait op http://10.20.10.24:8081 en de verschillende adressen waarop de server luistert:
- /upload: het eindpunt waar de ESP zijn data naartoe stuurt. De server controleert of de JSON geldig is en maakt een nieuwe scan_id aan wat een unieke nummer is voor elke scan. Loopt over elk netwerk in "netwerk" in de JSON. Slaat elk netwerk op in de database. Als de "Manual" waarde in de JSON nul is, berekent de server de locatie via fingerprinting (meer hierover in fingerpinting.py). Als het niet nul is slaat het de locatie over. Tenlaatste stuurt de server een bevestiging terug.
    - Wat er per netwerk opgeslagen wordt:
        Identificatie:    ssid, bssid
        Signaal:          rssi, antenna
    Kanaal:           primary_channel, secondary_channel, bandwidth
    Beveiliging:      auth_mode, pairwise_cipher, group_cipher
    WiFi standaarden: 11b, 11g, 11n, 11a, 11ac, 11ax
    Locatie:          x, y
    Tijd:             scan_time_start, scan_time_end

- /heatmap: Haalt alle opgeslagen scandata op en verstuurt het terug als JSON zodat het dashboard het kan gebruiken om de heatmap te tekenen.

- /fingerprint: Haalt alle referentiemetingen op. Dit zijn manuele metingen die gebruikt worden voor het fingerprinting program.

- /predict: Als je /predict/"scan_id" schrijft, toont hij de vermoedelijke locatie van het gekozen netwerk. Dit is alleen voor te testen.

- /dev
