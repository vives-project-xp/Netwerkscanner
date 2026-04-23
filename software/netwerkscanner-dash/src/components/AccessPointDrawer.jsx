import React from "react";
import { X, Plus, Trash2 } from "lucide-react";
import "../App.css";

export default function AccessPointDrawer({ editingAp, setEditingAp, setAccessPoints, scans, isOpen }) {
  if (!isOpen || !editingAp) return null;
  return (
    <div className="ap-drawer">
      <div className="ap-drawer-header">
        <h3 className="ap-drawer-title">Point Settings</h3>
        <button onClick={() => setEditingAp(null)} className="ap-drawer-close"><X size={24}/></button>
      </div>
      <div className="ap-drawer-content">
        <input
          type="text"
          value={editingAp.name}
          onChange={e => {
            const val = e.target.value;
            setAccessPoints(prev => prev.map(ap => ap.id === editingAp.id ? { ...ap, name: val } : ap));
            setEditingAp(prev => ({ ...prev, name: val }));
          }}
          className="ap-drawer-input"
        />
        <div className="ap-drawer-networks">
          <label className="ap-drawer-label">Assign Networks</label>
          <div className="ap-drawer-network-list">
            {scans.length > 0 ? [...new Map(scans.map(s => [s.bssid, s])).values()].map(signal => {
              const isSelected = editingAp.assignedBssids.includes(signal.bssid);
              return (
                <div
                  key={signal.bssid}
                  onClick={() => {
                    const next = isSelected
                      ? editingAp.assignedBssids.filter(b => b !== signal.bssid)
                      : [...editingAp.assignedBssids, signal.bssid];
                    setAccessPoints(prev => prev.map(ap => ap.id === editingAp.id ? { ...ap, assignedBssids: next } : ap));
                    setEditingAp(prev => ({ ...prev, assignedBssids: next }));
                  }}
                  className={`ap-drawer-network-item ${isSelected ? "selected" : ""}`}
                >
                  <div>
                    <div className="ap-drawer-network-ssid">{signal.ssid}</div>
                    <div className="ap-drawer-network-bssid">{signal.bssid}</div>
                  </div>
                  <div className={`ap-drawer-network-check ${isSelected ? "checked" : ""}`}>
                    {isSelected && <Plus size={14} className="rotate-45" />}
                  </div>
                </div>
              );
            }) : <p className="ap-drawer-network-empty">No data available...</p>}
          </div>
        </div>
      </div>
      <div className="ap-drawer-footer">
        <button
          onClick={() => {
            setAccessPoints(prev => prev.filter(ap => ap.id !== editingAp.id));
            setEditingAp(null);
          }}
          className="ap-drawer-delete"
        >
          <Trash2 size={20}/>
        </button>
        <button onClick={() => setEditingAp(null)} className="ap-drawer-save">Save</button>
      </div>
    </div>
  );
}
