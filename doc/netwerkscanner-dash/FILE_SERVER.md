# File Upload Server - API Documentatie

**Bestand**: `file-upload-server.js`

---

## Overzicht

Een **Express.js server** die twee kernfunctionaliteiten biedt:

1. **Bestandsuploads** - Plattegrondafbeeldingen en project media
2. **Project Data Management** - Opslag en ophalen van project configuraties

---

## Configuratie

### Server Details

- **Port**: 3030
- **Host**: 0.0.0.0 (all interfaces)
- **Upload Map**: `../assets/projectdata`
- **Project File**: `../assets/projectdata/central.json`

### Environment

```javascript
UPLOAD_DIR = './software/assets/projectdata'
PROJECTS_FILE = './software/assets/projectdata/central.json'
```

### Startup

```bash
node file-upload-server.js
```

Of via npm:

```bash
npm run upload-server
```

---

## API Endpoints

### 1. Bestandsupload

**Endpoint**: `POST /upload`

**Content-Type**: `multipart/form-data`

**Request**:

```bash
curl -X POST \
  -F "file=@plattegrond.jpg" \
  http://localhost:3030/upload
```

**Response** (200 OK):

```json
{
  "filename": "1234567890-plattegrond.jpg",
  "url": "/assets/projectdata/1234567890-plattegrond.jpg"
}
```

**Error Response** (400):

```json
{
  "error": "No file uploaded"
}
```

**Details**:

- Maximale bestandsgrootte: 10 MB (kan aangepast in `express.json()`)
- Bestandsnaam: `{timestamp}-{originalname}`
- Bestand opgeslagen in: `/assets/projectdata/`

---

### 2. Project Data Ophalen

**Endpoint**: `GET /projects/central.json`

**Response** (200 OK):

```json
[
  {
    "id": "building-1",
    "name": "Kantoor A",
    "address": "Straat 123",
    "floors": [
      {
        "id": "floor-1",
        "number": 1,
        "name": "Begane grond",
        "imageUrl": "/assets/projectdata/1234567890-plattegrond.jpg",
        "accessPoints": [
          {
            "id": "ap-1",
            "bssid": "00:11:22:33:44:55",
            "rssi": -45,
            "x": 150,
            "y": 200
          }
        ]
      }
    ]
  }
]
```

**Details**:

- Retourneert altijd een array
- Als bestand niet bestaat, retourneert lege array
- Als bestand corrupt is, retourneert lege array (geen error)

---

### 3. Project Data Opslaan

**Endpoint**: `POST /projects/central.json`

**Content-Type**: `application/json`

**Request**:

```json
[
  {
    "id": "building-1",
    "name": "Kantoor A",
    "floors": []
  }
]
```

**Response** (200 OK):

```json
{
  "ok": true
}
```

**Error Response** (500):

```json
{
  "ok": false,
  "error": "Save failed"
}
```

**Details**:

- Payload moet array zijn
- Pretty-printed JSON (2 spaties indent)
- Overschrijft bestaand bestand volledig
- Synchrone operatie (blokkering)

---

## File Upload Verwerking

### Upload Map Structuur

```txt
assets/projectdata/
├── central.json              # Hoofd project data
├── 1234567890-plan1.jpg
├── 1234567890-plan2.png
└── 1234567890-netwerk.svg
```

### Bestandsnaamgeneratie

```javascript
const uniqueSuffix = Date.now() + '-' + Math.round(Math.random() * 1E9);
const filename = uniqueSuffix + '-' + originalname;
// Voorbeeld: 1684923456789-0123456789-plattegrond.jpg
```

### Originalename behouden

- Originele bestandsnaam blijft behouden
- Unieke prefix voorkomt overwrite
- Ondersteunt alle bestandstypes (jpg, png, svg, etc.)

---

## Security Features

### CORS

```javascript
app.use(cors());  // Allow all origins
```

- Alle cross-origin requests toegestaan
- Production: Beperk tot specifieke domains

### Payload Limits

```javascript
express.json({ limit: '10mb' })  // Max JSON size
```

- Verhoog indien nodig voor grotere projecten
- Default multer: unlimited

### Bestandsvalidatie

```javascript
if (!req.file) return res.status(400).json({ error: 'No file uploaded' })
```

- Controleert bestandsexistentie
- Retourneert 400 Bad Request als missing

---

## 🔄 Data Workflow

### Upload Flow

```txt
Browser Form
    ↓
POST /upload (multipart)
    ↓
Multer Middleware
    ↓
File geschreven naar disk
    ↓
Response met filename
```

### Project Save Flow

```txt
React App
    ↓
POST /projects/central.json
    ↓
JSON Validatie
    ↓
fs.writeFileSync()
    ↓
Response { ok: true }
```

### Project Load Flow

```txt
Browser
    ↓
GET /projects/central.json
    ↓
fs.readFileSync()
    ↓
JSON Parse & Validate
    ↓
Response Array
```

---

## Integratie met Vite

In `vite.config.js`:

```javascript
'/upload': {
  target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
  changeOrigin: true
},
'/projects': {
  target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
  changeOrigin: true
},
'/assets/projectdata': {
  target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
  changeOrigin: true
}
```

**ProxyMapping**:

- `/upload` → `http://localhost:3030/upload`
- `/projects` → `http://localhost:3030/projects`
- `/assets/projectdata` → `http://localhost:3030/assets/projectdata`

---

## Voorbeeld React Integratie

### Bestand Uploaden

```jsx
const uploadFile = async (file) => {
  const formData = new FormData();
  formData.append('file', file);
  
  const response = await axios.post('/upload', formData, {
    headers: { 'Content-Type': 'multipart/form-data' }
  });
  
  return response.data.url;
};
```

### Project Laden

```jsx
const loadProject = async () => {
  const response = await axios.get('/projects/central.json');
  return response.data;
};
```

### Project Opslaan

```jsx
const saveProject = async (buildingsData) => {
  const response = await axios.post('/projects/central.json', buildingsData);
  return response.data.ok;
};
```

---

## Error Handling

| Situatie              | Status | Response                              |
| --------------------- | ------ | ------------------------------------- |
| Geen bestand geupload | 400    | `{ error: "No file uploaded" }`       |
| Corrupte JSON         | 200    | `[]` (lege array)                     |
| File save failure     | 500    | `{ ok: false, error: "Save failed" }` |
| Ongeldige payload     | 200    | `[]` (ignored)                        |

**Note**: GET request kan geen echte 404 geven - altijd 200 met array

---

## Performance Tips

### Bestandsgrootte

- Comprimeer afbeeldingen vooraf
- Gebruik WebP voor moderne browsers
- Max 10MB per file (configureerbaar)

### Database Schaling

- `central.json` kan groot worden (>50MB)
- Voor productie: Migrate naar SQLite of PostgreSQL
- Implementeer pagination voor grote datasets

### Server Optimization

```javascript
// Momenteel: Sync file operations
// Better: Async operaties
const response = await fs.promises.readFile(...)
const response = await fs.promises.writeFile(...)
```

---

## Docker Deployment

In `docker-compose.yml`:

```yaml
upload-server:
  build:
    context: ./software/netwerkscanner-dash
  container_name: upload-server
  command: npm run upload-server
  volumes:
    - ./software/assets/projectdata:/assets/projectdata
  ports:
    - "3030:3030"
```

**Volume Mount**: Persistente opslag van uploads

---

## Backup & Recovery

### Backup van Projects

```bash
# Backup central.json
cp software/assets/projectdata/central.json backup.json
```

### Restore van Backup

```bash
# Restore central.json
cp backup.json software/assets/projectdata/central.json
```

### Uploaded Files

Alle geupload bestanden in `software/assets/projectdata/` zijn persistent via Docker volume.

---

## Toekomstige Verbeteringen

- [ ] Async/await refactoring
- [ ] SQLite database ipv JSON
- [ ] File versioning
- [ ] Bestandsvalidatie (type checking)
- [ ] Compression algoritmes
- [ ] Rate limiting
- [ ] Authentication/Authorization
- [ ] File deletion endpoints

---

**Voor meer info zie**: [README.md](./README.md)
