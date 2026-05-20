# APP.PY

Dit is de servercode van het project.

Via deze code weet de server wat hij moet doen met de data die de ESP verstuurt.

De server is ontwikkeld in Python met het Flask-framework.  
Hij ontvangt scandata van de ESP en slaat deze op in een MariaDB-database via een API die beschikbaar is voor het dashboard.

---

# FLASK ROUTES

Flask maakt gebruik van routes.

De server draait op:

```text
http://10.20.10.24:8081
```

## Beschikbare routes

---

## `/upload`

Dit is het eindpunt waar de ESP zijn data naartoe stuurt.

### Werking

- De server controleert of de ontvangen JSON geldig is
- Er wordt een nieuwe `scan_id` aangemaakt
  - dit is een uniek nummer voor elke scan
- De server loopt door elk netwerk in `"networks"` van de JSON
- Elk netwerk wordt opgeslagen in de database

### Locatiebepaling

Als de `Manual` waarde in de JSON gelijk is aan `0`:
- berekent de server automatisch de locatie via fingerprinting
- meer informatie hierover staat in `fingerprinting.py`

Als `Manual` niet gelijk is aan `0`:
- wordt de automatische locatiebepaling overgeslagen

Ten slotte stuurt de server een bevestiging terug naar de ESP.

---

## Opgeslagen gegevens per netwerk

| Categorie | Data |
|---|---|
| Identificatie | `ssid`, `bssid` |
| Signaal | `rssi`, `antenna` |
| Kanaal | `primary_channel`, `secondary_channel`, `bandwidth` |
| Beveiliging | `auth_mode`, `pairwise_cipher`, `group_cipher` |
| WiFi standaarden | `11b`, `11g`, `11n`, `11a`, `11ac`, `11ax` |
| Locatie | `x`, `y` |
| Tijd | `scan_time_start`, `scan_time_end` |

---

## `/heatmap`

Haalt alle opgeslagen scandata op en verstuurt deze terug als JSON.

Het dashboard gebruikt deze data om een heatmap te tekenen.

---

## `/fingerprint`

Haalt alle referentiemetingen op.

Dit zijn manuele metingen die gebruikt worden voor het fingerprinting-systeem.

---

## `/predict`

Wanneer je:

```text
/predict/"scan_id"
```

oproept, toont de server de vermoedelijke locatie van de gekozen scan.

Deze route wordt enkel gebruikt voor testen.

---

## `/dev`

Developerroute voor debugging en testen.