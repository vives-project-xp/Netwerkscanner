async function loadData() {
    try {
        const response = await fetch("../data/scan.json?t=" + new Date().getTime());
        const data = await response.json();

        // Sorteer op sterkste signaal (hoogste RSSI eerst)
        data.sort((a, b) => b.rssi - a.rssi);

        const tableBody = document.querySelector("#deviceTable tbody");
        tableBody.innerHTML = "";

        data.forEach(device => {
            const row = document.createElement("tr");

            const name = device.name && device.name !== "" ? device.name : "Onbekend";

            const signal = getSignalInfo(device.rssi);

            row.innerHTML = `
                <td>${name}</td>
                <td>${device.address}</td>
                <td>
                    <div class="signal-wrapper">
                        <div class="signal-text">${device.rssi} dBm (${signal.label})</div>
                        <div class="signal-bar" 
                             style="width:${signal.percent}%; background-color:${signal.color};">
                        </div>
                    </div>
                </td>
            `;

            tableBody.appendChild(row);
        });

    } catch (error) {
        console.error("Fout bij laden JSON:", error);
    }
}

function getSignalInfo(rssi) {

    const percent = Math.min(Math.max((rssi + 100) * 1.6, 5), 100);

    let color;
    let label;

    if (rssi > -60) {
        color = "#22c55e";
        label = "Sterk";
    } else if (rssi > -75) {
        color = "#eab308";
        label = "Matig";
    } else {
        color = "#ef4444";
        label = "Zwak";
    }

    return { percent, color, label };
}

loadData();
//refresh elke 5 seconden
setInterval(loadData, 5000);