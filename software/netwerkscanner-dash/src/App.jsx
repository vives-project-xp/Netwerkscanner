import React, { useState } from 'react';
import { LayoutDashboard, Building2, Loader2 } from 'lucide-react';
import Dashboard from './pages/Dashboard';
import Buildings from './pages/Buildings';
import BuildingDetail from './pages/BuildingDetail';
import { useBuildings } from './hooks/useBuildings';

export default function App() {
  const [activeTab, setActiveTab] = useState('dashboard');
  const [selectedBuildingId, setSelectedBuildingId] = useState(null);
  const { buildings, addBuilding, deleteBuilding, addFloor, deleteFloor, isLoaded } = useBuildings();

  const selectedBuilding = buildings.find(b => b.id === selectedBuildingId);

  if (!isLoaded) {
    return (
      <div className="h-screen bg-[#0f172a] flex flex-col items-center justify-center text-white">
        <Loader2 className="animate-spin mb-4 text-blue-500" size={48} />
        <p className="font-bold uppercase tracking-widest text-xs">Database Initialiseren...</p>
      </div>
    );
  }

  return (
    <div className="flex min-h-screen bg-[#0f172a]">
      <aside className="w-64 bg-[#1e293b] border-r border-slate-800 p-6 hidden lg:block">
        <div className="text-white font-black text-2xl mb-12 italic tracking-tighter">interface</div>
        <nav className="space-y-2">
          <button 
            onClick={() => { setActiveTab('dashboard'); setSelectedBuildingId(null); }}
            className={`w-full flex items-center gap-4 p-4 rounded-2xl font-bold transition-all ${activeTab === 'dashboard' ? 'bg-blue-600 text-white shadow-lg' : 'text-slate-400 hover:bg-slate-800'}`}
          >
            <LayoutDashboard size={20} /> Dashboard
          </button>
          <button 
            onClick={() => setActiveTab('gebouwen')}
            className={`w-full flex items-center gap-4 p-4 rounded-2xl font-bold transition-all ${activeTab === 'gebouwen' ? 'bg-blue-600 text-white shadow-lg' : 'text-slate-400 hover:bg-slate-800'}`}
          >
            <Building2 size={20} /> Gebouwen
          </button>
        </nav>
      </aside>

      <main className="flex-1 bg-slate-50 text-slate-900 h-screen overflow-y-auto">
        {activeTab === 'dashboard' ? (
          <Dashboard buildings={buildings} />
        ) : selectedBuilding ? (
          <BuildingDetail 
            building={selectedBuilding} 
            onBack={() => setSelectedBuildingId(null)} 
            onAddFloor={addFloor}
            onDeleteFloor={deleteFloor}
          />
        ) : (
          <Buildings 
            buildings={buildings} 
            onAddBuilding={addBuilding} 
            onDeleteBuilding={deleteBuilding} 
            onSelectBuilding={(b) => setSelectedBuildingId(b.id)} 
          />
        )}
      </main>
    </div>
  );
}