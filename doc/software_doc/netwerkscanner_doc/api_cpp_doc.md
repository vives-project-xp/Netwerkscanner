# API

Zet WiFi-scandata om naar JSON en verstuurt deze naar de server.

## Functies

---

### `PrintApiInfo()`

Print alle informatie van één WiFi-netwerk naar de console.

Deze functie wordt gebruikt om data te controleren tijdens debugging, maar heeft geen directe functionele rol binnen het systeem.

---

### `GetChipId()`

Geeft een unieke naam terug, bijvoorbeeld:

```text
ESP32-C5_FFE8
```

De naam bestaat uit:
- het chiptype
- de laatste 2 bytes van het MAC-adres

Hierdoor weet de server welk apparaat de data verstuurt.

---

### `CreateWiFiJson()`

Neemt een lijst van gescande netwerken en bouwt hiervan een JSON-string.

## Parameters

| Parameter | Beschrijving |
|---|---|
| `wifi_ap_record_t* aps` | lijst van gescande netwerken |
| `uint16_t start` | vanaf welk netwerk starten |
| `uint16_t count` | hoeveel netwerken meenemen |
| `uint64_t TimeStart` | wanneer de scan begon |
| `uint64_t TimeEnd` | wanneer de scan eindigde |
| `int32_t x, y` | locatie van de scan (standaard `0,0`) |

---

# JSON Resultaat

Het resultaat ziet er als volgt uit:

```json
{
  "device_id": "ESP32-C5_FFE8",
  "scan_time_start": 10675583,
  "scan_time_end": 10675585,
  "x": 0,
  "y": 0,
  "networks": [
    {
      "ssid": "Proximus-Home",
      "rssi": -82
    },
    {
      "ssid": "Proximus-Home",
      "rssi": -91
    }
  ]
}
```