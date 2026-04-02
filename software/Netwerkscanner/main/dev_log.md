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

# CPU verbruik bij scannen
--- [CPU GEBRUIK] (uS = Tijd actief | % = Belasting) ---
MonitorCpuTask  819484          <1%
IDLE            192341844               97%
tiT             104539          <1%
ScannerTask     56346           <1%
nimble_host     3249            <1%
sys_evt         3636            <1%
MenuTask        104479          <1%
Tmr Svc         7               <1%
esp_timer       120911          <1%
wifi            3057733         1%
TryConnectToWif 7165            <1%
ble_ll_task     1461            <1%

Ik zie dat de cpu niet veel tijd moet steken tijdens het scannen.  
Ik kan dus andere taken uitvoeren tijdens het scannen.


# ota (over the air update)
partitie waren niet groot genoeg om nieuw programma op te zetten.

# memory leak
na 1 min scannen crasht het programma.
mogelijk door json die payload niet free();
het was inderdaad dat.

# 
er werd geen melding naar de queue gestuurd al het verbonden is met de server.

# memory error
als er teveel netwerken gevonden worden kan de json niet gemaakt worden omdat het te groot wordt.
Door dat de taken apart zitten was het telkens net voor de 2de keer scannen waardoor ik eerst in de foute taak de fout zat te zoeken.

# test max send ap's
23 gaat maar had 1 keer als het nog aan het verbinden met wifi was dat het crashte.
30 lukt ook

# toekomst
Presence Detection: Detecteer wanneer een specifiek MAC-adres (bijvoorbeeld je eigen telefoon) in de buurt is en schakel een lamp in.

Channel Occupancy Monitor: Analyseer welke kanalen het drukst zijn (meeste beacons) om je eigen router optimaal in te stellen.

Rogue AP Detection: Scan op Access Points met jouw SSID die niet jouw MAC-adres hebben (om "Evil Twin" aanvallen op jezelf te detecteren).