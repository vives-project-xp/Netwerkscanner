# React Componenten - Gedetailleerde Gids

## 📑 Inhoudsopgave
1. [App.jsx](#appjsx)
2. [Pages](#pages)
3. [Components](#components)
4. [Hooks](#hooks)

---

## App.jsx

**Locatie**: `src/App.jsx`

### Doel
Hoofdtoepassing met:
- Tab-navigatie (Dashboard / Gebouwen)
- Gebouw selectie management
- Loading state handling
- Responsive layout met sidebar

### Structuur

```jsx
export default function App()
```

**Props**: Geen (root component)

**State**:
```javascript
activeTab          // 'dashboard' of 'gebouwen'
selectedBuildingId // ID van geselecteerd gebouw of null
buildings          // Array van alle gebouwen
isLoaded           // Boolean: data geladen?
```

**Key Features**:
- Laadscherm met spinner terwijl database initialiseert
- Adaptive navigation (sidebar verborgen op mobiel)
- Dynamische pagina rendering op basis van activeTab
- Building detail view met terug-navigatie

### Component Hierarchie
```
App
├── Sidebar (links menu)
├── Dashboard (tab)
├── Buildings (tab)
└── BuildingDetail (sub-view)
```

---

## Pages

### Dashboard.jsx
**Locatie**: `src/pages/Dashboard.jsx`

**Doel**: Startpagina met overzichten en statistieken

**Props**:
- `buildings` (array) - Alle beschikbare gebouwen

**Weergave**:
- Statistieken (aantal gebouwen, verdiepingen, etc.)
- Recente activiteiten
- Snelle koppelingen naar functies

---

### Buildings.jsx
**Locatie**: `src/pages/Buildings.jsx`

**Doel**: Beheer van gebouwen (CRUD operaties)

**Props**:
```javascript
buildings        // Array van gebouwen
onAddBuilding    // Callback: (buildingData) => void
onDeleteBuilding // Callback: (buildingId) => void
onSelectBuilding // Callback: (building) => void
```

**Functionaliteit**:
- Lijst alle gebouwen op
- Voeg nieuw gebouw toe (formulier)
- Verwijder gebouw (met bevestiging)
- Selecteer gebouw voor detail view

**UI Elementen**:
- Gebouw kaarten met preview
- "Nieuw Gebouw" knop
- Verwijder knop per gebouw

---

### BuildingDetail.jsx
**Locatie**: `src/pages/BuildingDetail.jsx`

**Doel**: Detail weergave van één gebouw met verdiepingen

**Props**:
```javascript
building        // Object met gebouwgegevens
onBack          // Callback: () => void (terug naar list)
onAddFloor      // Callback: (buildingId, floorData) => void
onDeleteFloor   // Callback: (buildingId, floorId) => void
```

**Functionaliteit**:
- Toon gebouwdetails
- Beheer verdiepingen (toevoegen/verwijderen)
- Toon verdiepingskaart met plattegrond
- Plaats en beheer access points
- Visualiseer RSSI-gegevens

**Subcomponenten**:
- `FloorSelector` - Verdiepingkeuze
- `FloorConfig` - Verdiepingsconfiguratie
- `AccessPointList` - Lijst van access points
- `AccessPointDrawer` - Toevoegen/bewerken punten

---

## Components

### Sidebar.jsx
**Locatie**: `src/components/Sidebar.jsx`

**Doel**: Navigatie menu met gebouwen lijst

**Props**:
```javascript
buildings       // Array van gebouwen
activeTab       // Huidige actieve tab
onTabChange     // Callback: (tabName) => void
onSelectBuilding // Callback: (buildingId) => void
```

**Functionaliteit**:
- Toon navigatie tabs (Dashboard, Gebouwen)
- Snelle links naar alle gebouwen
- Highlight huiding tab
- Responsive (verborgen op kleine schermen)

---

### FloorSelector.jsx
**Locatie**: `src/components/FloorSelector.jsx`

**Doel**: Selectie van verdieping in een gebouw

**Props**:
```javascript
building      // Gebouw object met verdiepingen
onFloorSelect // Callback: (floorId) => void
selectedFloor // Momenteel geselecteerde verdieping
```

**Functionaliteit**:
- Toon alle verdiepingen van gebouw
- Selecteer actieve verdieping
- Toon verdiepingsnummer en naam

---

### FloorConfig.jsx
**Locatie**: `src/components/FloorConfig.jsx`

**Doel**: Configuratie van verdieping (plattegrond, access points)

**Props**:
```javascript
floor        // Verdiepingsobject
building     // Gebouw object (context)
onAddFloor   // Callback: (data) => void
onDeleteFloor // Callback: (floorId) => void
```

**Functionaliteit**:
- Toon en wijzig verdiepingsinformatie
- Upload plattegrond afbeelding
- Toon huiding plattegrond
- Voeg/verwijder verdiepingen toe

**Subcomponenten**:
- Afbeelding uploader
- Afbeelding preview

---

### AccessPointList.jsx
**Locatie**: `src/components/AccessPointList.jsx`

**Doel**: Toont alle access points op geselecteerde verdieping

**Props**:
```javascript
accessPoints  // Array van AP objecten
onDelete      // Callback: (apId) => void
onEdit        // Callback: (apId, newData) => void
selectedAP    // Huiding geselecteerde AP
```

**Functionaliteit**:
- Lijst access points in tabel
- Toon BSSID, RSSI, positie
- Verwijder optie per AP
- Toon RSSI visueel (kleurkoding)

---

### AccessPointDrawer.jsx
**Locatie**: `src/components/AccessPointDrawer.jsx`

**Doel**: Interactieve kaart voor placement van access points

**Props**:
```javascript
floorImage   // URL van plattegrondafbeelding
onAddPoint   // Callback: (position, data) => void
existingPoints // Array van bestaande points
```

**Functionaliteit**:
- Toon zoombare/panbare plattegrond
- Klik voor nieuw access point
- Invoerveld voor BSSID en RSSI
- Visueel feedback bij plaatsing
- Verwijder points met klik

**Interactie**:
- Mouse wheel: zoom
- Klik en sleep: pan
- Dubbel klik: point plaatsen

---

## Hooks

### useBuildings()
**Locatie**: `src/hooks/useBuildings.js`

**Doel**: Centraal gebouwen data management

**Returns**:
```javascript
{
  buildings,        // Array
  addBuilding,      // Function
  deleteBuilding,   // Function
  addFloor,         // Function
  deleteFloor,      // Function
  isLoaded         // Boolean
}
```

**Functionaliteit**:
- Load gebouwen van IndexedDB
- CRUD operaties op gebouwen
- CRUD operaties op verdiepingen
- Persisteer naar IndexedDB
- Sync met upload-server

**Voorbeeld Gebruik**:
```jsx
const { buildings, addBuilding } = useBuildings();

const handleAddBuilding = (data) => {
  addBuilding({
    name: data.name,
    address: data.address
  });
};
```

---

## Data Flow

```
App.jsx
  ↓
useBuildings() hook
  ↓
IndexedDB (client-side cache)
  ↓
file-upload-server.js
  ↓
FileSystem
```

---

## Styling

Alle componenten gebruiken **Tailwind CSS**:

```javascript
// Voorbeeld: Knopkleur
className="bg-blue-600 hover:bg-blue-700 text-white"
```

**Globale Thema**:
- Donkerblauw achtergrond: `#0f172a`
- Accent kleur: `#3b82f6` (blauw)
- Tekst: Slate grijs tinten

---

## Best Practices

✅ **Wel doen**:
- Props validatie met PropTypes
- Callback functies als props
- Logische component splitsing
- IndexedDB voor persistentie

❌ **Niet doen**:
- Globale state (wanneer hooks beter zijn)
- Directe DOM manipulatie
- Zware computaties in render

---

## Uitbreiding

Om een nieuw component toe te voegen:

1. Maak bestand in `src/components/ComponentName.jsx`
2. Definieer props en doel
3. Importeer in parent component
4. Voeg PropTypes toe (begin van bestand)
5. Style met Tailwind classes

Voorbeeld template:

```jsx
import PropTypes from 'prop-types';

export default function MyComponent({ prop1, prop2, onAction }) {
  return (
    <div className="p-4 bg-white rounded-lg">
      {/* content */}
    </div>
  );
}

MyComponent.propTypes = {
  prop1: PropTypes.string.isRequired,
  prop2: PropTypes.number,
  onAction: PropTypes.func.isRequired
};
```

---

**Voor meer info zie**: [README.md](./README.md)
