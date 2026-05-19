# REST API overzicht

```mermaid
flowchart LR
  esp[ESP32 / Netwerkscanner] -->|GET /time| api[Flask REST API\nsoftware/wifi-api/api/app.py]
  esp -->|POST /upload| api

  ui[Dashboard / React app] -->|GET /api/heatmap| vite[Vite proxy\nsoftware/netwerkscanner-dash/vite.config.js]
  ui -->|GET/POST /projects/central.json| upload[Upload server\nsoftware/netwerkscanner-dash/file-upload-server.js]
  ui -->|POST /upload| upload

  vite -->|rewrite /api -> /| api
  upload -->|JSON + files| json[(software/assets/projectdata/central.json\n+ uploaded images)]

  api -->|MariaDB| db[(MariaDB\nheatmap, fingerprint, dev)]

  api -->|GET /heatmap| db
  api -->|GET /fingerprint| db
  api -->|GET /dev| db
  api -->|GET /predict/<id>| db
```

## Kort

- De hoofd-REST-API is de Flask-service in `software/wifi-api/api/app.py`.
- De dashboard gebruikt Vite als proxy voor `/api/*`.
- Gebouw- en vloerdata worden opgeslagen via de uploadserver in JSON en uploads in `software/assets/projectdata`.
- Scan- en fingerprintdata gaan naar MariaDB.