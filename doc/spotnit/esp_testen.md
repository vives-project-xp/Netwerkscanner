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

# test 3

1. WiFi.scanNetworks(false, true, false, 80);

resultaat  
```
gemiddelde voor 1 scan = 5395
gemiddelde voor 1 scan = 5395
gemiddelde voor 1 scan = 5395
```

2. ```WiFi.scanNetworks(false, true, true, 80);``` prob aan.  

resultaat  
```
gemiddelde voor 1 scan = 1128
gemiddelde voor 1 scan = 1128
gemiddelde voor 1 scan = 1128
gemiddelde voor 1 scan = 1128
```

478% sneller. Dit klop beter met de berekende tijd 14ch * 80ms = 1120.  
Scantijden blijven ook heel consistent.


op school ook dezelfde tijden  
```
gemiddelde voor 1 scan = 1129
gemiddelde voor 1 scan = 1129
gemiddelde voor 1 scan = 1129
```

test op school
´´´
Scan start
Scan done
28 networks found
Nr | SSID                             | RSSI |  | BSSID                    | CH | Encryption
 1 | DIRECT-HC0S1EBB00701GOAD         |  -44 |  | BSSID: 11:BB:9E:95:AB:22 | 11 | WPA2
 2 | MPSK                             |  -59 |  | BSSID: 11:BA:25:4F:EB:22 | 11 | WPA2
 3 | eduroam                          |  -59 |  | BSSID: 11:BA:25:4F:EB:22 | 11 | WPA2-EAP
 4 | campusroam                       |  -59 |  | BSSID: 11:BA:25:4F:EB:22 | 11 | WPA2-EAP
 5 | VIVES-GUEST                      |  -59 |  | BSSID: 11:BA:25:4F:EB:22 | 11 | open
 6 |                                  |  -59 |  | BSSID: 11:BA:25:4F:EB:22 | 11 | WPA2
 7 | VIVES-EVENT                      |  -59 |  | BSSID: 11:BA:25:4F:EB:22 | 11 | open
 8 |                                  |  -59 |  | BSSID: 11:BA:25:4F:EB:22 | 11 | WPA2
 9 | devbit                           |  -60 |  | BSSID: 11:8B:A9:15:5F:22 |  1 | WPA2
10 | iotlab                           |  -60 |  | BSSID: 11:8B:A9:15:5F:22 |  1 | WPA2
11 | devbit                           |  -70 |  | BSSID: 11:8B:A9:15:60:22 |  6 | WPA2
12 | iotlab                           |  -70 |  | BSSID: 11:8B:A9:15:60:22 |  6 | WPA2
13 | campusroam                       |  -73 |  | BSSID: 11:BA:25:50:1B:22 |  1 | WPA2-EAP
14 |                                  |  -73 |  | BSSID: 11:BA:25:50:1B:22 |  1 | WPA2
15 |                                  |  -73 |  | BSSID: 11:BA:25:50:1B:22 |  1 | WPA2
16 | eduroam                          |  -74 |  | BSSID: 11:BA:25:50:1B:22 |  1 | WPA2-EAP
17 | MPSK                             |  -74 |  | BSSID: 11:BA:25:50:1B:22 |  1 | WPA2
18 | VIVES-EVENT                      |  -74 |  | BSSID: 11:BA:25:50:1B:22 |  1 | open
19 | Coco 🥥                         |  -77 |  | BSSID: 11:DD:00:4E:02:22|  6 | WPA2+WPA3
20 | campusroam                       |  -83 |  | BSSID: 11:BA:25:50:27:22 | 11 | WPA2-EAP
21 |                                  |  -84 |  | BSSID: 11:BA:25:50:27:22 | 11 | WPA2
22 | VIVES-EVENT                      |  -84 |  | BSSID: 11:BA:25:50:27:22 | 11 | open
23 |                                  |  -84 |  | BSSID: 11:BA:25:50:27:22 | 11 | WPA2
24 | MPSK                             |  -84 |  | BSSID: 11:BA:25:50:27:22 | 11 | WPA2
25 | eduroam                          |  -84 |  | BSSID: 11:BA:25:50:27:22 | 11 | WPA2-EAP
26 | VIVES-GUEST                      |  -87 |  | BSSID: 11:BA:25:50:26:22 |  1 | open
27 | MPSK                             |  -88 |  | BSSID: 11:BA:25:50:26:22 |  1 | WPA2
28 | VIVES-EVENT                      |  -89 |  | BSSID: 11:BA:25:50:26:22 |  1 | open
´´´

Als we goed kijken zien we dat sommige netwerken een bijna identiek bssid hebben.
Enkel de laatste waarde veranderd. 1 acces point heeft meerdere netwerken die hij uitstuurt.  

Dit is belangrijk om te weten omdat als we triangulatie willen doen dat we niet met 3 dezelfde de positie proberen te bepalen.
