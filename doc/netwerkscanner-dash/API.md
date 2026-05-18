# API Integratie & Proxy Configuratie

---

## API Overzicht

Het Netwerkscanner Dashboard integreert met twee externe services:

### 1. WiFi API Server

- **Host**: http://10.20.10.24:8081
- **Functie**: WiFi scan data, netwerk informatie
- **Status**: Backend service (Flask/Python)

### 2. Upload Server

- **Host**: http://localhost:3030 (development) of http://upload-server:3030 (Docker)
- **Functie**: Bestandsuploads, project data opslag
- **Status**: Node.js Express server

---

## Proxy Configuration

Alle API requests worden geproxied via Vite dev server (`vite.config.js`):

### Upload Proxy

```javascript
'/upload': {
  target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
  changeOrigin: true
}
```

**Request Flow**:

```txt
Browser: POST /upload
   ↓
Vite Proxy (localhost:5173)
   ↓
Upload Server (localhost:3030)
```

### Projects Proxy

```javascript
'/projects': {
  target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
  changeOrigin: true
}
```

**Request Flow**:

```txt
Browser: GET /projects/central.json
   ↓
Vite Proxy
   ↓
Upload Server
```

### Assets Proxy

```javascript
'/assets/projectdata': {
  target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
  changeOrigin: true
}
```

**Request Flow**:

```txt
Browser: GET /assets/projectdata/image.jpg
   ↓
Vite Proxy
   ↓
Upload Server
```

### WiFi API Proxy

```javascript
'/api': {
  target: 'http://10.20.10.24:8081',
  changeOrigin: true,
  rewrite: (path) => path.replace(/^\/api/, '')
}
```

**Request Flow**:

```txt
Browser: GET /api/networks
   ↓
Vite Proxy
   ↓
WiFi API (rewrite to /networks)
```

---

## Upload Server API

### POST /upload

**Upload bestand**

```javascript
const uploadFile = async (file) => {
  const formData = new FormData()
  formData.append('file', file)
  
  const response = await axios.post('/upload', formData, {
    headers: { 'Content-Type': 'multipart/form-data' }
  })
  
  return response.data // { filename, url }
}
```

**Response**:
```json
{
  "filename": "1684923456789-plattegrond.jpg",
  "url": "/assets/projectdata/1684923456789-plattegrond.jpg"
}
```

---

### GET /projects/central.json

**Ophalen van project data**

```javascript
const loadProjects = async () => {
  const response = await axios.get('/projects/central.json')
  return response.data // Array of buildings
}
```

**Response**:

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
        "imageUrl": "/assets/projectdata/...",
        "accessPoints": [...]
      }
    ]
  }
]
```

---

### POST /projects/central.json

**Opslaan van project data**

```javascript
const saveProjects = async (buildingsData) => {
  const response = await axios.post('/projects/central.json', buildingsData)
  return response.data // { ok: true }
}
```

**Request Body**:

```json
[
  {
    "id": "building-1",
    "name": "Kantoor A",
    "floors": [...]
  }
]
```

**Response**:

```json
{
  "ok": true
}
```

---

## 📡 WiFi API Endpoints

*(Dit zijn voorbeelden - verificatie nodig met API docs)*

### GET /networks

**Beschikbare WiFi netwerken**

```javascript
const getNetworks = async () => {
  const response = await axios.get('/api/networks')
  return response.data
}
```

### GET /scan/:floorId

**WiFi scan voor verdieping**

```javascript
const scanFloor = async (floorId) => {
  const response = await axios.get(`/api/scan/${floorId}`)
  return response.data
}
```

### POST /scan/start

**Start WiFi scan operatie**

```javascript
const startScan = async (config) => {
  const response = await axios.post('/api/scan/start', {
    floorId: 'floor-1',
    duration: 30 // seconds
  })
  return response.data
}
```

---

## Axios Interceptors

*(Aanbevolen implementatie)*

```javascript
// src/api/client.js
import axios from 'axios'

const client = axios.create({
  timeout: 10000,
  headers: {
    'Content-Type': 'application/json'
  }
})

// Request interceptor
client.interceptors.request.use(
  config => {
    console.log('API Request:', config.url)
    return config
  },
  error => Promise.reject(error)
)

// Response interceptor
client.interceptors.response.use(
  response => response,
  error => {
    console.error('API Error:', error.message)
    // Custom error handling
    return Promise.reject(error)
  }
)

export default client
```

---

## Docker Environment Variables

In `docker-compose.yml`:

```yaml
network-scanner:
  environment:
    - VITE_UPLOAD_SERVER=http://upload-server:3030
    - VITE_API_URL=http://wifi_api:80
```

**In `.env.docker`**:

```env
VITE_UPLOAD_SERVER=http://upload-server:3030
VITE_API_URL=http://wifi_api:80
```

**Load in Vite config**:

```javascript
const env = loadEnv(mode, '.', '')
const uploadServer = env.VITE_UPLOAD_SERVER || 'http://localhost:3030'
```

---

## 🔍 Debugging API Calls

### Browser DevTools

1. Open DevTools (F12)
2. Network tab
3. Filter by XHR/Fetch
4. Inspecteer request/response headers

### Vite Debug Mode

```bash
DEBUG=vite:* npm run dev
```

### Log API Calls

```javascript
// In vite.config.js
server: {
  proxy: {
    '/upload': {
      target: 'http://localhost:3030',
      changeOrigin: true,
      logLevel: 'debug' // Show proxy logs
    }
  }
}
```

---

## Error Handling

### Common Errors

| Error      | Cause              | Fix                |
| ---------- | ------------------ | ------------------ |
| 404        | Endpoint not found | Check URL path     |
| 500        | Server error       | Check server logs  |
| CORS error | Different origin   | Check proxy config |
| Timeout    | Network slow       | Increase timeout   |

### Retry Logic

```javascript
const apiCall = async (fn, retries = 3) => {
  try {
    return await fn()
  } catch (error) {
    if (retries > 0) {
      await new Promise(r => setTimeout(r, 1000))
      return apiCall(fn, retries - 1)
    }
    throw error
  }
}

// Usage
const data = await apiCall(() => axios.get('/projects/central.json'))
```

---

## API Request Examples

### Upload Plattegrond

```javascript
const uploadFloorImage = async (file, buildingId, floorNumber) => {
  try {
    const formData = new FormData()
    formData.append('file', file)
    formData.append('building', buildingId)
    formData.append('floor', floorNumber)
    
    const response = await axios.post('/upload', formData)
    return response.data.url
  } catch (error) {
    console.error('Upload failed:', error)
    throw error
  }
}
```

### Load & Save Project

```javascript
const saveProject = async (buildingsData) => {
  try {
    // Save to server
    const saveResponse = await axios.post(
      '/projects/central.json',
      buildingsData
    )
    
    if (saveResponse.data.ok) {
      console.log('Project saved successfully')
      return true
    }
  } catch (error) {
    console.error('Save failed:', error)
    // Data is still in IndexedDB
    return false
  }
}

const loadProject = async () => {
  try {
    const response = await axios.get('/projects/central.json')
    return response.data || []
  } catch (error) {
    console.error('Load failed:', error)
    return []
  }
}
```

### Scan Network

```javascript
const scanFloor = async (floorId) => {
  try {
    const response = await axios.get(`/api/networks/${floorId}`)
    return response.data
  } catch (error) {
    console.error('Scan failed:', error)
    return []
  }
}
```

---

## 🔐 Authentication

*(Toekomstige implementatie)*

```javascript
// Add auth token to requests
client.interceptors.request.use(config => {
  const token = localStorage.getItem('authToken')
  if (token) {
    config.headers.Authorization = `Bearer ${token}`
  }
  return config
})

// Handle 401 responses
client.interceptors.response.use(
  response => response,
  async error => {
    if (error.response?.status === 401) {
      // Token expired - refresh or redirect to login
      localStorage.removeItem('authToken')
      window.location.href = '/login'
    }
    return Promise.reject(error)
  }
)
```

---

## Performance Tips

### Caching

```javascript
// Cache API responses in memory
const cache = new Map()

const getCached = async (key, fn) => {
  if (cache.has(key)) {
    return cache.get(key)
  }
  const data = await fn()
  cache.set(key, data)
  return data
}

// Usage
const projects = await getCached('projects', () => 
  axios.get('/projects/central.json')
)
```

### Request Deduplication

```javascript
const pendingRequests = new Map()

const dedupedRequest = async (key, fn) => {
  if (pendingRequests.has(key)) {
    return pendingRequests.get(key)
  }
  
  const promise = fn()
  pendingRequests.set(key, promise)
  
  try {
    return await promise
  } finally {
    pendingRequests.delete(key)
  }
}
```

---

## API Documentation

Zie de volgende resources voor volledige API documentatie:

- **Upload Server**: [FILE_SERVER.md](./FILE_SERVER.md)
- **WiFi API**: Contact API owner voor docs
- **Vite Proxy**: [CONFIG.md](./CONFIG.md#vite-configjs)

---

**Voor meer info zie**: [README.md](./README.md)
