# OTA SERVER
Via de OTA server kunnen we nieuwe software versturen naar de ESP via WiFi, zonder het gebruik van kabels. Op de webpagina kies je een .bin bestand en de ESP installeerd zichzelf opnieuw. Via de OTA server is het uploaden sneller.

De 3 functies:
- UploadGetHandler(): Wanneer je surf naar "http://10.10.219.58/"(zie ip adres links bovenaan schermpje) ontvangt de ESP de verzoek en stuurt het uploadHTML terug, wat de webpagina bevat.
- UploadPostHandler(): ontvangt en installeert de .bin. Bij het uploaden van een bestand via een HTML formulier, steekt je browser extra informatie als text erbij. De ESP verwacht alleen code, wat het dus zou laten crashen. ESP32 firmware bestanden beginnen dus altijd met de byte 0xE9. Het negeert de extra informatie en installeert direct het echte code.

- OtaWebserverTask(): start de webserver. Registreert 2 routes
    - / (GET): toont uploadpagina
    - /update (POST): ontvang en installeer .bin
