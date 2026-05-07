# TIME SYNC
Deze code syncronizeerd de tijd op de ESP met de tijd op de server, doordat de ESP geen ingebouwde klok heeft.
De 4 Functies:
- Init(): Zet beide variabelen terug op 0, wordt aangeroepen bij het opstarten
- Sync(): roept de GetTimeFromServer functie op en slaat die op met het huidige ESP-tijdstip.  esp_timer_get_time() telt het aantal microseconden sinds de ESP opgestart is.
- Get(): Berekent de huidige tijd zonder opnieuw naar de server te gaan. Dit wordt berekent door de servertijd op het moment van het opstarten plus het verstreken seconden sinds de sync.

- GetTimeFromServer(): vraagt de tijd op via HTTP GET naar /time op de server en leest de timestamp uit het JSON-antwoord
