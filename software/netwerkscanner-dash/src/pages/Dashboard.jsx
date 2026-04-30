import React, { useState, useEffect, useMemo, useRef } from 'react';
import { TransformWrapper, TransformComponent } from "react-zoom-pan-pinch";
import { Database, Layers, RefreshCw, MapPin, ZoomIn, ZoomOut, Maximize, Ruler, X, ArrowLeft } from 'lucide-react';

const snapToGrid = (percent, mapDimPixels, ppm) => {
  if (ppm === 0) return percent;
  const currentPixels = (percent / 100) * mapDimPixels;
  const snappedPixels = Math.round(currentPixels / ppm) * ppm;
  return (snappedPixels / mapDimPixels) * 100;
};

export default function FloorVisionPro({ buildings = [] }) {
  const [scans, setScans] = useState([]); 
  const [selectedBuildingId, setSelectedBuildingId] = useState('');
  const [selectedFloorId, setSelectedFloorId] = useState('');
  const [isCalibrating, setIsCalibrating] = useState(false);
  const [showGrid, setShowGrid] = useState(true);
  const [userLocation, setUserLocation] = useState(null); 
  const [mapSize, setMapSize] = useState({ width: 0, height: 0 });
  const [rulerPoints, setRulerPoints] = useState([]);
  const [loading, setLoading] = useState(true);
  const [activePointId, setActivePointId] = useState(null);
  const [ppm, setPpm] = useState(() => JSON.parse(localStorage.getItem('fv_ppm_config') || '{}'));
  
  const imgRef = useRef(null);

  const updateMapSize = () => {
    if (imgRef.current) {
      setMapSize({
        width: imgRef.current.clientWidth,
        height: imgRef.current.clientHeight 
      });
    }
  };

  useEffect(() => {
    const fetchData = async () => {
      try {
        const res = await fetch('/api/heatmap'); 
        const data = await res.json();
          setScans(Array.isArray(data) ? data : []);
          console.log('Database info (API /api/heatmap):', data);
        setLoading(false);
      } catch (e) { setLoading(false); }
    };
    fetchData();
    const interval = setInterval(fetchData, 5000); 
    return () => clearInterval(interval);
  }, []);

  const currentBuilding = buildings.find(b => String(b.id) === String(selectedBuildingId));
  const currentFloor = currentBuilding?.floors?.find(f => String(f.id) === String(selectedFloorId));
  const currentPpmValue = ppm[selectedFloorId] || 0;

  const latestPointId = useMemo(() => {
    if (!scans.length) return null;
    let bestId = null;
    let bestValue = -Infinity;
    scans.forEach(scan => {
      const id = parseInt(scan.scan_id || scan.scanId || 0, 10);
      if (!Number.isNaN(id) && id > bestValue) {
        bestValue = id;
        bestId = `${scan.x}_${scan.y}`;
      }
    });
    return bestId;
  }, [scans]);

  const positionedPoints = useMemo(() => {
    if (!userLocation || !currentPpmValue || mapSize.width === 0 || scans.length === 0) return [];

    const meterToPercentX = (currentPpmValue / mapSize.width) * 100;
    const meterToPercentY = (currentPpmValue / mapSize.height) * 100;

    const groups = scans.reduce((acc, scan) => {
      const key = `${scan.x}_${scan.y}`;
      if (!acc[key]) {
        acc[key] = {
          id: key,
          gridX: parseFloat(scan.x || 0),
          gridY: parseFloat(scan.y || 0),
          networks: [],
          maxRssi: -100,
          maxScanId: -Infinity,
        };
      }
      const rssiVal = parseInt(scan.net_rssi || -100, 10);
      const scanIdVal = parseInt(scan.scan_id || scan.scanId || 0, 10);
      acc[key].networks.push({ ssid: scan.net_ssid, bssid: scan.net_bssid, rssi: rssiVal });
      if (rssiVal > acc[key].maxRssi) acc[key].maxRssi = rssiVal;
      if (!Number.isNaN(scanIdVal) && scanIdVal > acc[key].maxScanId) {
        acc[key].maxScanId = scanIdVal;
      }
      return acc;
    }, {});

    return Object.values(groups).map(group => ({
      ...group,
      renderX: userLocation.x + (group.gridX * meterToPercentX),
      renderY: userLocation.y + (group.gridY * meterToPercentY),
      isLatestScan: group.id === latestPointId,
    }));
  }, [scans, userLocation, currentPpmValue, mapSize, latestPointId]);

  const activePointData = useMemo(() => 
    positionedPoints.find(p => p.id === activePointId), 
    [activePointId, positionedPoints]
  );

  const handleMapClick = (e) => {
    if (activePointId) setActivePointId(null);
    if (!currentFloor) return;

    const rect = e.currentTarget.getBoundingClientRect();
    let x = ((e.clientX - rect.left) / rect.width) * 100;
    let y = ((e.clientY - rect.top) / rect.height) * 100;

    if (isCalibrating) {
      setRulerPoints(prev => prev.length >= 2 ? [{x, y}] : [...prev, {x, y}]);
    } else {
      if (showGrid && currentPpmValue > 0) {
        x = snapToGrid(x, mapSize.width, currentPpmValue);
        y = snapToGrid(y, mapSize.height, currentPpmValue);
      }
      setUserLocation({x, y});
    }
  };

  const handleConfirmCalibration = () => {
    const m = prompt("Hoeveel meter is de getrokken lijn?");
    if (m && rulerPoints.length === 2) {
      const p1 = rulerPoints[0];
      const p2 = rulerPoints[1];
      const dx = (p2.x - p1.x) * (mapSize.width / 100);
      const dy = (p2.y - p1.y) * (mapSize.height / 100);
      const pixelDistance = Math.sqrt(dx * dx + dy * dy);
      const newPpm = pixelDistance / parseFloat(m);
      const updated = { ...ppm, [selectedFloorId]: newPpm };
      setPpm(updated);
      localStorage.setItem('fv_ppm_config', JSON.stringify(updated));
      setIsCalibrating(false);
      setRulerPoints([]);
    }
  };

  return (
    <div className="flex h-screen bg-white text-slate-800 font-sans overflow-hidden">
      
      {/* SIDEBAR */}
      <div className="w-80 bg-[#1e293b] border-r border-slate-700 flex flex-col z-50 shadow-xl overflow-hidden">
        <div className="p-6 border-b border-slate-700 space-y-4 bg-[#0f172a]">
          <div className="flex items-center justify-between text-white">
            <div className="flex items-center gap-2">
              <Database size={18} className="text-blue-400" />
              <h2 className="text-[11px] font-black uppercase tracking-widest">FloorVision Pro</h2>
            </div>
            {loading ? <RefreshCw size={14} className="animate-spin text-blue-400" /> : <div className="h-2 w-2 rounded-full bg-emerald-500 animate-pulse" />}
          </div>
          <div className="space-y-2">
            <select value={selectedBuildingId} onChange={(e) => setSelectedBuildingId(e.target.value)} className="w-full bg-slate-800 border border-slate-600 rounded-lg px-3 py-2 text-[11px] text-white outline-none">
               <option value="">Gebouw...</option>
               {buildings.map(b => <option key={b.id} value={b.id}>{b.name}</option>)}
            </select>
            <select value={selectedFloorId} onChange={(e) => {setSelectedFloorId(e.target.value); setUserLocation(null);}} className="w-full bg-slate-800 border border-slate-600 rounded-lg px-3 py-2 text-[11px] text-white outline-none">
               <option value="">Verdieping...</option>
               {currentBuilding?.floors?.map(f => <option key={f.id} value={f.id}>{f.name}</option>)}
            </select>
          </div>
        </div>

        <div className="flex-1 overflow-y-auto custom-scrollbar-dark bg-slate-900/50">
           {activePointData ? (
             <div className="animate-in slide-in-from-left duration-200">
                <div className="p-4 bg-slate-800 border-b border-slate-700">
                  <button onClick={() => setActivePointId(null)} className="flex items-center gap-2 text-blue-400 text-[10px] font-bold uppercase mb-3 hover:text-white transition-colors">
                    <ArrowLeft size={14}/> Terug
                  </button>
                  <h3 className="text-white font-mono text-xl font-bold">{activePointData.gridX}m, {activePointData.gridY}m</h3>
                </div>
                <div className="p-3 space-y-2">
                  {activePointData.networks.sort((a,b) => b.rssi - a.rssi).map((n, idx) => (
                    <div key={idx} className="p-3 rounded-xl bg-slate-800 border border-slate-700">
                      <div className="flex justify-between items-start">
                        <p className="text-white text-[12px] font-bold truncate pr-2">{n.ssid || "Hidden"}</p>
                        <span className={`text-[12px] font-black ${n.rssi > -60 ? 'text-emerald-400' : 'text-blue-400'}`}>{n.rssi}</span>
                      </div>
                    </div>
                  ))}
                </div>
             </div>
           ) : userLocation ? (
             <div className="p-4 space-y-2">
               <p className="text-[9px] font-black text-slate-500 uppercase tracking-widest mb-4 flex items-center gap-2 px-1"><Layers size={12}/> Punten ({positionedPoints.length})</p>
               {positionedPoints.map((gp, i) => (
                 <div key={i} className="p-3 rounded-xl border border-slate-700 bg-slate-800 hover:border-blue-500/50 cursor-pointer transition-all" onClick={() => setActivePointId(gp.id)}>
                    <div className="flex justify-between items-center">
                        <span className="text-[10px] font-bold text-white">{gp.gridX}m, {gp.gridY}m</span>
                        <div className={`h-2 w-2 rounded-full ${gp.maxRssi > -60 ? 'bg-emerald-500' : 'bg-blue-500'}`} />
                    </div>
                 </div>
               ))}
               <button onClick={() => setUserLocation(null)} className="w-full py-3 text-[10px] text-slate-500 font-bold uppercase hover:text-rose-500 transition-colors">Reset Positie</button>
             </div>
           ) : (
             <div className="flex flex-col items-center justify-center h-64 text-center px-10 opacity-40 text-white">
                <MapPin size={28} className="mb-4 text-blue-400"/>
                <p className="text-[10px] font-bold uppercase tracking-widest leading-relaxed">Klik op de grid om het<br/>0m punt te bepalen</p>
             </div>
           )}
        </div>

        <div className="p-4 border-t border-slate-700 bg-[#0f172a] flex gap-2">
            <button onClick={() => { setIsCalibrating(!isCalibrating); setRulerPoints([]); }} className={`flex-1 py-2 rounded-lg text-[10px] font-black border transition-all ${isCalibrating ? 'bg-emerald-600 text-white border-emerald-500' : 'bg-slate-800 border-slate-600 text-slate-400'}`}>SCHAAL</button>
            <button onClick={() => setShowGrid(!showGrid)} className={`flex-1 py-2 rounded-lg text-[10px] font-black border transition-all ${showGrid ? 'bg-blue-600 text-white border-blue-500' : 'bg-slate-800 border-slate-600 text-slate-400'}`}>GRID</button>
        </div>
      </div>

      {/* MAIN MAP */}
      <div className="flex-1 relative bg-white flex items-center justify-center overflow-hidden">
        {currentFloor ? (
          <TransformWrapper disabled={isCalibrating} centerOnInit>
            {({ zoomLevel, zoomIn, zoomOut, resetTransform }) => (
              <>
                <div className="absolute top-6 right-6 z-[450] flex flex-col gap-2 p-1.5 bg-slate-800/80 shadow-2xl rounded-2xl border border-slate-700">
                  <button onClick={() => zoomIn()} className="p-2.5 text-white hover:bg-slate-700 rounded-xl"><ZoomIn size={20}/></button>
                  <button onClick={() => zoomOut()} className="p-2.5 text-white hover:bg-slate-700 rounded-xl"><ZoomOut size={20}/></button>
                  <button onClick={() => resetTransform()} className="p-2.5 text-white hover:bg-slate-700 rounded-xl"><Maximize size={20}/></button>
                </div>

                <TransformComponent wrapperStyle={{ width: "100%", height: "100%" }}>
                  <div className="relative inline-block" onClick={handleMapClick}>
                    <img 
                      ref={imgRef}
                      src={currentFloor.image} 
                      onLoad={updateMapSize}
                      className="h-[95vh] w-auto block pointer-events-none" 
                    />
                    
                    {/* GRID */}
                    {showGrid && currentPpmValue > 0 && userLocation && (
                      <div className="absolute inset-0 pointer-events-none" 
                        style={{ 
                          backgroundImage: `linear-gradient(to right, rgba(0,0,0,0.1) 1px, transparent 1px), linear-gradient(to bottom, rgba(0,0,0,0.1) 1px, transparent 1px)`,
                          backgroundSize: `${currentPpmValue}px ${currentPpmValue}px`,
                          backgroundPosition: `${(userLocation.x / 100) * mapSize.width}px ${(userLocation.y / 100) * mapSize.height}px`,
                          zIndex: 10
                        }} 
                      />
                    )}

                    {/* DATA PUNTEN (De 0,0 bol is nu een van deze punten) */}
                    {positionedPoints.map((gp, i) => (
                        <div key={i} className="absolute z-[150]" style={{ left: `${gp.renderX}%`, top: `${gp.renderY}%`, transform: 'translate(-50%, -50%)' }}>
                           <button 
                              onClick={(e) => { e.stopPropagation(); setActivePointId(gp.id); }}
                              className={`rounded-full border-[2px] border-white transition-all bg-black ${activePointId === gp.id ? 'scale-150 ring-4 ring-blue-500/30' : 'hover:scale-125'} ${gp.isLatestScan ? 'ring-4 ring-yellow-400/60 shadow-[0_0_12px_rgba(234,179,8,0.65)]' : ''}`}
                              style={{ 
                                width: `${12 / zoomLevel}px`, 
                                height: `${12 / zoomLevel}px`,
                                borderColor: gp.isLatestScan ? '#f59e0b' : (gp.maxRssi > -60 ? '#10b981' : '#3b82f6')
                              }}
                            />
                        </div>
                    ))}

                    {/* KALIBRATIE LIJN */}
                    {isCalibrating && (
                      <svg className="absolute inset-0 w-full h-full pointer-events-none z-[250]">
                        {rulerPoints.map((p, i) => <circle key={i} cx={`${p.x}%`} cy={`${p.y}%`} r={6/zoomLevel} fill="#10b981" stroke="white" strokeWidth={2/zoomLevel} />)}
                        {rulerPoints.length === 2 && <line x1={`${rulerPoints[0].x}%`} y1={`${rulerPoints[0].y}%`} x2={`${rulerPoints[1].x}%`} y2={`${rulerPoints[1].y}%`} stroke="#10b981" strokeWidth={3/zoomLevel} strokeDasharray={6/zoomLevel} />}
                      </svg>
                    )}
                  </div>
                </TransformComponent>
              </>
            )}
          </TransformWrapper>
        ) : <div className="text-slate-400 font-bold uppercase text-[11px]">Selecteer verdieping</div>}

        {isCalibrating && rulerPoints.length === 2 && (
          <div className="absolute bottom-10 bg-[#1e293b] p-6 rounded-3xl border border-slate-700 flex flex-col items-center gap-4 z-[400]">
            <div className="flex items-center gap-3 text-emerald-400 font-black text-xs uppercase tracking-widest"><Ruler size={20}/> Afstand instellen</div>
            <button onClick={handleConfirmCalibration} className="bg-blue-600 text-white px-10 py-3 rounded-2xl text-[11px] font-black uppercase shadow-xl hover:bg-blue-500 transition-all">Bevestig Meting</button>
          </div>
        )}
      </div>
    </div>
  );
}