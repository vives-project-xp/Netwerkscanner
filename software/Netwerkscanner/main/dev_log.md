# waarom gebruikt het scherm geen library?
als ik een library wil toevoegen is het altijd miserie dat het het h bestand niet vind of het vind het enkel in de main of in een cpp bestand(onvoorspelbaar). Ook loop de programmeer omgeving vast en vind het plots de wifi library niet ook al heeft het er normaal geen invloed op.


# verbruik van task's 27/03/2026
--- [CPU GEBRUIK] (uS = Tijd actief | % = Belasting) ---
monitorCpuTask  1128668         1%
IDLE            94487608                97%
tiT             47990           <1%
nimble_host     3350            <1%
MenuTask        51292           <1%
Tmr Svc         6               <1%
TryConnectToWif 3996            <1%
esp_timer       64987           <1%
wifi            1422851         1%
sys_evt         1275            <1%
ble_ll_task     1454            <1%

--- [TAAK STATUS] (X=Run, R=Ready, B=Blocked, S=Suspended) ---
Naam            Stat    Prio    VrijeStack      ID
monitorCpuTask  X       6       1376    18
IDLE            R       0       1256    3
tiT             B       18      2616    5
MenuTask        B       5       2188    10
Tmr Svc         B       1       1784    4
TryConnectToWif B       7       1980    11
sys_evt         B       20      916     6
wifi            B       23      3756    7
esp_timer       S       22      3600    1
ble_ll_task     B       23      3596    8
nimble_host     B       21      2148    9

# macro collision
oplossing  
//Moet hier staan anders compile error door combinatie van  
//lwIP en Arduino  
#ifdef INADDR_NONE  
#undef INADDR_NONE  
#endif  

#ifdef INADDR_ANY  
#undef INADDR_ANY  
#endif  

# button up triggert 2 keer
de andere knoppen doen normaal

trig bij induwen  
rig bij loslaten  

condensators lossen het niet op.  
```cpp
static void IRAM_ATTR buttonIsrUp(void *arg)
{
    uint64_t now = esp_timer_get_time();
    int currentState = gpio_get_level(GPIO_BUTTON_UP);

    if (now - lastIsrTimeUp > DEBOUNCE_DELAY)
    {
        // RISING EDGE: Hardware is HOOG, maar software dacht nog LAAG
        if (currentState == 1 && pressedUp == 0)
        {
            ButtonEventT event = BUTTON_UP;
            xQueueSendFromISR(menuQueue, &event, NULL);
            
            pressedUp = 1;        // Zet vlag op "ingedrukt"
            lastIsrTimeUp = now;
        }
        // FALLING EDGE: Hardware is LAAG, maar software dacht nog HOOG
        else if (currentState == 0 && pressedUp == 1)
        {
            // We sturen geen event naar de queue (zoals gevraagd),
            // maar we resetten de vlag zodat de volgende klik weer werkt.
            pressedUp = 0;        
            lastIsrTimeUp = now;  // (negeert release-dender)
        }
    }
}
```
Anyedge Interrupt: De ISR wordt bij elke verandering (stijgend en dalend) aangeroepen.

Zelf-correctie: Als gpio_get_level() door dender een verkeerde waarde leest (bijv. 0 terwijl je drukt), faalt de if-check. Omdat de tijdstempel (lastIsrTimeUp) dan niet wordt bijgewerkt, blijft de deur openstaan voor de volgende dender-puls (microseconden later) die wél de juiste waarde leest.

Lock-out: Zodra een stabiele waarde (1 bij press, 0 bij release) is geregistreerd, blokkeert de DEBOUNCE_DELAY alle volgende interrupts voor 50ms.

# down button triggert nu meer
dit komt door de condensators die zorgen dat het uitgaan langzamer gaat en bij down is dit een grotere dan de andere.

# static volatile uint64_t lastIsrTimeUp = 0;
static is zodat het enkel in het bestand kan gebruikt worden.
Onthouden (Variabelen): Als je static binnen een functie gebruikt, wordt de waarde niet vergeten als de functie klaar is. Bij de volgende klik staat de oude tijd er nog in.

# condensators weg gedaan 
interrupt werkt niet meer

# het werkt met dit 
```cpp
static void IRAM_ATTR buttonIsrUp(void *arg)
{
    uint64_t now = esp_timer_get_time();

    // 1. Harde debounce: negeer alles binnen 50ms (50000 us)
    if (now - lastIsrTimeUp < 50000) {
        return;
    }

    // 2. Omdat we geen digitalRead doen, vertrouwen we op de 'state'
    if (!pressedUp) {
        // We gaan ervan uit dat dit de RISING edge is
        ButtonEventT event = BUTTON_UP;
        xQueueSendFromISR(menuQueue, &event, NULL);
        
        pressedUp = true;
        lastIsrTimeUp = now;
    } else {
        // We gaan ervan uit dat dit de FALLING edge is
        pressedUp = false;
        lastIsrTimeUp = now;
    }
}
```


# stappen plan
1. Systeem Initialisatie (NVS & WiFi)
Initialiseer de NVS (Non-Volatile Storage), dit is nodig voor de WiFi-stack.

Configureer de WiFi in Station Mode, maar maak nog geen verbinding met een netwerk.

Stel de 2.4 GHz en 5 GHz scan-parameters in (standaard scant de C5 beide).

2. De RTOS Structuur Opzetten
Maak één hoofdtaak aan (bijv. ScannerTask) met een stack-grootte van ±4096 bytes.

Maak een Static Buffer of een kleine Queue aan om de scan-resultaten tijdelijk op te slaan zonder de heap te vervuilen.

3. De "Async" Scan Cyclus
Start WiFi Scan: Gebruik esp_wifi_scan_start met de parameter block = false.

Parallelle Actie: Terwijl de hardware de kanalen afgaat, laat je de CPU de JSON-string of het datapakket van de vorige meting voorbereiden.

Event Wachten: Gebruik een RTOS Task Notification om de taak te laten pauzeren totdat de WiFi-driver het signaal "Scan Done" geeft.

4. Bluetooth Intermezzo
Zodra de WiFi-scan klaar is, start je een korte BLE Scan (bijv. 2-5 seconden).

Verzamel de gevonden MAC-adressen en RSSI-waarden van de Bluetooth-apparaten.

5. Verbinding en Overdracht
Verbind nu pas met je eigen WiFi-netwerk (of gebruik ESP-NOW als je geen volledige verbinding wilt).

Verstuur het voorbereide datapakket (WiFi 2.4 + 5 + BLE) naar de server.

Sluit de verbinding netjes af om RAM vrij te maken.

6. Opschonen en Rust (Deep Sleep)
Gebruik esp_wifi_scan_stop en maak de tijdelijke buffers leeg.

Zet de taak in vTaskDelay of breng de C5 in Deep Sleep voor een bepaald interval om stroom te besparen.