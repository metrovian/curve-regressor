function acquire() {
        const sensor = document.getElementById("sensor-select").value;
        const url = `/api/sensor/${sensor}`;
        fetch(url, { method: 'GET' 
        }).then(response => {
                if (!response.ok) {
                        throw new Error("API server error");
                }
        }).catch(error => {
                console.error("Acquisition failed:", error);
        });
}
