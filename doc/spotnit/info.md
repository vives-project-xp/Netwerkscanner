# test1 esp32

resultaat
```
7 networks found
Nr | SSID                             | RSSI |  | BSSID                    | CH | Encryption
 1 | Home sweet Home                  |  -80 |  | BSSID: AA:02:B8:6C:AA:D2 | 11 | WPA2
 2 | Orange-c7697                     |  -92 |  | BSSID: AA:6A:0A:2C:AA:9B |  1 | WPA2
 3 | JEAN                             |  -93 |  | BSSID: AA:24:72:4A:AA:C9 |  6 | WPA2
 4 | Proximus-Home-612796             |  -94 |  | BSSID: AA:05:95:FB:AA:61 |  6 | WPA2
 5 | Proximus-Home-832025             |  -95 |  | BSSID: AA:E3:1A:CD:AA:04 |  6 | WPA2
 6 | Proximus-Home-775089             |  -96 |  | BSSID: AA:B8:2B:8D:AA:E4 | 11 | WPA2+WPA3
 7 | Proximus-Home-D280               |  -97 |  | BSSID: AA:5D:9E:CB:AA:86 |  1 | WPA2

```

RSSI waarde veranderingen op stationaire locatie  
min = -75  
max = -80  

Het aantal gevonden netwerken verschild veel gaat van min 2 tot soms 8 

We kunnen deze problemen mogelijk verkleinen met een betere antenne.

# test2 

Door deze lijn code toe te voegen gaan de metingen beter.  ```WiFi.setSleep(false);  ```  

Maakt RSSI veel nauwkeuriger  
min = -78  
max = -79  

Gevonden netwerken nu tussen 6 en 8, een grote verbetering.  