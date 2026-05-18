# Netwerkscanner Dashboard - Volledige Documentatie

## Inhoudsopgave

1. [Overzicht](#overzicht)
2. [Snelle Start](#snelle-start)
3. [Project Structuur](#project-structuur)
4. [Technologie Stack](#technologie-stack)
5. [Functionaliteiten](#functionaliteiten)
6. [Gebruikshandleiding](#gebruikshandleiding)
7. [Ontwikkelaars Gids](#ontwikkelaars-gids)

---

## Overzicht

Het **Netwerkscanner Dashboard** is een webgebaseerde applicatie voor het beheren en visualiseren van draadloze netwerken in gebouwen. Het systeem stelt gebruikers in staat om:

- Gebouwen en verdiepingen te beheren
- WiFi-signaalsterkte (RSSI) gegevens te visualiseren
- Toegangspunten (Access Points) in kaarten in te tekenen
- Projectgegevens op te slaan en te delen

### Voor wie is dit?

- **Eindgebruikers**: IT-beheerders en netwerkspecialisten
- **Ontwikkelaars**: Backend engineers en frontend developers die het systeem uitbreiden
- **Iedereen**: De interface is intuïtief genoeg om zonder technische achtergrond te gebruiken

---

## Snelle Start

### Vereisten

- Docker en Docker Compose geïnstalleerd
- Git (om het project te clonen)

### Instalatie

1. **Clone het project**

   ```bash
   git clone <repository-url>
   cd Netwerkscanner
   ```

2. **Maak een `.env` bestand** in de projectroot

   ```env
   MYSQL_ROOT_PASSWORD=your_root_password
   MYSQL_DATABASE=netwerkscanner_db
   MYSQL_USER=scanner_user
   MYSQL_PASSWORD=scanner_password
   ```

3. **Start de applicatie via Docker Compose**

   ```bash
   docker-compose up
   ```

4. **Open je browser**

   - Dashboard: http://localhost
   - Upload Server: http://localhost:3030

### Afsluiten

```bash
docker-compose down
```

---

## Project Structuur

```
netwerkscanner-dash/
├── src/                      # React source code
│   ├── components/          # Herbruikbare UI-componenten
│   ├── pages/              # Volledige paginaweergaven
│   ├── hooks/              # Custom React hooks
│   ├── entities/           # Gegevensmodellen
│   ├── assets/             # Afbeeldingen en media
│   ├── App.jsx             # Hoofdtoepassing
│   ├── main.jsx            # React entry point
│   ├── index.css           # Globale stijlen
│   └── App.css             # App-specifieke stijlen
│
├── public/                  # Statische bestanden
├── dist/                    # Build output (gegenereerd)
│
├── Dockerfile              # Docker configuratie
├── docker-compose.yml      # Services orchestratie
├── package.json            # Dependencies en scripts
├── vite.config.js          # Vite bundler configuratie
├── tailwind.config.js      # Tailwind CSS configuratie
├── postcss.config.js       # PostCSS configuratie
├── eslint.config.js        # Code quality configuratie
│
├── file-upload-server.js   # Node.js upload server
├── index.html              # HTML template
└── README.md               # Dit bestand
```

---

## Technologie Stack

| Laag            | Technologie    | Functie                   |
| --------------- | -------------- | ------------------------- |
| **Frontend**    | React 19       | UI Framework              |
| **Build Tool**  | Vite           | Snelle build & dev server |
| **Styling**     | Tailwind CSS 4 | Utility-first CSS         |
| **Icons**       | Lucide React   | Icon library              |
| **Backend**     | Express.js     | File upload server        |
| **HTTP Client** | Axios          | API requests              |
| **Storage**     | IndexedDB      | Lokale client-side data   |
| **Deployment**  | Docker         | Containerization          |

---

## Functionaliteiten

### Hoofdscherm (Dashboard)

- Snelle statistieken weergeven
- Actieve projects bekijken
- Snelle navigatie naar andere secties

### Gebouwen Beheren

- **Toevoegen**: Nieuwe gebouwen creëren met naam en details
- **Verwijderen**: Gebouwen verwijderen (met bevestiging)
- **Selecteren**: Een gebouw kiezen om in detail te bewerken

### Gebouwdetails

- **Verdiepingen**: Meerdere verdiepingen per gebouw toevoegen
- **Verdiepingskaarten**: Plattegronden uploaden als afbeeldingen
- **Access Points**: WiFi-punten op kaarten plaatsen
- **RSSI-gegevens**: Signaalsterktegegevens visualiseren

---

## Gebruikshandleiding

### 1. Een Nieuw Project Starten

**Stap 1: Ga naar "Gebouwen"**

- Klik op "Gebouwen" in het linkermenu

**Stap 2: Voeg een gebouw toe**

- Klik "Nieuw Gebouw"
- Vul gebouwgegevens in (naam, adres, etc.)
- Klik "Opslaan"

**Stap 3: Voeg verdiepingen toe**

- Klik op het gebouw om details te openen
- Klik "Voeg verdieping toe"
- Vul verdiepingsnummer in
- Upload een afbeelding van de plattegrond

### 2. Access Points Plaatsen

**Stap 1: Selecteer een verdieping**

- Open het gebouw en selecteer de verdieping in de FloorSelector

**Stap 2: Upload/Bekijk de plattegrond**

- De plattegrondafbeelding wordt weergegeven
- Je kunt in/uit zoomen en pannen met muisgestures

**Stap 3: Voeg Access Points toe**

- Klik op "Access Points Toevoegen" in het linker menu
- Klik op de kaart om een punt toe te voegen
- Vul de BSSID (MAC-adres) in
- Vul de signaalsterkte (RSSI) in
- Sla op

### 3. Gegevens Exporteren

**Projectgegevens opslaan**

- De app slaat automatisch op in IndexedDB
- Download volledige projectdata via het menu

**Projectgegevens importeren**

- Kies "Importeren" in het menu
- Selecteer een eerder gedownload JSON-bestand
- De gegevens worden hersteld

---

##  Ontwikkelaars Gids

### Lokale Ontwikkeling Starten

```bash
# Installeer dependencies
npm install

# Start development server + upload server
npm run dev:full

# Of aparte commands:
npm run dev              # Vite dev server (poort 5173)
npm run upload-server    # Upload server (poort 3030)
```

### Project Structuur Documentatie

#### [**App.jsx**](./COMPONENTS.md#appjsx)

Hoofdtoepassing met tab-navigatie en state management.

#### [**file-upload-server.js**](./FILE_SERVER.md)

Express server voor bestandsuploads en projectgegevensbeheer.

#### [**src/components/**](./COMPONENTS.md#components)

Herbruikbare UI-componenten voor verschillende functionaliteiten.

#### [**src/pages/**](./PAGES.md)

Volledige paginaweergaven (Dashboard, Buildings, BuildingDetail).

#### [**src/hooks/**](./HOOKS.md)

Custom React hooks voor data fetching en state management.

#### [**Configuratiebestanden**](./CONFIG.md)

- `vite.config.js` - Build configuratie
- `tailwind.config.js` - Styling setup
- `docker-compose.yml` - Service orchestratie
- `.env` - Omgevingsvariabelen

### Build en Deployment

```bash
# Production build
npm run build

# Preview production build
npm run preview

# Lint code
npm run lint
```

### Docker Development

```bash
# Build services
docker-compose build

# Start met live reload
docker-compose up

# Kijk logs
docker-compose logs -f network-scanner

# Stop services
docker-compose down
```

### API Integratie

De app maakt verbinding met:

- **WiFi API**: `http://10.20.10.24:8081` (naar `/api` proxy)
- **Upload Server**: `http://localhost:3030` (lokaal)

Alle routes worden geproxied via `vite.config.js`.

---

## Gedetailleerde Documentatie

Zie de volgende bestanden voor meer details:

- **[COMPONENTS.md](./COMPONENTS.md)** - Beschrijving van alle React componenten
- **[PAGES.md](./PAGES.md)** - Pagina-logica en flow
- **[HOOKS.md](./HOOKS.md)** - Custom React hooks
- **[FILE_SERVER.md](./FILE_SERVER.md)** - Upload server API
- **[CONFIG.md](./CONFIG.md)** - Configuratiebestanden uitgelegd
- **[API.md](./API.md)** - API endpoints en integratie

---

## Probleemoplossing

### Dashboard laadt niet

- Controleer of alle Docker services draaien: `docker-compose ps`
- Controleer de browser console voor fouten

### Upload server werkt niet

- Controleer poort 3030: `netstat -an | grep 3030`
- Check Docker logs: `docker-compose logs upload-server`

### IndexedDB fouten

- Wis browsercache: DevTools > Application > Clear Site Data
- Reset IndexedDB database manueel

### Vite build fouten

- Verwijder `node_modules` en `dist`
- Herinstalleer: `npm install && npm run build`

---

**Laatst bijgewerkt**: 2026
**Versie**: 1.0.0
