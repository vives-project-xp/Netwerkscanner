# OTA SERVER

Via de OTA server kunnen we nieuwe software versturen naar de ESP via WiFi, zonder het gebruik van kabels.

Op de webpagina kies je een `.bin` bestand en de ESP installeert zichzelf opnieuw.  
Via de OTA server verloopt het uploaden sneller en eenvoudiger.

## Functies

---

### `UploadGetHandler()`

Wanneer je surft naar:

```text
http://10.10.219.58/
```

ontvangt de ESP het verzoek en stuurt het `uploadHTML` terug, wat de webpagina bevat voor het uploaden van firmware.

---

### `UploadPostHandler()`

Deze functie ontvangt en installeert het `.bin` bestand.

Bij het uploaden van een bestand via een HTML-formulier stuurt de browser extra tekstinformatie mee.  
De ESP verwacht echter alleen firmwarecode, waardoor dit anders een crash zou veroorzaken.

ESP32 firmwarebestanden beginnen altijd met de byte:

```text
0xE9
```

De functie negeert alle extra informatie vóór deze byte en installeert daarna direct de echte firmwarecode.

---

### `OtaWebserverTask()`

Start de webserver en registreert de benodigde routes.

### Routes

- `/` (`GET`)
  - toont de uploadpagina

- `/update` (`POST`)
  - ontvangt en installeert het `.bin` bestand