const SERVER = "http://172.16.0.24:1880";

///////////////////////////////////////////////////////////
// 1️⃣ AGGIORNAMENTO DATI (GET /serra)
///////////////////////////////////////////////////////////

async function updateData() {
    try {
        const response = await fetch(`${SERVER}/serra`);
        const data = await response.json();

        // GLOBAL
        document.getElementById("avgTemp").textContent = data.avgTemp + "°C";
        document.getElementById("avgHumidity").textContent = data.avgHumidity + "%";
        document.getElementById("globalLight").textContent = data.globalLight + " lux";

        // MODE
        setModeUI(data.mode);

        // PUMPS
        for (let i = 1; i <= 4; i++) {
            updatePumpUI(i, data.pumps[i]);
        }

        // FANS
        for (let i = 1; i <= 2; i++) {
            updateFanUI(i, data.fans[i]);
        }

        // EMERGENCY
        updateEmergencyUI(data.emergency);

    } catch (err) {
        console.error("Errore updateData:", err);
    }
}

// aggiorna ogni 2 secondi (non 500ms)
setInterval(updateData, 2000);
updateData(); // primo avvio


///////////////////////////////////////////////////////////
// 2️⃣ INVIO COMANDI A NODE-RED
///////////////////////////////////////////////////////////

async function sendCommand(endpoint, payload) {
    try {
        await fetch(`${SERVER}/${endpoint}`, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(payload)
        });
    } catch (err) {
        console.error("Errore sendCommand:", err);
    }
}


///////////////////////////////////////////////////////////
// 3️⃣ POMPE
///////////////////////////////////////////////////////////

function togglePump(padNum) {
    const speed = document.getElementById(`pumpSlider${padNum}`).value;

    sendCommand("pump", {
        pump: padNum,
        speed: parseInt(speed)
    });
}

function updatePumpUI(padNum, pumpData) {
    const status = document.getElementById(`pumpStatus${padNum}`);
    const slider = document.getElementById(`pumpSlider${padNum}`);
    const value = document.getElementById(`pumpValue${padNum}`);

    slider.value = pumpData.speed;
    value.textContent = pumpData.speed;

    if (pumpData.on) {
        status.classList.add("active");
        status.classList.remove("inactive");
        status.textContent = "ON";
    } else {
        status.classList.add("inactive");
        status.classList.remove("active");
        status.textContent = "OFF";
    }
}


///////////////////////////////////////////////////////////
// 4️⃣ VENTOLE
///////////////////////////////////////////////////////////

function toggleFan(fanNum) {
    const speed = document.getElementById(`fanSlider${fanNum}`).value;

    sendCommand("fan", {
        fan: fanNum,
        speed: parseInt(speed)
    });
}

function updateFanUI(fanNum, fanData) {
    const status = document.getElementById(`fanStatus${fanNum}`);
    const slider = document.getElementById(`fanSlider${fanNum}`);
    const value = document.getElementById(`fanValue${fanNum}`);

    slider.value = fanData.speed;
    value.textContent = fanData.speed;

    if (fanData.on) {
        status.classList.add("active");
        status.classList.remove("inactive");
        status.textContent = "ON";
    } else {
        status.classList.add("inactive");
        status.classList.remove("active");
        status.textContent = "OFF";
    }
}


///////////////////////////////////////////////////////////
// 5️⃣ MODALITÀ
///////////////////////////////////////////////////////////

function setMode(mode) {
    sendCommand("mode", { mode: mode });
    setModeUI(mode);
}

function setModeUI(mode) {
    document.querySelectorAll('.mode-btn').forEach(btn => {
        btn.classList.remove("active");
    });

    if (mode === "auto") {
        document.querySelector(".mode-btn:nth-child(1)").classList.add("active");
    } else {
        document.querySelector(".mode-btn:nth-child(2)").classList.add("active");
    }
}


///////////////////////////////////////////////////////////
// 6️⃣ EMERGENCY
///////////////////////////////////////////////////////////

function updateEmergencyUI(isEmergency) {
    const emergency = document.getElementById("emergency");

    if (isEmergency) {
        emergency.textContent = "EMERGENCY!";
        emergency.classList.add("triggered");
    } else {
        emergency.textContent = "NORMAL";
        emergency.classList.remove("triggered");
    }
}
/*
json da mettere in Node-RED per far funzionare
{
  "avgTemp": 25.4,
  "avgHumidity": 67,
  "globalLight": 780,
  "mode": "auto",
  "emergency": false,
  "pumps": {
    "1": { "on": true,  "speed": 70 },
    "2": { "on": false, "speed": 0 },
    "3": { "on": true,  "speed": 50 },
    "4": { "on": false, "speed": 0 }
  },
  "fans": {
    "1": { "on": true,  "speed": 40 },
    "2": { "on": false, "speed": 0 }
  }
}
*/