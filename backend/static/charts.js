window.chartData.forEach(cfg => {
    const ctx = document.getElementById(cfg.id);

    new Chart(ctx, {
        type: "line",
        data: {
            labels: cfg.data.map(p => p.t),
            datasets: [{
                label: cfg.label,
                data: cfg.data.map(p => p.v),
                borderWidth: 2,
                tension: 0.3
            }]
        },
        options: {
            responsive: true,
            scales: {
                x: { ticks: { color: "#cbd5f5" } },
                y: { ticks: { color: "#cbd5f5" } }
            },
            plugins: {
                legend: { labels: { color: "#e5e7eb" } }
            }
        }
    });
});
