# HTTP POST
Deze code verstuurt de JSON naar de server en ontvangt de locatie die de server terug heeft.
De functies:
- SendJsonPost(): verstuurt de Json naar de server. Geeft een 0 terug als het lukt, 1 als het mislukt
- _http_event_handler(): Wanneer de server antwoordt wordt deze functie automatisch aangeroepen. De server stuurt de berekende locatie terug als JSON. _http_event_handler haalt deze eruit.
Deze wordt opgeslagen in LocationBasket. MenuTask gebruikt dan de informatie in LocationBasket aan de hand van EVENT_HANDLER (een bericht naar MenuTask)
