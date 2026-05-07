# API
Zet WiFi scandata om naar JSON en verstuurt naar de server

De functies:
- PrintApiInfo(): print alle info van 1 WiFi netwerk naar de console, gebruikt voor data te checken. Heeft geen werkelijke functie.
- GetChipId(): geeft een unieke naam zoals ESP32-C5FFE8. De naamt komt van de chiptype en de laatste 2 bytes van het MAC-adress. DE server weet hierdoor welk apparaat de data stuurt.
- CreateWiFiJson(): Neemt een lijst van gescande netwerken en bouwt daar een JSON string van. Die heeft ook parameters:
   - wifi_ap_record_t* aps  // de lijst van gescande netwerken
   - uint16_t start         // vanaf welk netwerk beginnen
   - uint16_t count         // hoeveel netwerken meenemen
   - uint64_t TimeStart     // wanneer begon de scan
   - uint64_t TimeEnd       // wanneer eindigde de scan
   - int32_t x, y          // locatie (standaard 0,0)

- Het resultaat ziet er zo uit:
   -   {
   - "device_id": "ESP32-C5_FFE8",
   - "scan_time_start": 10675583,
   - "scan_time_end": 10675585,
   - "x": 0,
   - "y": 0,
   - "networks": [
   - { "ssid": "Proximus-Home", "rssi": -82, ... },
   - { "ssid": "Proximus-Home", "rssi": -91, ... }
   - ]
   - }