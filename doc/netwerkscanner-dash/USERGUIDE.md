# Netwerkscanner Dash - Gebruikshandleiding

Deze gebruikshandleiding hoort bij de Netwerkscanner Dash front-end. De inhoud hieronder is afgestemd op dit project: korte uitleg, installatie via Docker Compose, bestand-upload en veelvoorkomende problemen.

---

## Inhoud

- [Netwerkscanner Dash - Gebruikshandleiding](#netwerkscanner-dash---gebruikshandleiding)
  - [Inhoud](#inhoud)
  - [Snelstart (Docker Compose)](#snelstart-docker-compose)
  - [Gebruiken van de Dash](#gebruiken-van-de-dash)
  - [Uploaden van scanbestanden](#uploaden-van-scanbestanden)
  - [Configuratie en ontwikkelmodus](#configuratie-en-ontwikkelmodus)
  - [Problemen oplossen](#problemen-oplossen)
  - [Nuttige locaties in de repository](#nuttige-locaties-in-de-repository)

---

## Snelstart (Docker Compose)

1. Vanaf de repository-root start je de services met:

   ```bash
   docker compose up --build -d
   ```

2. Controleer de logs als iets niet start:

   ```bash
   docker compose logs -f
   ```

3. Open de Dash op `http://localhost:8080` (of de poort zoals gedefinieerd in `docker-compose.yml`).

4. Stop de services met:

   ```bash
   docker compose down
   ```

---

## Gebruiken van de Dash

- De Dash toont gescande netwerkgegevens en biedt een eenvoudige upload-interface voor scanbestanden.
- Navigatie: Dashboard, Upload, (optioneel) Settings.
- Dashboard: overzicht van recente scans en statistieken.
- Upload: gebruik het formulier om scanbestanden (meestal JSON) te versturen naar de backend.

---

## Uploaden van scanbestanden

1. Ga naar de Upload-pagina in de Dash.
2. Klik op "Bestand kiezen" of sleep het bestand naar het uploadvlak.
3. Selecteer het juiste scanbestand (JSON) en klik op "Upload".
4. Na succesvolle upload wordt meestal een bevestiging getoond en/of wordt de lijst met scans ververst.

Opmerkingen:

- Controleer bij fouten de backend-logs (`docker compose logs`) voor details.
- Zorg dat het bestandstype overeenkomt met wat de backend verwacht (JSON).

---

## Configuratie en ontwikkelmodus

- Optioneel lokaal (zonder Docker):

```bash
cd Netwerkscanner/netwerkscanner-dash
npm install
npm run dev
```

- Voor productie: gebruik `npm run build` en host de inhoud van `dist/`.
- API-endpoints en proxy-instellingen kunnen in `vite.config.js` of via environment-variabelen worden aangepast.

---

## Problemen oplossen

- Dashboard niet bereikbaar: controleer `docker compose ps` en `docker compose logs`.
- Upload werkt niet: bekijk backend-logs en controleer bestandsgrootte/format.
- Foutmeldingen in frontend: open de browser console (F12) voor netwerk- en JS-errors.

---

## Nuttige locaties in de repository

- Frontend bron: [Netwerkscanner/netwerkscanner-dash/src/](../../software/netwerkscanner-dash/src/)
- Upload-backend: [Netwerkscanner/netwerkscanner-dash/file-upload-server.js](../../software/netwerkscanner-dash/file-upload-server.js)
- Docker-compose: [docker-compose.yml](../../docker-compose.yml) en [netwerkscanner-dash/Dockerfile](../../software/netwerkscanner-dash/Dockerfile)
- Documentatie (dit dossier): [doc/netwerkscanner-dash/](./)

---

**Laatst bijgewerkt**: 2026
**Versie**: 1.1.0
