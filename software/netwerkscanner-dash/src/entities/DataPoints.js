export const createDataPoint = (ssid, rssi, x, y) => ({
  id: crypto.randomUUID(),
  ssid,
  rssi,
  x,
  y,
  timestamp: new Date().toISOString()
});