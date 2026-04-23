import React, { useState } from 'react';
import { ArrowLeft, Plus, Trash2, Upload, Map as MapIcon } from 'lucide-react';

export default function BuildingDetail({ building, onBack, onAddFloor, onDeleteFloor }) {

  const [fName, setFName] = useState('');
  const [fImage, setFImage] = useState(null);
  const [fFile, setFFile] = useState(null);
  const [uploading, setUploading] = useState(false);

  const handleFile = (e) => {
    const file = e.target.files[0];
    if (file) {
      setFFile(file);
      const reader = new FileReader();
      reader.onloadend = () => setFImage(reader.result);
      reader.readAsDataURL(file);
    }
  };

  const handleAddFloor = async (e) => {
    e.preventDefault();
    let imagePath = null;
    if (fFile) {
      setUploading(true);
      const formData = new FormData();
      formData.append('file', fFile);
      try {
        const res = await fetch('/upload', {
          method: 'POST',
          body: formData
        });
        const data = await res.json();
        imagePath = data.url;
      } catch (err) {
        console.error('Uploaden mislukt:', err);
        alert('Uploaden mislukt');
      }
      setUploading(false);
    }
    onAddFloor(building.id, fName, imagePath);
    setFName('');
    setFImage(null);
    setFFile(null);
  };

  return (
    <div className="p-8 space-y-8">
      <button onClick={onBack} className="flex items-center gap-2 text-slate-400 hover:text-blue-600 font-bold text-sm">
        <ArrowLeft size={16} /> Terug naar overzicht
      </button>

      <div className="bg-white p-8 rounded-3xl border border-slate-200 flex justify-between items-center">
        <div>
          <h1 className="text-2xl font-black text-slate-800 italic uppercase">{building.name}</h1>
          <p className="text-slate-500 text-sm">{building.location}</p>
        </div>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
        <div className="bg-white p-6 rounded-3xl border border-slate-200 h-fit">
          <h3 className="font-bold mb-6 italic uppercase text-sm tracking-widest text-blue-600">Verdieping Toevoegen</h3>
          <form className="space-y-4" onSubmit={handleAddFloor}>
            <input required className="w-full p-4 bg-slate-50 rounded-xl text-sm outline-none" placeholder="Naam (bijv. 1e Verdieping)" value={fName} onChange={e => setFName(e.target.value)} />
            <div className="relative">
              <input type="file" accept="image/*" id="img-up" className="hidden" onChange={handleFile} />
              <label htmlFor="img-up" className="flex flex-col items-center justify-center w-full h-32 border-2 border-dashed border-slate-200 rounded-2xl cursor-pointer hover:bg-slate-50">
                {fImage ? <img src={fImage} className="h-full w-full object-cover rounded-2xl opacity-50" /> : <div className="text-center"><Upload size={20} className="mx-auto text-slate-300"/><span className="text-[10px] font-bold text-slate-400">UPLOAD TEKENING</span></div>}
              </label>
            </div>
            <button type="submit" className="w-full bg-blue-600 text-white p-4 rounded-xl font-bold text-xs uppercase" disabled={uploading}>{uploading ? 'Bezig met uploaden...' : 'Opslaan'}</button>
          </form>
        </div>

        <div className="lg:col-span-2 space-y-4">
          {building.floors?.map(f => (
            <div key={f.id} className="bg-white p-4 rounded-2xl border border-slate-100 flex items-center justify-between">
              <div className="flex items-center gap-4">
                <div className="w-16 h-16 bg-slate-100 rounded-lg overflow-hidden border">
                  {f.image ? <img src={f.image} className="w-full h-full object-cover" /> : <MapIcon className="m-auto text-slate-300" />}
                </div>
                <h4 className="font-bold">{f.name}</h4>
              </div>
              <button onClick={() => onDeleteFloor(building.id, f.id)} className="p-3 text-slate-300 hover:text-red-500 transition-colors">
                <Trash2 size={20} />
              </button>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
}