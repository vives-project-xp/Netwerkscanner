import React from "react";
import { Layers } from "lucide-react";
import "../App.css";

export default function Sidebar({ isEditMode, showFloorConfig, setIsEditMode, setShowFloorConfig }) {
  return (
    <div className="sidebar">
      <div className="sidebar-header">
        <div className="sidebar-logo"><Layers size={20} /></div>
        <h1 className="sidebar-title">Netwerkscanner</h1>
        <div className="sidebar-subtitle">Dashboard</div>
      </div>
      <div className="sidebar-tabs">
        <button
          onClick={() => { setIsEditMode(false); setShowFloorConfig(false); }}
          className={`sidebar-tab ${!isEditMode && !showFloorConfig ? "active" : ""}`}
        >
          Monitor
        </button>
        <button
          onClick={() => { setIsEditMode(true); setShowFloorConfig(false); }}
          className={`sidebar-tab ${isEditMode ? "edit" : ""}`}
        >
          Points
        </button>
        <button
          onClick={() => { setShowFloorConfig(true); setIsEditMode(false); }}
          className={`sidebar-tab ${showFloorConfig ? "link" : ""}`}
        >
          ESP Link
        </button>
      </div>
    </div>
  );
}
