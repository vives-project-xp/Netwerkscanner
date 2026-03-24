export const createBuilding = (name, location) => ({
  id: crypto.randomUUID(),
  name: name,
  location: location,
  createdAt: new Date().toISOString(),
  floors: [] 
});