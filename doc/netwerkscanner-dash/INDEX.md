# Netwerkscanner Dashboard - Documentatie Index

Welkom bij de volledige documentatie van het Netwerkscanner Dashboard. Dit document helpt je het juiste document te vinden voor jouw gebruik.

---

## 📖 Documentatie per Rol

### 👤 Voor Eindgebruikers & IT-Beheerders

**Start hier**: [Gebruikshandleiding](./USERGUIDE.md)

- Hoe het systeem starten
- Gebouwen en verdiepingen beheren
- Access Points plaatsen
- Project gegevens opslaan/importeren
- Veelgestelde vragen
- Troubleshooting

**Duur**: 15-20 minuten leestijd

---

### 👨‍💻 Voor Ontwikkelaars

**Start hier**: [README.md](./README.md) - Developers Gids

Dit bevat:
- Project structuur overzicht
- Snelle start instructies
- Docker setup
- Links naar technische documentatie

---

### 🚀 Voor DevOps & Systeembeheerders

**Relevante secties**:
1. [README.md](./README.md#snelle-start) - Docker Compose setup
2. [CONFIG.md](./CONFIG.md) - Configuratiebestanden
3. [FILE_SERVER.md](./FILE_SERVER.md) - Upload server deployment

---

## 📚 Volledige Documentatie Map

### Gids Documents

| Document | Onderwerp | Voor | Duur |
|----------|-----------|------|------|
| [README.md](./README.md) | Volledige project overzicht | Iedereen | 30 min |
| [USERGUIDE.md](./USERGUIDE.md) | Praktische gebruikershandleiding | Eindgebruikers | 20 min |
| [COMPONENTS.md](./COMPONENTS.md) | React componenten uitleg | Developers | 25 min |
| [FILE_SERVER.md](./FILE_SERVER.md) | Upload server API | Backend devs | 20 min |
| [HOOKS.md](./HOOKS.md) | Custom React hooks | Frontend devs | 20 min |
| [CONFIG.md](./CONFIG.md) | Configuratie bestanden | DevOps | 25 min |
| [API.md](./API.md) | API integratie & proxy | Developers | 20 min |

---

## 🎯 Snelle Links op Onderwerp

### Installatie & Setup

- [Quick Start](./README.md#snelle-start)
- [Docker Compose Setup](./README.md#snelle-start)
- [Environment Variables](./CONFIG.md#env-file)
- [Port Configuration](./CONFIG.md#vite-configjs)

### Gebruikershandleiding

- [Aan de slag](./USERGUIDE.md#aan-de-slag)
- [Gebouwen beheren](./USERGUIDE.md#gebouwen-beheren)
- [Access Points plaatsen](./USERGUIDE.md#access-points-plaatsen)
- [Project data](./USERGUIDE.md#project-gegevens)
- [FAQ](./USERGUIDE.md#veelgestelde-vragen)

### Technische Architectuur

- [Project Structuur](./README.md#project-structuur)
- [Technologie Stack](./README.md#technologie-stack)
- [Component Hierarchie](./COMPONENTS.md#component-hierarchie)
- [Data Flow](./COMPONENTS.md#data-flow)

### Backend & APIs

- [File Upload Server](./FILE_SERVER.md)
- [API Endpoints](./FILE_SERVER.md#-api-endpoints)
- [API Integratie](./API.md)
- [Proxy Configuration](./API.md#-proxy-configuration)

### Frontend Components

- [App.jsx](./COMPONENTS.md#appjsx)
- [Pages](./COMPONENTS.md#pages)
- [Components](./COMPONENTS.md#components)
- [Custom Hooks](./HOOKS.md)

### Configuratie

- [package.json](./CONFIG.md#-packagejson)
- [vite.config.js](./CONFIG.md#-viteconfigjs)
- [Dockerfile](./CONFIG.md#-dockerfile)
- [docker-compose.yml](./CONFIG.md#-docker-composeyml)
- [Tailwind Config](./CONFIG.md#-tailwindconfigjs)
- [ESLint Config](./CONFIG.md#-eslintconfigjs)

### Problemen Oplossen

- [Troubleshooting](./README.md#-probleemoplossing)
- [Error Handling](./API.md#-error-handling)
- [Common Issues](./HOOKS.md#common-issues--solutions)

---

## 📊 Documento Structuur

```
doc/
└── netwerkscanner-dash/
    ├── README.md              # Hoofd documentatie
    ├── USERGUIDE.md           # Voor eindgebruikers
    ├── COMPONENTS.md          # React componenten
    ├── FILE_SERVER.md         # Upload server
    ├── HOOKS.md               # Custom hooks
    ├── CONFIG.md              # Configuratie
    ├── API.md                 # API integratie
    └── INDEX.md               # Dit document
```

---

## 🚀 Aan de Slag Scenario's

### Scenario 1: Ik wil het dashboard gebruiken

1. Lees: [USERGUIDE.md](./USERGUIDE.md)
2. Start: Docker Compose (zie [README.md](./README.md#snelle-start))
3. Open: http://localhost
4. Volg: Stap-voor-stap instructies

**Tijd**: ~30 minuten

---

### Scenario 2: Ik wil het lokaal ontwikkelen

1. Lees: [README.md - Developers Gids](./README.md#-ontwikkelaars-gids)
2. Installeer: `npm install`
3. Start: `npm run dev:full`
4. Lees: [COMPONENTS.md](./COMPONENTS.md) voor componenten
5. Lees: [CONFIG.md](./CONFIG.md) voor instellingen

**Tijd**: ~45 minuten

---

### Scenario 3: Ik wil het in productie deployen

1. Lees: [README.md - Docker Deployment](./README.md#docker-development)
2. Lees: [CONFIG.md - Configuration](./CONFIG.md)
3. Setup: Environment variables (`.env`)
4. Deploy: `docker-compose up -d`
5. Monitor: Docker logs

**Tijd**: ~1 uur

---

### Scenario 4: Ik wil een nieuw feature toevoegen

1. Lees: [COMPONENTS.md - Structuur](./COMPONENTS.md#component-architectuur)
2. Lees: [HOOKS.md - State Management](./HOOKS.md)
3. Lees: [API.md - Integratie](./API.md)
4. Maak: Nieuwe component/hook
5. Test: Lokaal met `npm run dev`

**Tijd**: Afhankelijk van feature

---

## 🔗 Externe Referenties

### Technologie Documentatie

- [React](https://react.dev)
- [Vite](https://vitejs.dev)
- [Tailwind CSS](https://tailwindcss.com)
- [Express.js](https://expressjs.com)
- [Docker](https://docs.docker.com)

### Bibliotheken

- [Lucide React Icons](https://lucide.dev)
- [Axios HTTP Client](https://axios-http.com)
- [IndexedDB](https://developer.mozilla.org/en-US/docs/Web/API/IndexedDB_API)

---

## ❓ Veel Gestelde Vragen

### V: Welk document moet ik lezen?

**A**: 
- Eindgebruiker? → [USERGUIDE.md](./USERGUIDE.md)
- Developer? → [README.md](./README.md)
- DevOps? → [CONFIG.md](./CONFIG.md)
- Alle onderdelen? → Lees alles in volgorde

---

### V: Hoe installeer ik het systeem?

**A**: Zie [README.md - Snelle Start](./README.md#snelle-start) voor stap-voor-stap instructies.

---

### V: Welke poorten worden gebruikt?

**A**: 
- Port 80: Dashboard
- Port 3030: Upload Server
- Port 3306: Database
- Port 8081: WiFi API

Zie [docker-compose.yml](./CONFIG.md#-docker-composeyml) voor details.

---

### V: Kan ik componenten uitbreiden?

**A**: Ja! Zie [COMPONENTS.md - Uitbreiding](./COMPONENTS.md#uitbreiding) voor template.

---

### V: Hoe debug ik API calls?

**A**: Zie [API.md - Debugging](./API.md#-debugging-api-calls).

---

## 📋 Documentatie Checklist

Zorg dat je het volgende hebt:

- [ ] [README.md](./README.md) - Project overzicht
- [ ] Relevante rol-gebaseerde gids:
  - [ ] [USERGUIDE.md](./USERGUIDE.md) - Voor eindgebruikers
  - [ ] [COMPONENTS.md](./COMPONENTS.md) - Voor developers
  - [ ] [CONFIG.md](./CONFIG.md) - Voor DevOps
- [ ] [FILE_SERVER.md](./FILE_SERVER.md) - Voor upload/storage
- [ ] [HOOKS.md](./HOOKS.md) - Voor state management
- [ ] [API.md](./API.md) - Voor integratie

---

## 🎓 Leesvolgorde Aanbevelingen

### Voor Eindgebruikers (30 min)
1. [USERGUIDE.md](./USERGUIDE.md) - Compleet

### Voor Frontend Developers (90 min)
1. [README.md](./README.md) - Overzicht
2. [COMPONENTS.md](./COMPONENTS.md) - Alle secties
3. [HOOKS.md](./HOOKS.md) - Custom logic
4. [CONFIG.md](./CONFIG.md) - Vite & Tailwind
5. [API.md](./API.md) - Integratie

### Voor Backend Developers (60 min)
1. [README.md](./README.md) - Overzicht
2. [FILE_SERVER.md](./FILE_SERVER.md) - Upload server
3. [API.md](./API.md) - API endpoints
4. [CONFIG.md](./CONFIG.md) - Docker setup

### Voor DevOps/Systeembeheerders (45 min)
1. [README.md](./README.md#snelle-start) - Quick start
2. [CONFIG.md](./CONFIG.md) - Alle secties
3. [FILE_SERVER.md](./FILE_SERVER.md) - Deployment

---

## 🔄 Updates & Maintenance

### Documentatie Wordt Bijgewerkt Wanneer:

- ✅ Nieuwe features worden toegevoegd
- ✅ Dependencies worden geupdatet
- ✅ Configuration verandert
- ✅ Bugs worden opgelost

### Laatst Bijgewerkt

- **Datum**: 2026
- **Versie**: 1.0.0
- **Status**: Volledig

---

## 📞 Support & Feedback

### Vragen?

- Lees eerst het relevante document
- Check [FAQ](./USERGUIDE.md#-veelgestelde-vragen)
- Contacteer je IT-beheerder

### Feedback op Documentatie?

- Report issues op GitHub
- Suggesties voor verbetering welkom

---

## 🎯 Korte Samenvattingen

### README.md
Compleet project overzicht met snelle start, structuur, technologie stack en links naar gedetailleerde docs.

### USERGUIDE.md
Praktische stap-voor-stap gids voor eindgebruikers om het dashboard te gebruiken.

### COMPONENTS.md
Uitgebreide beschrijving van alle React componenten en hun interacties.

### FILE_SERVER.md
API documentatie voor de upload server en project data management.

### HOOKS.md
Gedetailleerde gids over custom React hooks en state management.

### CONFIG.md
Configuratie bestanden uitgelegd: package.json, vite, tailwind, docker, etc.

### API.md
API integratie, proxy configuration, request/response examples.

---

**Start je avontuur met het Netwerkscanner Dashboard!** 🚀

---

*Versie 1.0.0 - 2026*
