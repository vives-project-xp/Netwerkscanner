import { useState, useEffect } from 'react';
import { get, set } from 'idb-keyval';

export function useBuildings() {
  const [buildings, setBuildings] = useState([]);
  const [isLoaded, setIsLoaded] = useState(false);

  // Initialiseer: haal data op uit IndexedDB
  useEffect(() => {
    async function init() {
      try {
        const saved = await get('netwerkscanner_projects');
        if (saved) setBuildings(saved);
      } catch (err) {
        console.error("Fout bij laden database:", err);
      } finally {
        setIsLoaded(true);
      }
    }
    init();
  }, []);

  // Sync: sla op zodra buildings verandert
  useEffect(() => {
    if (isLoaded) {
      set('netwerkscanner_projects', buildings);
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