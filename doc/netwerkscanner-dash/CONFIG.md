# Configuratiebestanden - Gedetailleerde Gids

## Overzicht

Dit document beschrijft alle configuratiebestanden in het netwerkscanner-dash project.

---

## package.json

**Doel**: Node.js project configuratie en dependency management

### Scripts

```json
{
  "scripts": {
    "dev:full": "concurrently \"npm run upload-server\" \"npm run dev\"",
    "dev": "vite",
    "upload-server": "node file-upload-server.js",
    "build": "vite build",
    "lint": "eslint .",
    "preview": "vite preview"
  }
}
```

| Script | Functie | Port |
|--------|---------|------|
| `npm run dev:full` | Start Vite + Upload Server | 5173, 3030 |
| `npm run dev` | Start Vite dev server | 5173 |
| `npm run upload-server` | Start file upload server | 3030 |
| `npm run build` | Production build | - |
| `npm run lint` | Code quality check | - |
| `npm run preview` | Preview van build | 4173 |

### Dependencies

**Frontend Framework**:

- `react@19.2.4` - UI library
- `react-dom@19.2.4` - React DOM rendering

**Styling**:

- `@tailwindcss/postcss@4.2.2` - Tailwind CSS
- `@tailwindcss/vite@4.2.2` - Vite integration
- `lucide-react@1.0.1` - Icon library

**Utilities**:

- `axios@1.13.6` - HTTP client
- `idb-keyval@6.2.2` - IndexedDB wrapper
- `prop-types@15.8.1` - Runtime type checking

**Interactie**:

- `react-quick-pinch-zoom@5.1.1` - Touch zoom support
- `react-zoom-pan-pinch@3.7.0` - Mouse zoom & pan

**Backend**:

- `express@5.2.1` - Web server
- `multer@2.1.1` - File upload handling
- `cors@2.8.6` - Cross-origin requests

### DevDependencies

**Build & Dev**:

- `vite@8.0.1` - Build tool & dev server
- `@vitejs/plugin-react@6.0.1` - React support

**Linting**:

- `eslint@9.39.4` - Code linter
- `eslint-plugin-react-hooks@7.0.1` - React hooks rules
- `eslint-plugin-react-refresh@0.5.2` - Refresh rules

**CSS Processing**:

- `postcss@8.5.8` - CSS transformations
- `autoprefixer@10.4.27` - Browser prefixes
- `tailwindcss@4.2.2` - CSS framework

**Type Checking** (optional):

- `@types/react@19.2.14` - React type definitions
- `@types/react-dom@19.2.3` - React DOM types

---

## vite.config.js

**Doel**: Build tool en development server configuratie

### Plugin Configuration

```javascript
plugins: [
  react(),           // React HMR support
  tailwindcss()      // Tailwind CSS compilation
]
```

### Server Configuration

```javascript
server: {
  host: true,        // Listen on all interfaces (0.0.0.0)
  proxy: { ... }     // API proxy configuration
}
```

### API Proxies

```javascript
'/api': {
  target: 'http://10.20.10.24:8081',
  changeOrigin: true,
  rewrite: (path) => path.replace(/^\/api/, '')
}
```

**Functie**: Verwijdert `/api` prefix en stuurt door naar WiFi API server

**Voorbeeld**:

- Browser request: `GET /api/networks`
- Server request: `GET http://10.20.10.24:8081/networks`

### Upload Server Proxies

```javascript
'/upload': {
  target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
  changeOrigin: true
}
```

**Omgevingsvariabelen**:

- `VITE_UPLOAD_SERVER` - Custom upload server (default: localhost:3030)
- `VITE_API_URL` - Custom API endpoint

**Usage in Docker**:

```yaml
environment:
  - VITE_UPLOAD_SERVER=http://upload-server:3030
```

---

## tailwind.config.js

**Doel**: Tailwind CSS customization

### Default Setup

```javascript
/** @type {import('tailwindcss').Config} */
export default {
  content: [],
  theme: {
    extend: {},
  },
  plugins: [],
}
```

### Custom Theming (aanbevolen)

**Donker thema kleurpalet**:

```javascript
theme: {
  extend: {
    colors: {
      'slate': {
        900: '#0f172a',  // Background
        800: '#1e293b',  // Sidebar
      }
    }
  }
}
```

**Content Path** (belangrijk):

```javascript
content: [
  './index.html',
  './src/**/*.{js,jsx,ts,tsx}',
]
```

---

## postcss.config.js

**Doel**: CSS preprocessing

### Standard Configuration

```javascript
export default {
  plugins: {
    tailwindcss: {},
    autoprefixer: {},
  },
}
```

**Pipeline**:

1. Tailwind CSS generaties CSS
2. Autoprefixer voegt vendor prefixes toe
3. Output: Cross-browser compatible CSS

### Autoprefixer Details

Voegt automatisch vendor prefixes toe:

```css
/* Input */
display: grid;

/* Output */
display: -webkit-grid;
display: grid;
```

---

## eslint.config.js

**Doel**: Code quality en best practices

### Configuration

```javascript
import js from '@eslint/js'
import globals from 'globals'
import react from 'eslint-plugin-react'
import reactHooks from 'eslint-plugin-react-hooks'
import reactRefresh from 'eslint-plugin-react-refresh'
```

### Recommended Rules

```javascript
{
  files: ['**/*.{js,jsx}'],
  languageOptions: {
    globals: globals.browser,
  },
  rules: {
    ...react.configs.recommended.rules,
    ...reactHooks.configs.recommended.rules,
  }
}
```

### Rules Uitgelegd

| Rule                                   | Doel                        |
| -------------------------------------- | --------------------------- |
| `react/jsx-*`                          | JSX syntax regels           |
| `react-hooks/rules-of-hooks`           | Hooks moet in functions     |
| `react-hooks/exhaustive-deps`          | Dependency array validation |
| `react-refresh/only-export-components` | HMR compatibility           |

### Lint Uitvoeren

```bash
npm run lint
```

Optie om automatisch op te lossen:

```bash
npm run lint -- --fix
```

---

## Dockerfile

**Doel**: Containerization van React app

### Stages

```dockerfile
FROM node:20-alpine
WORKDIR /app
COPY package*.json ./
RUN npm install
COPY . .
EXPOSE 5173 3030
CMD ["npm", "run", "dev"]
```

### Build Command

```bash
docker build -t netwerkscanner-dash .
```

### Run Command

```bash
docker run -p 5173:5173 -p 3030:3030 netwerkscanner-dash
```

### Image Size Optimalisatie

- `node:20-alpine` - Kleine Linux distro (170MB)
- Multi-stage build (optioneel voor production)

---

## 🐋 docker-compose.yml (Project Root)

**Doel**: Multi-container orchestration

### Services

#### mariadb

```yaml
image: mariadb:latest
ports:
  - "3306:3306"
volumes:
  - db_data:/var/lib/mysql
```

#### wifi_api

```yaml
build: ./software/wifi-api
depends_on:
  - mariadb
ports:
  - "8081:80"
```

#### network-scanner

```yaml
build:
  context: ./software/netwerkscanner-dash
volumes:
  - ./software/netwerkscanner-dash:/app
ports:
  - "80:5173"
```

#### upload-server

```yaml
build:
  context: ./software/netwerkscanner-dash
command: npm run upload-server
volumes:
  - ./software/assets/projectdata:/assets/projectdata
ports:
  - "3030:3030"
```

### Environment Variables

Create `.env` in project root:

```env
MYSQL_ROOT_PASSWORD=root_pass
MYSQL_DATABASE=netwerkscanner_db
MYSQL_USER=scanner_user
MYSQL_PASSWORD=user_pass
```

### Volumes

```yaml
volumes:
  db_data:           # MariaDB persistence
```

Mount paths:

```yaml
- ./software/assets/projectdata:/assets/projectdata
```

### Network

Alle services op hetzelfde Docker network:

- `mariadb:3306` - Database
- `wifi_api:8081` - WiFi API
- `network-scanner:5173` - Frontend
- `upload-server:3030` - Upload server

---

## .env File

**Bestand**: Project root

### Verplichte Variabelen

```env
# Database
MYSQL_ROOT_PASSWORD=secure_password_123
MYSQL_DATABASE=netwerkscanner_db
MYSQL_USER=scanner_user
MYSQL_PASSWORD=user_password_456

# API Endpoint (optioneel)
VITE_UPLOAD_SERVER=http://localhost:3030
VITE_API_URL=http://localhost:8081
```

### Development

```env
NODE_ENV=development
DEBUG=true
```

### Production

```env
NODE_ENV=production
DEBUG=false
```

### Niet committen

Add to `.gitignore`:

```txt
.env
.env.local
.env.*.local
```

---

## .gitignore

**Bestand**: Project root

### Standaard Entries

```txt
node_modules/
dist/
*.env
*.log
.DS_Store
.idea/
```

### Tailwind Generated

```.css
.next/
out/
build/
```

---

## index.html

**Doel**: HTML template

```html
<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <link rel="icon" type="image/svg+xml" href="/vite.svg" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Netwerkscanner Dashboard</title>
  </head>
  <body>
    <div id="root"></div>
    <script type="module" src="/src/main.jsx"></script>
  </body>
</html>
```

### Key Points

- Div `id="root"` - React mount point
- `<script>` tag laadt main.jsx
- Responsive viewport meta tag
- Titel wordt weergegeven in browser tab

---

## Build Optimization

### Production Build

```bash
npm run build
```

Genereert:

- `dist/` folder met geoptimaliseerde files
- Code splitting
- CSS minification
- Asset hashing

### File Structure After Build

```txt
dist/
├── index.html          # Gecompileerde HTML
├── assets/
│   ├── index-ABC123.js    # Main JS bundle (minified)
│   ├── style-XYZ789.css   # CSS bundle (minified)
│   └── logo-DEF456.svg    # Assets
```

### File Size Analysis

```bash
npm run build -- --analyze
```

---

## Development vs Production

| Aspect        | Development | Production      |
| ------------- | ----------- | --------------- |
| Build Speed   | Fast (2-3s) | Slower (10-15s) |
| Bundle Size   | Large       | Minimal         |
| Source Maps   | Full        | None            |
| HMR           | Yes         | No              |
| Optimizations | None        | Full            |

---

## Configuration Best Practices

**Wel doen**:

- Use environment variables voor sensible data
- Separate dev/prod configs
- Document alle env vars
- Use Docker for consistency

**Niet doen**:

- Hardcode credentials
- Commit .env files
- Use different versions per environment
- Ignore Docker warnings

---

**Voor meer info zie**: [README.md](./README.md)
