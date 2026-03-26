import React, { useState, useEffect, useMemo } from 'react';
import { TransformWrapper, TransformComponent } from "react-zoom-pan-pinch";
import { Wifi, Ruler, MapPin, Grid3X3, Database } from 'lucide-react';

export default function FloorVisionPro({ buildings = [] }) {
  // --- STATES ---
  const [scans, setScans] = useState([]); // De live data uit de database
  const [selectedBuildingId, setSelectedBuildingId] = useState('');
  const [selectedFloorId, setSelectedFloorId] = useState('');
  const [isCalibrating, setIsCalibrating] = useState(false);
  const [showGrid, setShowGrid] = useState(true);
  const [userLocation, setUserLocation] = useState(null); // Het 0,0 punt in %
  const [mapSize, setMapSize] = useState({ width: 0, height: 0 });
  const [rulerPoints, setRulerPoints] = useState([]);
  const [ppm, setPpm] = useState(() => JSON.parse(localStorage.getItem('fv_ppm_config') || '{}'));

  // --- FETCH LIVE SCAN DATA (De data die je in je console zag) ---
  useEffect(() => {
    const fetchData = async () => {
      try {
        const res = await fetch('/api/scans');
        const data = await res.json();
        // We loggen dit om te bevestigen dat de data er is
        console.log("📡 LIVE DATA BINNEN:", data);
        setScans(Array.isArray(data) ? data : []);
      } catch (e) { 
        console.error("Fetch error:", e); 
      }
    };
    fetchData();
    const interval = setInterval(fetchData, 3000);
    return () => clearInterval(interval);
  }, []);

  // --- SELECTIE LOGICA ---
  const currentBuilding = buildings.find(b => String(b.id) === String(selectedBuildingId));
  const currentFloor = currentBuilding?.floors?.find(f => String(f.id) === String(selectedFloorId));
  const currentPpmValue = ppm[selectedFloorId] || 0;

  // --- BEREKENING: POSITIES OP DE KAART ---
  const positionedPoints = useMemo(() => {
    // We gebruiken de 'scans' state omdat 'currentFloor.access_points' leeg is in je console
    if (!userLocation || !currentPpmValue || mapSize.width === 0 || scans.length === 0) {
      return [];
    }

    return scans.map((ap, index) => {
      // LOGICA: Elke scan 3 meter onder de vorige op de Y-as
      const xMeter = 0; 
      const yMeter = index * 3; 

      // Omrekenen van meters naar pixels op basis van de kalibratie (PPM)
      const offsetPixelsX = xMeter * currentPpmValue;
      const offsetPixelsY = yMeter * currentPpmValue;

      // Omrekenen naar % van de kaart om correct te tekenen
      const offsetPercentX = (offsetPixelsX / mapSize.width) * 100;
      const offsetPercentY = (offsetPixelsY / mapSize.height) * 100;

      return {
        ...ap,
        renderX: userLocation.x + offsetPercentX,
        renderY: userLocation.y + offsetPercentY,
        distance: yMeter.toFixed(2)
      };
    });
  }, [scans, userLocation, currentPpmValue, mapSize]);

  // --- HANDLERS ---
  const handleImageLoad = (e) => {
    setMapSize({ width: e.target.offsetWidth, height: e.target.offsetHeight });
  };

  const handleMapClick = (e) => {
    const rect = e.currentTarget.getBoundingClientRect();
    const x = ((e.clientX - rect.left) / rect.width) * 100;
    const y = ((e.clientY - rect.top) / rect.height) * 100;

    if (isCalibrating) {
      setRulerPoints(prev => prev.length >= 2 ? [{x, y}] : [...prev, {x, y}]);
    } else {
      setUserLocation({x, y});
      console.log("0,0 punt gezet op:", x.toFixed(2), "%", y.toFixed(2), "%");
    }
  };

  return (
    <div className="flex h-screen bg-[#020617] text-slate-300 font-sans overflow-hidden">
      
      {/* SIDEBAR */}
      <div className="w-80 bg-[#0f172a] border-r border-white/5 flex flex-col z-50 shadow-2xl">
        <div className="p-6 border-b border-white/5 space-y-4">
          <div className="flex items-center gap-2 mb-2">
            <Database size={16} className="text-blue-500" />
            <h2 className="text-xs font-black text-white uppercase tracking-widest text-blue-400">Grid Mapper Pro</h2>
          </div>
          
          <select value={selectedBuildingId} onChange={(e) => setSelectedBuildingId(e.target.value)} className="w-full bg-[#1e293b] border-white/10 rounded-xl px-4 py-3 text-xs text-white outline-none">
             <option value="">Selecteer Project...</option>
             {buildings.map(b => <option key={b.id} value={b.id}>{b.name}</option>)}
          </select>
          <select value={selectedFloorId} onChange={(e) => {setSelectedFloorId(e.target.value); setUserLocation(null);}} className="w-full bg-[#1e293b] border-white/10 rounded-xl px-4 py-3 text-xs text-white outline-none">
             <option value="">Selecteer Verdieping...</option>
             {currentBuilding?.floors?.map(f => <option key={f.id} value={f.id}>{f.name}</option>)}
          </select>

          <div className="flex gap-2">
            <button onClick={() => setIsCalibrating(!isCalibrating)} className={`flex-1 py-2 rounded-lg text-[9px] font-bold border transition-all ${isCalibrating ? 'bg-emerald-600 border-emerald-400 text-white' : 'bg-white/5 border-white/10'}`}>SCHAAL</button>
            <button onClick={() => setShowGrid(!showGrid)} className={`flex-1 py-2 rounded-lg text-[9px] font-bold border transition-all ${showGrid ? 'bg-blue-600 border-blue-400 text-white' : 'bg-white/5 border-white/10'}`}>GRID</button>
          </div>
        </div>

        <div className="flex-1 overflow-y-auto p-4 space-y-2">
           {userLocation ? (
             <div className="space-y-2">
               <h3 className="text-[10px] font-black text-blue-400 uppercase px-2">Gevonden Scans</h3>
               {positionedPoints.map((ap, i) => (
                 <div key={i} className="p-3 bg-white/5 rounded-xl border border-white/5 flex justify-between items-center text-[10px]">
                    <span className="font-bold text-slate-200">{ap.bssid || "Onbekend"}</span>
                    <span className="font-mono text-blue-400">{ap.distance}m</span>
                 </div>
               ))}
               <button onClick={() => setUserLocation(null)} className="w-full py-2 text-[9px] text-rose-500 font-bold uppercase mt-4">Reset Oorsprong</button>
             </div>
           ) : (
             <div className="text-center p-8 border border-dashed border-white/10 rounded-2xl text-[10px] text-slate-500 uppercase">
               Klik op de kaart voor 0,0 punt
             </div>
           )}
        </div>
      </div>

      {/* MAIN VIEW */}
      <div className="flex-1 relative bg-[#020617] flex items-center justify-center p-8 overflow-hidden">
        {currentFloor ? (
          <TransformWrapper disabled={isCalibrating} centerOnInit>
            <TransformComponent wrapperStyle={{ width: "100%", height: "100%" }}>
              <div className="relative inline-block bg-[#0f172a] shadow-2xl" onClick={handleMapClick}>
                <img src={currentFloor.image} alt="Map" onLoad={handleImageLoad} className="h-[85vh] w-auto block pointer-events-none" />
                
                {/* GRID */}
                {showGrid && currentPpmValue > 0 && userLocation && (
                  <div className="absolute inset-0 pointer-events-none opacity-20" 
                    style={{ 
                      backgroundImage: `linear-gradient(to right, #6366f1 1px, transparent 1px), linear-gradient(to bottom, #6366f1 1px, transparent 1px)`,
                      backgroundSize: `${currentPpmValue}px ${currentPpmValue}px`,
                      backgroundPosition: `${(userLocation.x / 100) * mapSize.width}px ${(userLocation.y / 100) * mapSize.height}px`
                    }} 
                  />
                )}

                {/* 0,0 MARKER */}
                {userLocation && (
                  <div className="absolute z-[100] pointer-events-none" style={{ left: `${userLocation.x}%`, top: `${userLocation.y}%`, transform: 'translate(-50%, -50%)' }}>
                    <div className="w-6 h-6 bg-emerald-500 rounded-full border-4 border-white shadow-lg flex items-center justify-center animate-pulse" />
                  </div>
                )}

                {/* DE PUNTEN UIT DE DATABASE (GEPLOT PER 3 METER) */}
                {positionedPoints.map((ap, i) => (
                  <div key={i} className="absolute z-[90] transition-all duration-500" 
                       style={{ 
                         left: `${ap.renderX}%`, 
                         top: `${ap.renderY}%`, 
                         transform: 'translate(-50%, -50%)'
                       }}>
                    <div className="relative group">
                      <div className="w-8 h-8 rounded-lg border-2 border-blue-500 bg-[#0f172a] flex items-center justify-center shadow-lg shadow-blue-500/30">
                        <Wifi size={14} className="text-blue-400" />
                      </div>
                      <div className="absolute -top-6 left-1/2 -translate-x-1/2 bg-blue-600 text-white text-[8px] px-1.5 py-0.5 rounded font-bold whitespace-nowrap">
                        {ap.distance}m
                      </div>
                    </div>
                  </div>
                ))}

                {/* KALIBRATIE HUD */}
                {isCalibrating && (
                  <svg className="absolute inset-0 w-full h-full pointer-events-none z-[110]">
                    {rulerPoints.map((p, i) => <circle key={i} cx={`${p.x}%`} cy={`${p.y}%`} r="6" fill="#10b981" stroke="white" strokeWidth="2" />)}
                    {rulerPoints.length === 2 && <line x1={`${rulerPoints[0].x}%`} y1={`${rulerPoints[0].y}%`} x2={`${rulerPoints[1].x}%`} y2={`${rulerPoints[1].y}%`} stroke="#10b981" strokeWidth="3" strokeDasharray="8" />}
                  </svg>
                )}
              </div>
            </TransformComponent>
          </TransformWrapper>
        ) : <div className="text-slate-500 text-xs font-black uppercase tracking-widest animate-pulse">Selecteer een verdieping...</div>}

        {/* KALIBRATIE MODAL */}
        {isCalibrating && rulerPoints.length === 2 && (
          <div className="absolute bottom-10 left-1/2 -translate-x-1/2 bg-[#0f172a] border-2 border-emerald-500 p-6 rounded-3xl z-[200] flex flex-col items-center gap-4">
            <p className="text-white text-sm font-black uppercase italic">Afstand in meters?</p>
            <button onClick={() => {
               const m = prompt("Meters?");
               if(m) {
                 const p1 = rulerPoints[0]; const p2 = rulerPoints[1];
                 const px = Math.sqrt(Math.pow((p2.x-p1.x)*(mapSize.width/100), 2) + Math.pow((p2.y-p1.y)*(mapSize.height/100),2));
                 const newPpm = px / parseFloat(m);
                 const updated = { ...ppm, [selectedFloorId]: newPpm };
                 setPpm(updated);
                 localStorage.setItem('fv_ppm_config', JSON.stringify(updated));
                 setIsCalibrating(false); setRulerPoints([]);
               }
            }} className="bg-emerald-600 px-8 py-3 rounded-xl text-xs font-black text-white uppercase tracking-widest shadow-lg shadow-emerald-500/20">Bevestig</button>
          </div>
        )}
      </div>
    </div>
  );
}