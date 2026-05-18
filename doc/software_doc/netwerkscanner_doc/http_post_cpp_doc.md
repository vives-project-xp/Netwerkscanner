# HTTP POST

Deze code verstuurt JSON-data naar de server en ontvangt de berekende locatie terug.

## Functies

---

### `SendJsonPost()`

Verstuurt de JSON-data naar de server.

### Returnwaarden
- `0` → succesvol verzonden
- `1` → fout bij verzenden

---

### `_http_event_handler()`

Wanneer de server antwoordt, wordt deze functie automatisch aangeroepen.

De server stuurt de berekende locatie terug als JSON.  
`_http_event_handler()` haalt deze informatie uit het antwoord en slaat ze op in `LocationBasket`.

Daarna gebruikt `MenuTask` deze informatie via `EVENT_HANDLER`, wat een bericht naar `MenuTask` verstuurt.