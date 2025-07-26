function acquire() {
        const sensor = document.getElementById("sensor-select").value;
        const url = `/api/sensor/${sensor}`;
        fetch(url, { method: 'GET' 
        }).then(response => {
                if (!response.ok) {
                        throw new Error("API server error");
                }
        }).catch(error => {
                console.error("Acquisition failed: ", error);
        });

        setTimeout(load, 100);
}

function load() {
        fetch("/api/sensor/data"
        ).then(response => response.json()
        ).then(data => {
                const select = document.getElementById("measurement-select");
                select.innerHTML = "";
                data.forEach(item => {
                        const option = document.createElement("option");
                        option.value = item.id;
                        option.text = item.created_at;
                        select.appendChild(option);
                });
        }).catch(error => {
                console.error("Load failed: ", error);
        });
}

function calibrate() {
        const select = document.getElementById("measurement-select");
        const selected = select.value;
        if (!selected) {
                alert("Please select measurement before calibration.");
                return;
        }

        fetch(`/api/sensor/calibrate?id=${selected}`
        ).then(response => response.text()
        ).then(body => {
                alert(body);
        }).catch(error => {
                console.error("Calibration failed:", error);
        });
}

window.onload = () => {
    load();
};