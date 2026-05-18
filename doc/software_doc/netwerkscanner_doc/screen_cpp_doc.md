# SCREEN

Deze code laat de ESP toe om tekst en vormen op het scherm te tonen.

## Opbouw

De code bestaat uit 3 lagen:

---

## Laag 1 — Communicatie met het scherm

Deze laag verzorgt de directe communicatie met het display.

### Functies
- `LcdCommand()`
- `LcdData()`

Deze functies sturen bytes via SPI naar het scherm.

---

## Laag 2 — Basistekenfuncties

Deze laag bevat de basisfuncties om vormen en pixels te tekenen.

### Functies
- `DrawPixel()`
- `DrawLine()`
- `FillScreen()`
- `DrawRectFilled()`

Met deze functies kunnen lijnen, rechthoeken en volledige schermen getekend of ingevuld worden.

---

## Laag 3 — Tekst en iconen

Deze laag bouwt verder op de tekenfuncties om tekst en iconen weer te geven.

### Functies
- `DrawStringFast()`
- `DrawWifiIcon()`

Deze functies worden gebruikt om tekst en symbolen op een efficiënte manier op het scherm te tonen.
