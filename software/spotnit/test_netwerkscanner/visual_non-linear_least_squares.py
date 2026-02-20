import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import time
import math
from matplotlib.widgets import Button

BAUDRATE = 112500
UPDATE_INTERVAL = 0.1

# Zoek COM-poort
ports = list(serial.tools.list_ports.comports())
if not ports:
    print("Geen COM-poorten gevonden!")
    exit()

port = ports[0].device
print(f"Gebruik COM-poort: {port}")

ser = serial.Serial(port, BAUDRATE, timeout=0.01)

# Data containers
aps = []
distances = []

deviceX = None
deviceY = None
hoofdas = None
nevenas = None
hoek = None

paused = False

plt.ion()
fig, ax = plt.subplots(figsize=(7, 5))
plt.subplots_adjust(bottom=0.2)

def toggle_pause(event):
    global paused
    paused = not paused
    pause_button.label.set_text("Continue" if paused else "Pause")

ax_pause = plt.axes([0.4, 0.05, 0.2, 0.075])
pause_button = Button(ax_pause, "Pause")
pause_button.on_clicked(toggle_pause)

def draw_plot():
    ax.clear()

    # AP's tekenen
    for i, (x, y) in enumerate(aps):
        ax.scatter(x, y, marker="s", s=100)
        if i < len(distances):
            ax.text(x + 0.2, y + 0.2, f"AP{i} {distances[i]:.2f}", fontsize=9)

    # Afstandscirkels
    for i, d in enumerate(distances):
        if i < len(aps):
            circle = patches.Circle(aps[i], d, fill=False, linestyle="--", alpha=0.4)
            ax.add_patch(circle)

    # Device tekenen
    if deviceX is not None and deviceY is not None:
        ax.scatter(deviceX, deviceY, marker="x", s=200, linewidth=3)
        ax.text(deviceX + 0.2, deviceY + 0.2,
                f"Device\nX={deviceX:.2f}, Y={deviceY:.2f}",
                fontsize=9)

    # 95% Ovaal tekenen
    if (deviceX is not None and deviceY is not None and
        hoofdas is not None and nevenas is not None and hoek is not None):

        ellipse = patches.Ellipse(
            (deviceX, deviceY),
            width=2 * nevenas,
            height=2 * hoofdas,
            angle=hoek * 180.0 / math.pi,
            fill=False,
            linestyle="--"
        )
        ax.add_patch(ellipse)

    ax.set_aspect("equal")
    ax.grid(True, linestyle=":")
    ax.set_title("95% Betrouwbaarheidsvisualisatie")

    fig.canvas.draw()
    fig.canvas.flush_events()

print("Luisteren naar Serial...")

last_update = time.time()

try:
    while plt.fignum_exists(fig.number):

        while ser.in_waiting > 0:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if not line:
                continue

            # Nieuwe dataset start
            if "--- 95% Betrouwbaarheidsovaal ---" in line:
                aps = []
                distances = []

            # Device locatie
            elif "Geschatte locatie:" in line:
                coords = line.split("(")[1].split(")")[0]
                x_str, y_str = coords.split(",")
                deviceX = float(x_str)
                deviceY = float(y_str)

            # Hoofdas
            elif "Hoofdas (lengte):" in line:
                hoofdas = float(line.split(":")[1].strip())

            # Nevenas
            elif "Nevenas (breedte):" in line:
                nevenas = float(line.split(":")[1].strip())

            # Hoek radialen
            elif "Hoek (radialen):" in line:
                value_str = line.split(":")[1].replace("rad", "").strip()
                hoek = float(value_str)

            # Punt + Afstand
            elif "Punt:" in line:
                part = line.split("Punt:")[1].strip()
                coords_part, dist_part = part.split("->")
                x_str, y_str = coords_part.strip().split(",")
                d = float(dist_part.split(":")[1])

                aps.append((float(x_str), float(y_str)))
                distances.append(d)

        fig.canvas.flush_events()

        if not paused and (time.time() - last_update) > UPDATE_INTERVAL:
            draw_plot()
            last_update = time.time()

        time.sleep(0.01)

except KeyboardInterrupt:
    print("Gestopt.")

finally:
    ser.close()
    print("Serial verbinding verbroken.")