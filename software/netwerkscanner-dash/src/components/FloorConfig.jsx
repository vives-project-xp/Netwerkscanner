import React from "react";
import { Cpu } from "lucide-react";
import "../App.css";

export default function FloorConfig({ selectedFloorId, linkedDeviceId, scans, setFloorScannerMapping }) {
  return (
    <div className="floor-config">
      <h3 className="floor-config-title"><Cpu size={14}/> ESP Link</h3>
      <div className="floor-config-select-wrapper">
        <select
          value={linkedDeviceId}
          onChange={e => setFloorScannerMapping(prev => ({ ...prev, [selectedFloorId]: e.target.value }))}
          className="floor-config-select"
        >
          <option value="">-- No ESP selected --</option>
          {[...new Set(scans.map(s => s.device_id))].filter(Boolean).map(id => (
            <option key={id} value={id}>{id}</option>
          ))}
        </select>
      </div>
    </div>
  );
}
