import React from "react";
import "../App.css";

export default function FloorSelector({ buildings, selectedBuildingId, setSelectedBuildingId, selectedFloorId, setSelectedFloorId }) {
  const currentBuilding = buildings.find(b => String(b.id) === String(selectedBuildingId));
  return (
    <div className="floor-selector">
      <label className="floor-selector-label">Floor Selection</label>
      <select
        value={selectedBuildingId}
        onChange={e => { setSelectedBuildingId(e.target.value); setSelectedFloorId(""); }}
        className="floor-selector-dropdown"
      >
        <option value="">Choose Project...</option>
        {buildings.map(b => (
          <option key={b.id} value={b.id}>{b.name}</option>
        ))}
      </select>
      <select
        disabled={!selectedBuildingId}
        value={selectedFloorId}
        onChange={e => setSelectedFloorId(e.target.value)}
        className="floor-selector-dropdown"
      >
        <option value="">Choose Floor...</option>
        {currentBuilding?.floors?.map(f => (
          <option key={f.id} value={f.id}>{f.name}</option>
        ))}
      </select>
    </div>
  );
}
