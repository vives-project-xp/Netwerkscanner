import React from "react";
import "../App.css";

export default function AccessPointList({ mappedAPs, isEditMode, editingAp, setEditingAp }) {
  return (
    <div className="ap-list">
      <h2 className="ap-list-title">Live Status</h2>
      {mappedAPs.map(ap => (
        <div
          key={ap.id}
          onClick={() => isEditMode && setEditingAp(ap)}
          className={`ap-list-item ${editingAp?.id === ap.id ? "selected" : ""}`}
        >
          <div className="ap-list-item-row">
            <span className="ap-list-item-name">{ap.name}</span>
            <div className={`ap-list-item-dot ${ap.status === "online" ? "online" : "offline"}`} />
          </div>
        </div>
      ))}
    </div>
  );
}
