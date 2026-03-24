import React, { useState } from 'react';
import { Building2, Plus, Trash2, ChevronRight, MapPin } from 'lucide-react';

export default function Buildings({ buildings, onAddBuilding, onDeleteBuilding, onSelectBuilding }) {
  const [showModal, setShowModal] = useState(false);
  const [name, setName] = useState('');
  const [loc, setLoc] = useState('');

  return (
    <div className="p-8 space-y-8">
      <div className="flex justify-between items-center">
        <h1 className="text-2xl font-black italic uppercase">Projecten</h1>
        <button onClick={() => setShowModal(true)} className="bg-blue-600 text-white px-6 py-3 rounded-2xl font-black text-xs uppercase flex items-center gap-2">
          <Plus size={16} /> Nieuw Gebouw
        </button>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {buildings.map(b => (
          <div key={b.id} className="bg-white rounded-3xl p-6 border border-slate-200 shadow-sm relative group">
            <button onClick={() => onDeleteBuilding(b.id)} className="absolute top-4 right-4 text-slate-300 hover:text-red-500 transition-colors">
              <Trash2 size={18} />
            </button>
            <div className="bg-blue-50 w-12 h-12 rounded-xl flex items-center justify-center text-blue-600 mb-4"><Building2 /></div>
            <h3 className="font-bold text-lg">{b.name}</h3>
            <p className="text-xs text-slate-400 mb-6 flex items-center gap-1"><MapPin size={12}/> {b.location}</p>
            <button onClick={() => onSelectBuilding(b)} className="w-full py-3 bg-slate-50 rounded-xl text-xs font-bold hover:bg-blue-600 hover:text-white transition-all flex items-center justify-center gap-2">
              Beheer Verdiepingen <ChevronRight size={14} />
            </button>
          </div>
        ))}
      </div>

      {showModal && (
        <div className="fixed inset-0 bg-slate-900/50 backdrop-blur-sm flex items-center justify-center z-50 p-4">
          <form className="bg-white p-8 rounded-3xl max-w-md w-full shadow-2xl" onSubmit={(e) => { e.preventDefault(); onAddBuilding(name, loc); setShowModal(false); }}>
            <h2 className="text-xl font-bold mb-6 italic">NIEUW GEBOUW</h2>
            <input required className="w-full p-4 bg-slate-50 rounded-xl mb-4 outline-none" placeholder="Naam" onChange={e => setName(e.target.value)} />
            <input required className="w-full p-4 bg-slate-50 rounded-xl mb-6 outline-none" placeholder="Locatie" onChange={e => setLoc(e.target.value)} />
            <div className="flex gap-4">
              <button type="button" onClick={() => setShowModal(false)} className="flex-1 font-bold text-slate-400">Annuleer</button>
              <button type="submit" className="flex-1 bg-blue-600 text-white p-4 rounded-xl font-bold">Opslaan</button>
            </div>
          </form>
        </div>
      )}
    </div>
  );
}