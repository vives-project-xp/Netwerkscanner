import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import time
from matplotlib.widgets import Button
# gemaakt door chat-gpt
# pip install -r requirements.txt
# py visual_serial.py

BAUDRATE = 112500
UPDATE_INTERVAL = 0.1  # Iets sneller voor vloeiende weergave

# Pak eerste COM poort
ports = list(serial.tools.list_ports.comports())
if not ports:
    print("Geen COM-poorten gevonden!")
    exit()

port = ports[0].device
print(f"Gebruik COM-poort: {port}")

# Open seriele poort met een korte timeout om blokkeren te voorkomen
ser = serial.Serial(port, BAUDRATE, timeout=0.01)

# Data containers
aps = []
distances = []
temp_aps = []  # Tijdelijke opslag tijdens het inlezen
temp_dist = []
deviceX, deviceY, rmse = None, None, None

# Pauze variabele
paused = False

# Setup plot
plt.ion()
fig, ax = plt.subplots(figsize=(7, 5))
plt.subplots_adjust(bottom=0.2)  # ruimte voor de knop

# Callback functie voor pauze knop
def toggle_pause(event):
    global paused
    paused = not paused
    if paused:
        pause_button.label.set_text("Continue")
    else:
        pause_button.label.set_text("Pause")

# Voeg een pauze/continue knop toe
ax_pause = plt.axes([0.4, 0.05, 0.2, 0.075])  # [left, bottom, width, height]
pause_button = Button(ax_pause, "Pause")
pause_button.on_clicked(toggle_pause)

# Functie om plot te tekenen
def draw_plot():
    if not aps: 
        return
    
    ax.clear()
    # APs tekenen
    for i, (x, y) in enumerate(aps):
        ax.scatter(x, y, marker="s", c='blue', s=100)
        ax.text(x + 0.2, y + 0.2, f"AP{i}", fontsize=9)

    # Cirkels van APs
    for i, d in enumerate(distances):
        if i < len(aps):
            circle = patches.Circle(aps[i], d, fill=False, linestyle="--", edgecolor="gray", alpha=0.5)
            ax.add_patch(circle)

    # Device tekenen
    if deviceX is not None and deviceY is not None:
        # Maak de tekst met X, Y en RMSE
        text_str = f"Device\nX={deviceX:.2f}, Y={deviceY:.2f}"
        if rmse is not None:
            text_str += f"\n    RMSE={rmse:.2f}"  # voeg RMSE toe op een nieuwe regel

        ax.scatter(deviceX, deviceY, marker="x", s=200, c='red', linewidth=3)
        ax.text(deviceX + 0.2, deviceY + 0.2, text_str, 
                color='red', weight='bold', fontsize=9)

    # RMSE cirkel
    if deviceX is not None and deviceY is not None and rmse is not None:
        rmse_circle = patches.Circle((deviceX, deviceY), rmse, fill=True, color='green', alpha=0.1)
        ax.add_patch(rmse_circle)
        edge = patches.Circle((deviceX, deviceY), rmse, fill=False, linestyle=":", edgecolor="green")
        ax.add_patch(edge)

    ax.set_aspect("equal")
    ax.grid(True, linestyle=':', alpha=0.6)
    ax.set_title("Real-time Visualisatie")

    # Forceer update van de GUI
    fig.canvas.draw()
    fig.canvas.flush_events()

print("Luisteren naar Serial... (Sluit het venster om te stoppen)")

last_update = time.time()

try:
    while plt.fignum_exists(fig.number):
        # Lees alle beschikbare data uit de buffer
        while ser.in_waiting > 0:
            try:
                line = ser.readline().decode("utf-8", errors="ignore").strip()
            except:
                continue
                
            if not line: 
                continue

            # Parsen van de data
            if "start VisualTrilateratie" in line:
                temp_aps = []
                temp_dist = []
            elif "apX" in line:
                val = float(line.split("apX")[1])
                temp_aps.append([val, 0.0])
            elif "apY" in line:
                val = float(line.split("apY")[1])
                if temp_aps: temp_aps[-1][1] = val
            elif "distance" in line:
                val = float(line.split("distance")[1])
                temp_dist.append(val)
            elif "deviceX" in line:
                deviceX = float(line.split("deviceX")[1])
            elif "deviceY" in line:
                deviceY = float(line.split("deviceY")[1])
            elif "RMSE" in line:
                rmse = float(line.split("RMSE")[1])
            elif "end VisualTrilateratie" in line:
                aps = list(temp_aps)
                distances = list(temp_dist)
                
        # GUI blijft altijd reageren
        fig.canvas.flush_events()

        # Teken alleen nieuwe data als niet gepauzeerd en interval voorbij
        if not paused and (time.time() - last_update) > UPDATE_INTERVAL:
            draw_plot()
            last_update = time.time()
            
        time.sleep(0.01)

except KeyboardInterrupt:
    print("\nHandmatig gestopt.")
finally:
    ser.close()
    print("Serial verbinding verbroken.")
