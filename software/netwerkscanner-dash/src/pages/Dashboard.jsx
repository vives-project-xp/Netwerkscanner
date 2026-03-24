import { useState, useEffect, useMemo, useRef } from 'react';
import { TransformWrapper, TransformComponent } from "react-zoom-pan-pinch";
import { Wifi, X, Plus, Cpu, Trash2, Layers} from 'lucide-react';

export default function NetwerkscannerDash({ buildings = [] }) {
  const [scans, setScans] = useState([]);
  const [loading, setLoading] = useState(false);
  const [selectedBuildingId, setSelectedBuildingId] = useState('');
  const [selectedFloorId, setSelectedFloorId] = useState('');
  const [isEditMode, setIsEditMode] = useState(false);
  const [editingAp, setEditingAp] = useState(null);
  const [showFloorConfig, setShowFloorConfig] = useState(false);

  const [accessPoints, setAccessPoints] = useState(() => {
    const saved = localStorage.getItem('fv_ap_config');
    return saved ? JSON.parse(saved) : [];
  });

  const [floorScannerMapping, setFloorScannerMapping] = useState(() => {
    const saved = localStorage.getItem('fv_floor_esp_config');
    return saved ? JSON.parse(saved) : {};
  });

  const imgRef = useRef(null);

  useEffect(() => {
    localStorage.setItem('fv_ap_config', JSON.stringify(accessPoints));
    localStorage.setItem('fv_floor_esp_config', JSON.stringify(floorScannerMapping));
  }, [accessPoints, floorScannerMapping]);

  const fetchData = async () => {
    setLoading(true);
    try {
      const res = await fetch('/api/scans');
      const data = await res.json();
      console.log("--- RAW DATABASE DATA ---");
      console.table(data);
      const validData = (Array.isArray(data) ? data : []).filter(item => item.device_id);
      setScans(validData);
    } catch (e) { 
      console.error("API Error:", e); 
    } finally { 
      setLoading(false); 
    }
  };

  useEffect(() => {
    fetchData();
    const interval = setInterval(fetchData, 5000);
    return () => clearInterval(interval);
  }, []);

  const linkedDeviceId = useMemo(() => {
    return floorScannerMapping[selectedFloorId] || "";
  }, [floorScannerMapping, selectedFloorId]);

  const floorSpecificScans = useMemo(() => {
    if (!linkedDeviceId) return [];
    return scans.filter(s => s.device_id === linkedDeviceId);
  }, [scans, linkedDeviceId]);

  const mappedAPs = useMemo(() => {
    return accessPoints
      .filter(ap => ap.floorId === selectedFloorId)
      .map(ap => {
        const liveData = floorSpecificScans.filter(s => 
          ap.assignedBssids.some(b => b === s.bssid)
        );
        const strongestRssi = liveData.length > 0 ? Math.max(...liveData.map(l => l.rssi)) : -100;
        return { 
          ...ap, 
          status: liveData.length > 0 ? 'online' : 'offline', 
          rssi: strongestRssi,
          activeSignals: liveData 
        };
      });
  }, [floorSpecificScans, accessPoints, selectedFloorId]);

  const currentBuilding = buildings.find(b => String(b.id) === String(selectedBuildingId));
  const currentFloor = currentBuilding?.floors?.find(f => String(f.id) === String(selectedFloorId));

  return (
    <div className="flex h-screen bg-[#020617] text-slate-300 overflow-hidden font-sans">
      {/* SIDEBAR */}
      <div className="w-80 bg-[#0f172a] border-r border-white/5 flex flex-col z-50 shadow-2xl">
        <div className="p-6 border-b border-white/5 space-y-4">
          <div className="flex items-center gap-3">
            <div className="bg-blue-600 p-2 rounded-xl text-white shadow-lg"><Layers size={20} /></div>
            <h1 className="font-black uppercase tracking-tighter text-white text-xl italic">Controls</h1>
          </div>
          
          <div className="flex bg-black/40 p-1 rounded-xl border border-white/5">
            <button onClick={() => {setIsEditMode(false); setShowFloorConfig(false);}} className={`flex-1 py-2 rounded-lg text-[10px] font-black uppercase transition-all ${!isEditMode && !showFloorConfig ? 'bg-blue-600 text-white' : 'text-slate-500'}`}>Monitor</button>
            <button onClick={() => {setIsEditMode(true); setShowFloorConfig(false);}} className={`flex-1 py-2 rounded-lg text-[10px] font-black uppercase transition-all ${isEditMode ? 'bg-amber-500 text-black' : 'text-slate-500'}`}>Punten</button>
            <button onClick={() => {setShowFloorConfig(true); setIsEditMode(false);}} className={`flex-1 py-2 rounded-lg text-[10px] font-black uppercase transition-all ${showFloorConfig ? 'bg-indigo-600 text-white' : 'text-slate-500'}`}>ESP Link</button>
          </div>
        </div>

        <div className="flex-1 overflow-y-auto p-4 space-y-6">
          <div className="space-y-2 px-2">
            <label className="text-[10px] font-black text-slate-500 uppercase tracking-widest">Verdieping Selectie</label>
            <select value={selectedBuildingId} onChange={(e) => {setSelectedBuildingId(e.target.value); setSelectedFloorId('');}} className="w-full bg-[#1e293b] border border-white/10 rounded-xl px-4 py-3 text-xs font-bold text-white outline-none">
               <option value="">Kies Project...</option>
               {buildings.map(b => <option key={b.id} value={b.id}>{b.name}</option>)}
            </select>
            <select disabled={!selectedBuildingId} value={selectedFloorId} onChange={(e) => setSelectedFloorId(e.target.value)} className="w-full bg-[#1e293b] border border-white/10 rounded-xl px-4 py-3 text-xs font-bold text-white outline-none">
               <option value="">Kies Verdieping...</option>
               {currentBuilding?.floors?.map(f => <option key={f.id} value={f.id}>{f.name}</option>)}
            </select>
          </div>

          {showFloorConfig && selectedFloorId && (
            <div className="p-4 bg-indigo-600/10 border border-indigo-500/20 rounded-2xl space-y-4 animate-in fade-in duration-300">
               <h3 className="text-xs font-black text-white uppercase flex items-center gap-2 italic"><Cpu size={14}/> ESP Koppelen</h3>
               <div className="space-y-2">
                  <select 
                    value={linkedDeviceId}
                    onChange={(e) => setFloorScannerMapping(prev => ({...prev, [selectedFloorId]: e.target.value}))}
                    className="w-full bg-black/40 border border-white/10 rounded-xl px-3 py-2 text-xs text-blue-400 font-mono outline-none"
                  >
                    <option value="">-- Geen ESP geselecteerd --</option>
                    {[...new Set(scans.map(s => s.device_id))].filter(Boolean).map(id => (
                      <option key={id} value={id}>{id}</option>
                    ))}
                  </select>
               </div>
            </div>
          )}

          {!showFloorConfig && (
            <div className="space-y-2">
              <h2 className="text-[10px] font-black text-slate-500 uppercase px-2 tracking-widest italic">Live Status</h2>
              {mappedAPs.map((ap) => (
                <div key={ap.id} onClick={() => isEditMode && setEditingAp(ap)} className={`p-4 rounded-2xl border transition-all ${editingAp?.id === ap.id ? 'bg-blue-600/20 border-blue-500' : 'bg-white/5 border-white/5'}`}>
                  <div className="flex justify-between items-center">
                    <span className="font-bold text-sm text-white">{ap.name}</span>
                    <div className={`w-2 h-2 rounded-full ${ap.status === 'online' ? 'bg-emerald-500 shadow-[0_0_10px_rgba(16,185,129,0.5)]' : 'bg-slate-700'}`} />
                  </div>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>

      {/* MAP CANVAS */}
      <div className="flex-1 relative bg-[#f1f5f9] flex items-center justify-center p-8 overflow-hidden fv-map-canvas">
        {loading && (
          <div className="fv-loading-overlay fv-loading-overlay-local" />
        )}
        {currentFloor ? (
          <TransformWrapper disabled={isEditMode} centerOnInit>
            <TransformComponent wrapperStyle={{ width: "100%", height: "100%" }}>
              <div className="relative inline-block bg-white shadow-2xl" onClick={(e) => {
                if (!isEditMode || !imgRef.current || editingAp) return;
                const rect = imgRef.current.getBoundingClientRect();
                const x = parseFloat(((e.clientX - rect.left) / rect.width * 100).toFixed(2));
                const y = parseFloat(((e.clientY - rect.top) / rect.height * 100).toFixed(2));
                const newAp = { id: `ap_${Date.now()}`, name: "Nieuw Punt", x, y, assignedBssids: [], floorId: selectedFloorId };
                setAccessPoints(prev => [...prev, newAp]);
                setEditingAp(newAp);
              }}>
                <img ref={imgRef} src={currentFloor.image} alt="Floorplan" className="h-[85vh] w-auto block pointer-events-none" />
                <div className="absolute inset-0 pointer-events-none">
                  {mappedAPs.map((ap) => (
                    <div 
                      key={ap.id} 
                      className={`absolute pointer-events-auto transition-all cursor-pointer group ${editingAp?.id === ap.id ? 'z-[100]' : 'z-20'}`} 
                      style={{ left: `${ap.x}%`, top: `${ap.y}%`, transform: 'translate(-50%, -50%)' }} 
                      onClick={(e) => { e.stopPropagation(); isEditMode && setEditingAp(ap); }}
                    >
                      {/* AP ICON */}
                      <div className={`w-12 h-12 rounded-2xl border-4 flex items-center justify-center shadow-2xl transition-all ${
                        editingAp?.id === ap.id ? 'bg-amber-500 border-white scale-125' : 
                        ap.status === 'online' ? 'bg-[#0f172a] border-blue-500 hover:scale-110' : 'bg-slate-300 border-white opacity-40'
                      }`}>
                        <Wifi size={20} className={ap.status === 'online' ? 'text-blue-400' : 'text-slate-600'} />
                      </div>

                      {/* HOVER TOOLTIP (Alleen in Monitor mode) */}
                      {!isEditMode && (
                        <div className="absolute bottom-full left-1/2 -translate-x-1/2 mb-3 w-56 bg-[#0f172a] border border-white/10 rounded-xl p-3 shadow-2xl opacity-0 group-hover:opacity-100 pointer-events-none transition-all translate-y-2 group-hover:translate-y-0 z-[500]">
                          <div className="flex justify-between items-start mb-2 border-b border-white/5 pb-2">
                            <span className="text-[10px] font-black text-white uppercase truncate pr-2">{ap.name}</span>
                            <span className={`text-[9px] font-mono px-1.5 py-0.5 rounded ${ap.status === 'online' ? 'bg-emerald-500/20 text-emerald-500' : 'bg-slate-800 text-slate-500'}`}>
                              {ap.status === 'online' ? `${ap.rssi}dBm` : 'OFFLINE'}
                            </span>
                          </div>
                          
                          {ap.status === 'online' && ap.activeSignals?.length > 0 ? (
                            <div className="space-y-1.5">
                              <p className="text-[8px] text-slate-500 uppercase font-bold tracking-widest">Gevonden netwerken:</p>
                              {ap.activeSignals.map((scan, idx) => (
                                <div key={idx} className="flex justify-between items-center bg-white/5 p-1 rounded">
                                  <div className="flex flex-col overflow-hidden">
                                    <span className="text-[9px] text-blue-400 font-bold truncate leading-none">{scan.ssid}</span>
                                    <span className="text-[7px] text-slate-500 font-mono uppercase">{scan.bssid}</span>
                                  </div>
                                  <span className="text-[9px] font-mono text-slate-300 ml-2">{scan.rssi}</span>
                                </div>
                              ))}
                            </div>
                          ) : (
                            <p className="text-[9px] text-slate-600 italic">Geen actief signaal gedetecteerd.</p>
                          )}
                          <div className="absolute top-full left-1/2 -translate-x-1/2 border-8 border-transparent border-t-[#0f172a]"></div>
                        </div>
                      )}
                    </div>
                  ))}
                </div>
              </div>
            </TransformComponent>
          </TransformWrapper>
        ) : (
          <div className="text-slate-400 font-black uppercase text-[10px] tracking-[0.5em]">Kies een project & verdieping</div>
        )}

        {/* --- EDIT DRAWER --- */}
        {isEditMode && editingAp && (
          <div className="absolute top-4 right-4 bottom-4 w-[420px] bg-[#0f172a] border border-white/10 rounded-[32px] shadow-2xl z-[1000] flex flex-col">
            <div className="p-8 border-b border-white/5 flex justify-between items-center">
              <h3 className="font-black uppercase text-xs text-white italic">Punt Instellingen</h3>
              <button onClick={() => setEditingAp(null)} className="p-2 text-slate-400"><X size={24}/></button>
            </div>
            <div className="p-8 space-y-6 overflow-y-auto flex-1">
              <input type="text" value={editingAp.name} onChange={(e) => {
                 const val = e.target.value;
                 setAccessPoints(prev => prev.map(ap => ap.id === editingAp.id ? {...ap, name: val} : ap));
                 setEditingAp(prev => ({...prev, name: val}));
              }} className="w-full bg-black/40 border border-white/10 rounded-2xl px-5 py-4 text-white font-bold outline-none focus:ring-2 ring-blue-500/50" />
              
              <div className="space-y-4">
                <label className="text-[10px] font-black text-slate-500 uppercase tracking-widest">Koppel Netwerken</label>
                <div className="bg-black/40 border border-white/10 rounded-2xl overflow-hidden divide-y divide-white/5 max-h-[400px] overflow-y-auto">
                   {scans.length > 0 ? [...new Map(scans.map(s => [s.bssid, s])).values()].map(signal => {
                      const isSelected = editingAp.assignedBssids.includes(signal.bssid);
                      return (
                        <div key={signal.bssid} onClick={() => {
                          const next = isSelected ? editingAp.assignedBssids.filter(b => b !== signal.bssid) : [...editingAp.assignedBssids, signal.bssid];
                          setAccessPoints(prev => prev.map(ap => ap.id === editingAp.id ? {...ap, assignedBssids: next} : ap));
                          setEditingAp(prev => ({...prev, assignedBssids: next}));
                        }} className={`p-4 flex items-center justify-between cursor-pointer ${isSelected ? 'bg-blue-600/20' : ''}`}>
                           <div>
                              <div className="text-xs font-bold text-white">{signal.ssid}</div>
                              <div className="text-[9px] text-slate-500 font-mono">{signal.bssid}</div>
                           </div>
                           <div className={`w-5 h-5 rounded border-2 ${isSelected ? 'bg-blue-600 border-blue-600 text-white' : 'border-white/10'}`}>
                              {isSelected && <Plus size={14} className="rotate-45" />}
                           </div>
                        </div>
                      )
                   }) : <p className="p-4 text-xs italic text-slate-600">Geen data beschikbaar...</p>}
                </div>
              </div>
            </div>
            <div className="p-8 border-t border-white/5 flex gap-4">
               <button onClick={() => {setAccessPoints(prev => prev.filter(ap => ap.id !== editingAp.id)); setEditingAp(null);}} className="p-4 bg-rose-600/20 text-rose-500 rounded-2xl transition-colors hover:bg-rose-600 hover:text-white"><Trash2 size={20}/></button>
               <button onClick={() => setEditingAp(null)} className="flex-1 bg-blue-600 text-white py-4 rounded-2xl font-black uppercase tracking-widest">Opslaan</button>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}