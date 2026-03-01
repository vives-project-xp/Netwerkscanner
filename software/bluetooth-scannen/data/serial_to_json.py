#gebruik python data/serial_to_json.py COM4 115200 om de scan resultaten te noteren in scan.json

import sys
import json
import serial
import time
from pathlib import Path

if len(sys.argv) < 2:
    print("Usage: python serial_to_json.py <port> [baud]")
    sys.exit(1)

port = sys.argv[1]
baud = int(sys.argv[2]) if len(sys.argv) > 2 else 115200

out_path = Path(__file__).parent / "scan.json"

# ensure file exists and start fresh on each run
if not out_path.exists():
    out_path.write_text('[]')

# start with empty list every time the script starts
data = []
with open(out_path, 'w', encoding='utf-8') as f:
    json.dump(data, f, indent=2)

print(f"Opening serial {port}@{baud}")
ser = serial.Serial(port, baud, timeout=1)

buffer = []
in_scan = False

try:
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if not line:
            continue
        if line == "SCAN_START":
            # clear previous results so file always reflects only the current scan
            buffer = []
            data = []
            with open(out_path, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=2)
            in_scan = True
            continue
        if line == "SCAN_END":
            # write the current buffer directly (no dedup, we want exactly what was seen)
            if buffer:
                data = buffer[:]
                with open(out_path, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=2)
                print(f"Wrote {len(buffer)} devices from this scan")
            else:
                print("Scan finished but no devices were found")
            in_scan = False
            continue
        if in_scan:
            # expect JSON object on this line
            try:
                obj = json.loads(line)
                buffer.append(obj)
            except json.JSONDecodeError:
                print("Warning: malformed JSON", line)
        else:
            # print other status messages to console
            print(line)
except KeyboardInterrupt:
    print("Stopping")
    ser.close()
    sys.exit(0)
