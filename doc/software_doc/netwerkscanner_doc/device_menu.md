# MENU

De netwerkscanner toont een klein menu waarop gekozen kan worden welke soorten netwerken gescand moeten worden.

## Ondersteunde netwerken

- Wifi 2.4 GHz
- Wifi 5 GHz
- Bluetooth
- Debug RTOS (extra informatie voor developers)

Links bovenaan wordt ook weergegeven:
- met welk netwerk het device verbonden is
- de naam van het netwerk
- het IP-adres

![Netwerkscanner scherm](img/sociale_media/netwerkscanner_scherm.png)

---

# FUNCTIES

Het device kan bestuurd worden met de 6 knoppen aan de zijkant, elk met een aparte functie.

## Knoppen

(Van boven naar beneden)

- **Knop 1 = UP**
  - Navigeert omhoog in het menu

- **Knop 2 = DOWN**
  - Navigeert omlaag in het menu

- **Knop 3 = SELECT**
  - Selecteert het netwerk dat gescand moet worden

- **Knop 4 = BACK**

- **Knop 5 = RESET**
  - Voert een harde reset uit van het volledige device

- **Knop 6 = Vrij**
  - Momenteel niet gebruikt

---

# CODE

Doordat de ESP maar 1 processor heeft, kan hij technisch gezien maar 1 taak tegelijk uitvoeren.

Hiervoor wordt gebruik gemaakt van **FreeRTOS**, een klein besturingssysteem dat zeer snel wisselt tussen verschillende taken zodat het lijkt alsof meerdere taken tegelijkertijd uitgevoerd worden.

`xTaskCreate()` wordt gebruikt om een taak aan te maken.

```c
xTaskCreate(
    ScannerTask,    // welke functie
    "ScannerTask",  // naam voor debugging
    4096,           // geheugen (stack) in bytes
    NULL,           // extra parameters
    5,              // prioriteit
    NULL            // handle (optioneel)
);
```

---

# QUEUES

Wanneer verschillende taken dezelfde data willen gebruiken, wordt gebruik gemaakt van **queues**.

Een queue werkt als een wachtrij:
- de eerste data die binnenkomt
- is ook de eerste data die gelezen wordt

Dit voorkomt dat meerdere taken tegelijkertijd dezelfde data gebruiken.

## Gebruikte functies

- `xQueueCreate()`
  - maakt een queue aan

- `xQueueSend()`
  - verstuurt data naar de queue

- `xQueueReceive()`
  - leest data uit de queue

---

# TAKEN

## `MenuTask`

Start het display op en wacht op berichten in de `menuqueue`.

Deze berichten komen van:
- knopinterrupts
- de WiFi-verbindingstaak

### Mogelijke berichten

- `UP`
- `DOWN`
- `SELECT`
- `BACK`
- `EVENT_WIFI_CONNECTED`
- `portMAX_DELAY`

---

## `ScannerTask`

Blijft controleren wat gescand mag worden via `GlobalScanConfig`.

`GlobalScanConfig` bepaalt welke frequentiebanden gebruikt worden in `wifi_scan_config_t`.

### WiFi scanning

Met:

```c
esp_wifi_scan_start()
```

wordt een WiFi-scan gestart.

De code wacht tot de scan volledig klaar is.

Daarna worden de resultaten opgehaald met:

```c
esp_wifi_scan_get_ap_records()
```

De resultaten worden vervolgens in een queue geplaatst voor `JsonBuilderTask`.

---

### Bluetooth scanning

Bluetooth scanning gebeurt via:

```c
StartBleScan(5000)
```

Dit start een scan van 5 seconden.

Via:

```c
ble_gap_event_handler
```

worden gevonden apparaten één voor één in de `BluetoothQueue` geplaatst.

---

## Frequentiebanden

| Scanmodus | Channel bitmap |
|---|---|
| Wifi 2.4 GHz | `0x3ffe` |
| Wifi 5 GHz | `0xfeffffe` |
| Beide | beide bitmaps actief |

---

## `JsonBuilderTask`

Verwerkt de scandata en verstuurt deze naar de server.

De taak gebruikt een **QueueSet**, waardoor tegelijkertijd op meerdere queues gewacht kan worden.

Zodra data beschikbaar is in:
- `WiFiQueue`
- `BluetoothQueue`

wordt de QueueSet actief.

### Werking

- WiFi-data wordt verdeeld in blokken van 10 netwerken per HTTP POST
- daarna wordt het gebruikte geheugen opnieuw vrijgemaakt

---

# KNOPPENSYSTEEM

Wanneer een knop ingedrukt wordt, verandert de spanning en ontstaat een interrupt.

Een knop "stuitert" echter kort bij het indrukken, waardoor meerdere signalen ontstaan terwijl slechts één input gewenst is.

Hiervoor wordt gebruik gemaakt van **debouncing**.

```c
if (now - lastIsrTimeUp < 50000) {  // 50ms in microseconden
    return;  // negeer dit signaal
}
```

Met debouncing worden alle signalen binnen de eerste 50 ms genegeerd zodat slechts één input geregistreerd wordt.