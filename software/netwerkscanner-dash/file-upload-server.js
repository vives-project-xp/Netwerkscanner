import express from 'express';
import multer from 'multer';
import path from 'path';
import fs from 'fs';
import cors from 'cors';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const app = express();
const PORT = 3030;

const UPLOAD_DIR = path.join(__dirname, '../assets/projectdata');
const PROJECTS_FILE = path.join(UPLOAD_DIR, 'central.json');
if (!fs.existsSync(UPLOAD_DIR)) {
  fs.mkdirSync(UPLOAD_DIR, { recursive: true });
}
if (!fs.existsSync(PROJECTS_FILE)) {
  fs.writeFileSync(PROJECTS_FILE, '[]', 'utf-8');
}

app.use(cors());
app.use(express.json({ limit: '10mb' }));
app.use('/assets/projectdata', express.static(UPLOAD_DIR));

const storage = multer.diskStorage({
  destination: (req, file, cb) => {
    cb(null, UPLOAD_DIR);
  },
  filename: (req, file, cb) => {
    const uniqueSuffix = Date.now() + '-' + Math.round(Math.random() * 1E9);
    cb(null, uniqueSuffix + '-' + file.originalname);
  }
});
const upload = multer({ storage });

app.post('/upload', upload.single('file'), (req, res) => {
  if (!req.file) return res.status(400).json({ error: 'No file uploaded' });
  res.json({
    filename: req.file.filename,
    url: `/assets/projectdata/${req.file.filename}`
  });
});

app.get('/projects/central.json', (req, res) => {
  try {
    const raw = fs.readFileSync(PROJECTS_FILE, 'utf-8');
    const parsed = JSON.parse(raw || '[]');
    if (!Array.isArray(parsed)) {
      return res.status(200).json([]);
    }
    return res.status(200).json(parsed);
  } catch (error) {
    return res.status(200).json([]);
  }
});

app.post('/projects/central.json', (req, res) => {
  try {
    const payload = Array.isArray(req.body) ? req.body : [];
    fs.writeFileSync(PROJECTS_FILE, JSON.stringify(payload, null, 2), 'utf-8');
    return res.status(200).json({ ok: true });
  } catch (error) {
    return res.status(500).json({ ok: false, error: 'Save failed' });
  }
});

app.listen(PORT, () => {
  console.log(`File upload server running on http://localhost:${PORT}`);
});
