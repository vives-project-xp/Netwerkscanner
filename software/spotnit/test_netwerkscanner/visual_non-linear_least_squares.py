import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np
import re

def teken_geavanceerde_visualisatie():
    print("Plak de volledige output van je programma (inclusief de Punten):")
    
    input_lines = []
    while True:
        line = input()
        if not line.strip(): break # Stopt bij een lege regel
        input_lines.append(line)
        if "Hoek (graden):" in line:
            # We lezen nog even door voor de punten als die er direct onder staan
            continue 
        # Als er na de hoek nog punten komen, lezen we die ook. 
        # In dit script stoppen we als de gebruiker een extra Enter geeft.

    input_text = "\n".join(input_lines)

    try:
        # Basis data extraheren
        x_est = float(re.search(r"locatie: \(([-\d.]+), ([-\d.]+)\)", input_text).group(1))
        y_est = float(re.search(r"locatie: \(([-\d.]+), ([-\d.]+)\)", input_text).group(2))
        major = float(re.search(r"Hoofdas \(lengte\): ([-\d.]+)", input_text).group(1))
        minor = float(re.search(r"Nevenas \(breedte\): ([-\d.]+)", input_text).group(1))
        angle = float(re.search(r"Hoek \(graden\):\s+([-\d.]+)", input_text).group(1))

        # Punten extraheren: "Punt: 0,0 -> Afstand: 7.1"
        punten = re.findall(r"Punt:\s+([-\d.]+),([-\d.]+)\s+->\s+Afstand:\s+([-\d.]+)", input_text)
    except AttributeError:
        print("\nFout: Kon de gegevens niet goed lezen. Controleer het formaat.")
        return

    fig, ax = plt.subplots(figsize=(12, 10))

    # 1. Teken de Access Points en hun gemeten afstanden (cirkels)
    for i, (px, py, dist) in enumerate(punten):
        px, py, dist = float(px), float(py), float(dist)
        # Het fysieke punt
        ax.scatter(px, py, color='black', marker='s', s=80)
        ax.text(px, py + 0.5, f"AP{i+1}", fontsize=10, ha='center')
        # De afstandscirkel (geeft aan waar het punt zou moeten liggen volgens dit AP)
        cirkel = plt.Circle((px, py), dist, color='blue', fill=False, linestyle=':', alpha=0.3)
        ax.add_patch(cirkel)

    # 2. Teken de Betrouwbaarheidsovaal
    # Matplotlib gebruikt breedte/hoogte (2x de as-lengte)
    ellips = patches.Ellipse(
        (x_est, y_est), width=major*2, height=minor*2, angle=angle,
        edgecolor='red', facecolor='red', alpha=0.15, linewidth=2, label='95% Betrouwbaarheid'
    )
    ax.add_patch(ellips)
    
    # Rand van de ellips
    rand = patches.Ellipse(
        (x_est, y_est), width=major*2, height=minor*2, angle=angle,
        edgecolor='red', facecolor='none', linewidth=2, linestyle='--'
    )
    ax.add_patch(rand)

    # 3. De geschatte locatie
    ax.scatter(x_est, y_est, color='green', marker='X', s=150, label=f'Schatting ({x_est:.2f}, {y_est:.2f})', zorder=5)

    # Layout instellingen
    ax.set_aspect('equal')
    
    # Bepaal grenzen op basis van alle data (punten + ellips)
    all_x = [float(p[0]) for p in punten] + [x_est]
    all_y = [float(p[1]) for p in punten] + [y_est]
    
    # Zoom een beetje uit om de ellips te laten passen, maar niet té ver als de ellips 71m is
    limit_margin = max(minor * 4, 15) 
    ax.set_xlim(min(all_x) - limit_margin, max(all_x) + limit_margin)
    ax.set_ylim(min(all_y) - limit_margin, max(all_y) + limit_margin)

    ax.grid(True, which='both', linestyle='--', alpha=0.5)
    ax.axhline(0, color='black', linewidth=0.5)
    ax.axvline(0, color='black', linewidth=0.5)
    
    ax.set_title("Trilateratie: Locatie Punten & Onzekerheidsveld", fontsize=14)
    ax.legend(loc='upper right')

    print("\nGrafiek gegenereerd.")
    plt.show()

if __name__ == "__main__":
    teken_geavanceerde_visualisatie()