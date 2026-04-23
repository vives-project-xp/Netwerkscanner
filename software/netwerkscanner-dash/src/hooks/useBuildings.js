
import { useState, useEffect } from 'react';


export function useBuildings() {
  const [buildings, setBuildings] = useState([]);
  const [isLoaded, setIsLoaded] = useState(false);
  const PROJECTS_PATH = '/projects/central.json';


  // Initialiseer: haal data op uit public/projects/central.json
  useEffect(() => {
    async function init() {
      try {
        const res = await fetch(PROJECTS_PATH);
        if (res.ok) {
          const saved = await res.json();
          const normalized = Array.isArray(saved)
            ? saved.map((b) => ({
                ...b,
                floors: Array.isArray(b.floors) ? b.floors : []
              }))
            : [];
          setBuildings(normalized);
        }
      } catch (err) {
        console.error("Fout bij laden projecten:", err);
      } finally {
        setIsLoaded(true);
      }
    }
    init();
  }, []);


  // Sync: sla op zodra buildings verandert
  useEffect(() => {
    if (isLoaded) {
      // Sla centraal op via een POST naar een endpoint of upload naar public/projects/central.json
      fetch(PROJECTS_PATH, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(buildings)
      })
        .then((res) => {
          if (!res.ok) {
            throw new Error(`HTTP ${res.status}`);
          }
        })
        .catch(e => console.error('Fout bij opslaan projecten:', e));
    }
  }, [buildings, isLoaded]);


  // Alternatieve unieke ID generator
  function generateId() {
    return (
      Date.now().toString(36) +
      Math.random().toString(36).substr(2, 9)
    );
  }


  const addBuilding = (name, location) => {
    const newB = { id: generateId(), name, location, floors: [] };
    setBuildings([...buildings, newB]);
  };


  const deleteBuilding = (id) => {
    if (window.confirm("Gebouw verwijderen?")) {
      setBuildings(buildings.filter(b => b.id !== id));
    }
  };


  const addFloor = (buildingId, floorName, image) => {
    setBuildings(prev => prev.map(b => {
      if (b.id === buildingId) {
        return {
          ...b,
          floors: [...b.floors, { id: generateId(), name: floorName, image }]
        };
      }
      return b;
    }));
  };


  const deleteFloor = (buildingId, floorId) => {
    setBuildings(prev => prev.map(b => {
      if (b.id === buildingId) {
        return { ...b, floors: b.floors.filter(f => f.id !== floorId) };
      }
      return b;
    }));
  };

  return { buildings, addBuilding, deleteBuilding, addFloor, deleteFloor, isLoaded };
}