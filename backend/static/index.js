function applyFilters() {
    const node = document.getElementById("nodeFilter").value;
    const sensor = document.getElementById("sensorFilter").value;

    document.querySelectorAll("#data-table tbody tr").forEach(row => {
        const showNode = !node || row.dataset.node === node;
        const showSensor = !sensor || row.dataset.sensor === sensor;
        row.style.display = (showNode && showSensor) ? "" : "none";
    });
}

document.getElementById("nodeFilter").addEventListener("change", applyFilters);
document.getElementById("sensorFilter").addEventListener("change", applyFilters);

// 🔄 Subtle auto-refresh (table only)
async function refreshTable() {
    const res = await fetch("/");
    const html = await res.text();

    const parser = new DOMParser();
    const doc = parser.parseFromString(html, "text/html");

    const newBody = doc.querySelector("#data-table tbody");
    document.querySelector("#data-table tbody").innerHTML = newBody.innerHTML;

    applyFilters();
}

// refresh la 10 secunde
setInterval(refreshTable, 60000);
