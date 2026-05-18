# Hooks - Custom React Logic

**Locatie**: `src/hooks/`

---

## useBuildings Hook

**Bestand**: `src/hooks/useBuildings.js`

### Doel

Centraal beheer van gebouw data met:
- Laden van IndexedDB
- CRUD operaties
- Synchronisatie met upload server
- Loading state

### Signature

```javascript
function useBuildings() {
  return {
    buildings: Building[],
    addBuilding: (data) => void,
    deleteBuilding: (id) => void,
    addFloor: (buildingId, data) => void,
    deleteFloor: (buildingId, floorId) => void,
    isLoaded: boolean
  };
}
```

### Return Object

```javascript
{
  buildings: [
    {
      id: "building-1",
      name: "Kantoor A",
      address: "Straat 123",
      floors: [
        {
          id: "floor-1",
          number: 1,
          name: "Begane grond",
          imageUrl: "/assets/projectdata/image.jpg",
          accessPoints: [
            {
              id: "ap-1",
              bssid: "00:11:22:33:44:55",
              rssi: -45,
              x: 150,
              y: 200
            }
          ]
        }
      ]
    }
  ],
  isLoaded: true
}
```

### Functies

#### addBuilding(data)

```javascript
const { addBuilding } = useBuildings();

addBuilding({
  name: "Kantoor B",
  address: "Laan 456"
});
```

**Workflow**:
1. Genereer unieke ID
2. Voeg toe aan local state
3. Opslaan in IndexedDB
4. Synchroniseer met server

#### deleteBuilding(id)

```javascript
deleteBuilding("building-1");
```

**Workflow**:
1. Verwijder uit state
2. Verwijder uit IndexedDB
3. Update server

#### addFloor(buildingId, data)

```javascript
addFloor("building-1", {
  number: 2,
  name: "Eerste verdieping"
});
```

**Workflow**:
1. Vind gebouw
2. Voeg verdieping toe
3. Persisteer in IndexedDB
4. Synchroniseer

#### deleteFloor(buildingId, floorId)

```javascript
deleteFloor("building-1", "floor-2");
```

**Workflow**:
1. Vind en verwijder verdieping
2. Update state
3. Persisteer

### IndexedDB Integration

**Database**: `netwerkscanner_db`
**Store**: `buildings`

```javascript
// Internal operations
idbKeyval.get('buildings')      // Ophalen
idbKeyval.set('buildings', data) // Opslaan
```

### Server Synchronisatie

**Endpoints**:
- `GET /projects/central.json` - Laden
- `POST /projects/central.json` - Opslaan

**Auto-sync**:
- Op initialization laden
- Na elke CRUD operatie opslaan
- Error handling met fallback

### Voorbeeld Gebruik

```jsx
import { useBuildings } from './hooks/useBuildings';

export default function MyComponent() {
  const { buildings, addBuilding, isLoaded } = useBuildings();

  if (!isLoaded) return <div>Laden...</div>;

  return (
    <div>
      <h1>{buildings.length} gebouwen</h1>
      <button onClick={() => addBuilding({ name: 'Nieuw' })}>
        Toevoegen
      </button>
    </div>
  );
}
```

---

## Hook Architecture

### State Management Flow

```
useBuildings Hook
    ↓
useState(buildings)
    ↓
useEffect (initialization)
    ↓
Load from IndexedDB
    ↓
Load from Server (if sync)
    ↓
Update State
```

### Lifecycle

**Mount**:
1. Initialiseer lege state
2. Set `isLoaded = false`
3. Load van IndexedDB
4. Load van server
5. Merge data
6. Set `isLoaded = true`

**Update**:
1. Muteer local state
2. Opslaan in IndexedDB
3. Synchroniseer met server
4. Trigger re-render

**Unmount**:
- Automatische cleanup
- Alle subscriptions verwijderd

---

## Data Persistentie

### IndexedDB vs Server

| Aspect | IndexedDB | Server |
|--------|-----------|--------|
| Locatie | Browser | Disk |
| Speed | Instant | Network latency |
| Sync | Manueel | Bij save |
| Offline | Ja | Nee |

### Sync Strategy

```javascript
// Load phase
const local = await indexedDB.get()
const remote = await fetch('/projects/central.json')
const merged = mergeData(local, remote)

// Save phase
await indexedDB.set(data)
await fetch('/projects/central.json', { method: 'POST', body: data })
```

---

## Error Handling

### Try-Catch Patterns

```javascript
try {
  const data = await idbKeyval.get('buildings')
  if (!data) {
    // Initialize new
  }
} catch (error) {
  console.error('IndexedDB error:', error)
  // Fallback to server
}
```

### Network Errors

```javascript
try {
  await axios.post('/projects/central.json', data)
} catch (error) {
  // Data still in IndexedDB - not lost
  console.error('Server save failed')
}
```

---

## Performance Optimizations

### Memoization

```javascript
const buildings = useMemo(() => {
  return rawBuildings.filter(...)
}, [rawBuildings])
```

### Lazy Loading

Gebouwen worden lazily geladen:
- Index eerst
- Details on demand
- Access points op interactie

### Batch Operations

```javascript
// Bad - multiple saves
addBuilding(data1)
addBuilding(data2)
addBuilding(data3)

// Good - batch update
addBuildingBatch([data1, data2, data3])
```

---

## Testing Hook

### Unit Test Voorbeeld

```javascript
import { renderHook, act } from '@testing-library/react'
import { useBuildings } from './useBuildings'

test('adds building', () => {
  const { result } = renderHook(() => useBuildings())
  
  act(() => {
    result.current.addBuilding({ name: 'Test' })
  })
  
  expect(result.current.buildings).toHaveLength(1)
})
```

### Integration Test

```javascript
test('saves and loads building', async () => {
  const { result } = renderHook(() => useBuildings())
  
  await waitFor(() => {
    expect(result.current.isLoaded).toBe(true)
  })
  
  act(() => {
    result.current.addBuilding({ name: 'Test' })
  })
  
  // Verify in IndexedDB
  const saved = await idbKeyval.get('buildings')
  expect(saved).toBeDefined()
})
```

---

## Advanced Patterns

### Custom Hook Composition

```javascript
function useBuildingWithFloors(buildingId) {
  const { buildings } = useBuildings()
  return useMemo(() => {
    return buildings.find(b => b.id === buildingId)
  }, [buildings, buildingId])
}
```

### State Reducer Pattern

```javascript
function useReducerBuildings() {
  const [state, dispatch] = useReducer(buildingReducer, initialState)
  
  return {
    buildings: state.buildings,
    dispatch
  }
}
```

### Custom Hook with Args

```javascript
function useBuilding(buildingId) {
  const { buildings } = useBuildings()
  return buildings.find(b => b.id === buildingId)
}

// Usage
const building = useBuilding('building-1')
```

---

## Common Issues & Solutions

### Hook Called Conditionally

❌ **Wrong**:
```javascript
if (someCondition) {
  const { buildings } = useBuildings()
}
```

✅ **Correct**:
```javascript
const { buildings } = useBuildings()
if (someCondition) {
  // use buildings
}
```

### IndexedDB Quota

**Problem**: Storage vul vol
**Solution**: 
```javascript
// Check quota
navigator.storage?.estimate()

// Clear old data
idbKeyval.delete('old_key')
```

### Stale Closures

❌ **Wrong**:
```javascript
const x = buildings
setTimeout(() => {
  console.log(x) // May be stale
}, 1000)
```

✅ **Correct**:
```javascript
useEffect(() => {
  // Re-run when buildings change
}, [buildings])
```

---

## Migration Guide

### From Props Drilling

**Before**:
```javascript
<Parent>
  <Child buildings={buildings}>
    <Grandchild buildings={buildings} />
  </Child>
</Parent>
```

**After**:
```javascript
function Child() {
  const { buildings } = useBuildings()
  return <Grandchild />
}

function Grandchild() {
  const { buildings } = useBuildings()
  return ...
}
```

---

## Future Improvements

- [ ] Add TypeScript definitions
- [ ] Implement optimistic updates
- [ ] Add offline sync queue
- [ ] Implement cache invalidation
- [ ] Add subscription system
- [ ] Performance metrics tracking

---

**Voor meer info zie**: [README.md](./README.md)
