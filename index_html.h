#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AIKENDO TECH SCADA - Plant Monitor</title>
    <!-- Chart.js CDN -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        :root {
            --bg-color: #0b132b;
            --card-bg: rgba(30, 41, 59, 0.7);
            --text-main: #f8fafc;
            --text-sub: #94a3b8;
            --accent-blue: #38bdf8;
            --accent-green: #10b981;
            --accent-yellow: #f59e0b;
            --accent-red: #ef4444;
            --border-color: rgba(255, 255, 255, 0.1);
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }

        body {
            background-color: var(--bg-color);
            color: var(--text-main);
            padding: 16px;
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            gap: 16px;
        }

        /* HEADER KOMPAK & LENGKAP */
        header {
            background: var(--card-bg);
            backdrop-filter: blur(10px);
            border: 1px solid var(--border-color);
            border-radius: 12px;
            padding: 10px 18px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            flex-wrap: wrap;
            gap: 12px;
        }

        .header-title h1 {
            font-size: 1.3rem;
            color: var(--accent-blue);
            font-weight: 800;
        }

        .header-title p {
            font-size: 0.7rem;
            color: var(--text-sub);
        }

        .header-right {
            display: flex;
            align-items: center;
            gap: 12px;
            flex-wrap: wrap;
        }

        /* MINI MEMORY BADGES IN HEADER */
        .mem-badge {
            background: rgba(15, 23, 42, 0.8);
            border: 1px solid var(--border-color);
            border-radius: 8px;
            padding: 4px 10px;
            font-size: 0.7rem;
            display: flex;
            flex-direction: column;
            gap: 2px;
            min-width: 110px;
        }

        .mem-title {
            color: var(--text-sub);
            font-size: 0.62rem;
            display: flex;
            justify-content: space-between;
        }

        .mem-val {
            font-weight: bold;
            color: var(--accent-blue);
        }

        .mem-bar-bg {
            background: rgba(255, 255, 255, 0.1);
            height: 4px;
            border-radius: 2px;
            overflow: hidden;
        }

        .mem-bar-fill {
            height: 100%;
            background: var(--accent-blue);
            width: 0%;
            transition: width 0.3s;
        }

        .status-badge {
            display: flex;
            align-items: center;
            gap: 6px;
            font-size: 0.75rem;
            background: rgba(15, 23, 42, 0.8);
            padding: 6px 12px;
            border-radius: 20px;
            border: 1px solid var(--border-color);
            font-weight: 600;
        }

        .status-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background-color: var(--accent-red);
        }

        .status-dot.active {
            background-color: var(--accent-green);
            box-shadow: 0 0 8px var(--accent-green);
        }

        /* FULLSCREEN BUTTON */
        .btn-fullscreen {
            background: rgba(56, 189, 248, 0.15);
            border: 1px solid rgba(56, 189, 248, 0.4);
            color: var(--accent-blue);
            border-radius: 8px;
            padding: 6px 12px;
            font-size: 0.75rem;
            font-weight: bold;
            cursor: pointer;
            display: flex;
            align-items: center;
            gap: 4px;
            transition: all 0.2s;
        }

        .btn-fullscreen:hover {
            background: var(--accent-blue);
            color: #000;
        }

        /* TANK CARDS DENGAN TANGKI LEBIH TINGGI */
        .tank-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
            gap: 12px;
        }

        .tank-card {
            background: var(--card-bg);
            border: 1px solid var(--border-color);
            border-radius: 12px;
            padding: 12px;
            display: flex;
            flex-direction: column;
            justify-content: space-between;
        }

        .tank-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 8px;
        }

        .tank-title {
            font-size: 0.95rem;
            font-weight: 700;
            color: var(--accent-blue);
        }

        .badge-status {
            font-size: 0.65rem;
            padding: 2px 6px;
            border-radius: 4px;
            font-weight: bold;
        }

        .badge-normal { background: rgba(16, 185, 129, 0.2); color: var(--accent-green); }
        .badge-low { background: rgba(239, 68, 68, 0.2); color: var(--accent-red); }
        .badge-high { background: rgba(245, 158, 11, 0.2); color: var(--accent-yellow); }

        /* VISUAL TANGKI DITINGKATKAN TINGGINYA (220px) */
        .visual-tank {
            width: 100%;
            height: 220px; /* Ditingkatkan dari 140px */
            border: 2px solid rgba(255, 255, 255, 0.2);
            border-radius: 0 0 12px 12px;
            position: relative;
            background: rgba(2, 6, 23, 0.85);
            overflow: hidden;
            display: flex;
            align-items: flex-end;
            margin-bottom: 10px;
        }

        .water-fill {
            width: 100%;
            background: linear-gradient(180deg, #38bdf8 0%, #0284c7 100%);
            transition: height 0.5s ease-in-out;
        }

        .water-percent {
            position: absolute;
            inset: 0;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 1.8rem;
            font-weight: 900;
            text-shadow: 0 2px 6px rgba(0,0,0,0.9);
        }

        /* HIGHLIGHT LEVEL UTAMA */
        .highlight-level {
            background: rgba(56, 189, 248, 0.15);
            border: 1px solid rgba(56, 189, 248, 0.4);
            border-radius: 8px;
            padding: 8px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 8px;
        }

        .highlight-title {
            font-size: 0.7rem;
            text-transform: uppercase;
            color: var(--accent-blue);
            font-weight: bold;
            display: flex;
            align-items: center;
            gap: 4px;
        }

        .highlight-val {
            font-size: 1.4rem;
            font-weight: 900;
            color: #ffffff;
        }

        .data-list {
            display: flex;
            flex-direction: column;
            gap: 4px;
            font-size: 0.8rem;
        }

        .data-item {
            display: flex;
            justify-content: space-between;
            border-bottom: 1px dashed rgba(255, 255, 255, 0.05);
            padding-bottom: 2px;
        }

        /* CONTAINER 3 GRAFIK */
        .charts-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
            gap: 16px;
        }

        .chart-card {
            background: var(--card-bg);
            border: 1px solid var(--border-color);
            border-radius: 12px;
            padding: 14px;
            height: 280px;
            position: relative;
        }

        .chart-title {
            font-size: 0.85rem;
            font-weight: 700;
            color: var(--accent-blue);
            margin-bottom: 8px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
    </style>
</head>
<body>

    <!-- HEADER DENGAN MEMORI & FULLSCREEN -->
    <header>
        <div class="header-title">
            <h1>AIKENDO TECH SCADA</h1>
            <p>Water Treatment Plant Monitoring System</p>
        </div>

        <div class="header-right">
            <!-- RAM MINI -->
            <div class="mem-badge">
                <div class="mem-title"><span>RAM</span><span id="ram-text">0/0 KB</span></div>
                <div class="mem-bar-bg"><div class="mem-bar-fill" id="ram-bar"></div></div>
            </div>

            <!-- FLASH MINI -->
            <div class="mem-badge">
                <div class="mem-title"><span>FLASH</span><span id="flash-text">0/0 MB</span></div>
                <div class="mem-bar-bg"><div class="mem-bar-fill" id="flash-bar"></div></div>
            </div>

            <!-- STATUS WS -->
            <div class="status-badge">
                <div id="status-dot" class="status-dot"></div>
                <span id="status-text">Disconnected</span>
            </div>

            <!-- TOMBOL FULLSCREEN -->
            <button class="btn-fullscreen" onclick="toggleFullScreen()">
                <span id="fs-icon">⛶</span> Fullscreen
            </button>
        </div>
    </header>

    <!-- TANK CARDS -->
    <div class="tank-grid" id="tank-container">
        <!-- Render via JavaScript -->
    </div>

    <!-- 3 GRAFIK REALTIME -->
    <div class="charts-grid">
        <div class="chart-card">
            <div class="chart-title">
                <span>RESERVOIR 1</span>
                <span style="font-size:0.7rem; color:var(--text-sub)">Level & Flow vs Waktu</span>
            </div>
            <canvas id="chart0"></canvas>
        </div>
        <div class="chart-card">
            <div class="chart-title">
                <span>RESERVOIR 2</span>
                <span style="font-size:0.7rem; color:var(--text-sub)">Level & Flow vs Waktu</span>
            </div>
            <canvas id="chart1"></canvas>
        </div>
        <div class="chart-card">
            <div class="chart-title">
                <span>CWT 3</span>
                <span style="font-size:0.7rem; color:var(--text-sub)">Level & Flow vs Waktu</span>
            </div>
            <canvas id="chart2"></canvas>
        </div>
    </div>

    <script>
        const gateway = `ws://${window.location.host}/ws`;
        let websocket;
        const charts = [];
        const maxDataPoints = 25;

        // FULLSCREEN TOGGLE FUNCTION
        function toggleFullScreen() {
            if (!document.fullscreenElement) {
                document.documentElement.requestFullscreen().catch(err => {
                    alert(`Gagal masuk Fullscreen: ${err.message}`);
                });
            } else {
                if (document.exitFullscreen) {
                    document.exitFullscreen();
                }
            }
        }

        function createChartConfig() {
            return {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [
                        {
                            label: 'Level (cm)',
                            data: [],
                            borderColor: '#38bdf8',
                            backgroundColor: '#38bdf8',
                            borderWidth: 2,
                            pointRadius: 0,
                            tension: 0.3,
                            yAxisID: 'yLevel'
                        },
                        {
                            label: 'Flow (L/s)',
                            data: [],
                            borderColor: '#10b981',
                            backgroundColor: '#10b981',
                            borderWidth: 1.5,
                            borderDash: [4, 4],
                            pointRadius: 0,
                            tension: 0.3,
                            yAxisID: 'yFlow'
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    animation: { duration: 200 },
                    scales: {
                        x: {
                            grid: { color: 'rgba(255, 255, 255, 0.05)' },
                            ticks: { color: '#94a3b8', font: { size: 9 } }
                        },
                        yLevel: {
                            type: 'linear',
                            position: 'left',
                            beginAtZero: true,
                            grid: { color: 'rgba(255, 255, 255, 0.05)' },
                            ticks: { color: '#38bdf8', font: { size: 9 } },
                            title: { display: true, text: 'cm', color: '#38bdf8', font: { size: 10 } }
                        },
                        yFlow: {
                            type: 'linear',
                            position: 'right',
                            grid: { drawOnChartArea: false },
                            ticks: { color: '#10b981', font: { size: 9 } },
                            title: { display: true, text: 'L/s', color: '#10b981', font: { size: 10 } }
                        }
                    },
                    plugins: {
                        legend: {
                            labels: { color: '#f8fafc', font: { size: 10 }, boxWidth: 12 }
                        }
                    }
                }
            };
        }

        function initCharts() {
            for (let i = 0; i < 3; i++) {
                const ctx = document.getElementById(`chart${i}`).getContext('2d');
                charts[i] = new Chart(ctx, createChartConfig());
            }
        }

        function initWebSocket() {
            websocket = new WebSocket(gateway);
            websocket.onopen = onOpen;
            websocket.onclose = onClose;
            websocket.onmessage = onMessage;
        }

        function onOpen(event) {
            document.getElementById("status-dot").className = "status-dot active";
            document.getElementById("status-text").innerText = "Connected";
        }

        function onClose(event) {
            document.getElementById("status-dot").className = "status-dot";
            document.getElementById("status-text").innerText = "Disconnected";
            setTimeout(initWebSocket, 2000);
        }

        function onMessage(event) {
            try {
                const data = JSON.parse(event.data);

                // Update Header Memory Info
                if(data.sys) {
                    const ramUsed = (data.sys.ram_total - data.sys.ram_free) / 1024;
                    const ramTotal = data.sys.ram_total / 1024;
                    const ramPercent = (ramUsed / ramTotal) * 100;
                    document.getElementById('ram-text').innerText = `${ramUsed.toFixed(0)}KB`;
                    document.getElementById('ram-bar').style.width = `${ramPercent}%`;

                    const flashUsedMB = (data.sys.flash_used / (1024 * 1024)).toFixed(1);
                    const flashTotalMB = (data.sys.flash_total / (1024 * 1024)).toFixed(1);
                    const flashPercent = (data.sys.flash_used / data.sys.flash_total) * 100;
                    document.getElementById('flash-text').innerText = `${flashUsedMB}/${flashTotalMB}MB`;
                    document.getElementById('flash-bar').style.width = `${flashPercent}%`;
                }

                // Update Tanks & Charts
                if (data.tanks) {
                    renderTanks(data.tanks);
                    updateCharts(data.tanks);
                }
            } catch(e) {
                console.error("JSON Error:", e);
            }
        }

        function renderTanks(tanks) {
            const container = document.getElementById("tank-container");
            container.innerHTML = "";

            tanks.forEach((tank) => {
                const percent = Math.min(100, Math.max(0, (tank.cm / tank.maxH) * 100)).toFixed(1);
                
                let badgeClass = "badge-normal";
                let badgeText = "NORMAL";
                if(percent < 15) { badgeClass = "badge-low"; badgeText = "LOW"; }
                else if(percent > 85) { badgeClass = "badge-high"; badgeText = "HIGH"; }

                // Panah Flow Status
                let arrowIcon = '<span style="color:#94a3b8">➔</span>';
                let flowColor = "var(--text-sub)";
                if (tank.flow > 0.001) {
                    arrowIcon = '<span style="color:var(--accent-green)">▲</span>';
                    flowColor = "var(--accent-green)";
                } else if (tank.flow < -0.001) {
                    arrowIcon = '<span style="color:var(--accent-red)">▼</span>';
                    flowColor = "var(--accent-red)";
                }

                const card = `
                    <div class="tank-card">
                        <div>
                            <div class="tank-header">
                                <div class="tank-title">${tank.name}</div>
                                <span class="badge-status ${badgeClass}">${badgeText}</span>
                            </div>
                            <!-- TANGKI VISUAL LEBIH TINGGI -->
                            <div class="visual-tank">
                                <div class="water-fill" style="height: ${percent}%;"></div>
                                <div class="water-percent">${percent}%</div>
                            </div>
                        </div>

                        <!-- HIGHLIGHT LEVEL UTAMA -->
                        <div class="highlight-level">
                            <div>
                                <div class="highlight-title">Level Utama ${arrowIcon}</div>
                                <span style="font-size:0.65rem; color:var(--text-sub)">Ketinggian Air</span>
                            </div>
                            <div>
                                <span class="highlight-val">${tank.cm.toFixed(1)}</span>
                                <span style="font-size:0.75rem; color:var(--accent-blue); font-weight:bold;">cm</span>
                            </div>
                        </div>

                        <div class="data-list">
                            <div class="data-item">
                                <span style="color:var(--text-sub)">Volume:</span>
                                <b>${(tank.vol/1000).toFixed(2)} m³</b>
                            </div>
                            <div class="data-item">
                                <span style="color:var(--text-sub)">Flow Rate:</span>
                                <b style="color:${flowColor}">${tank.flow.toFixed(2)} L/s</b>
                            </div>
                            <div class="data-item">
                                <span style="color:var(--text-sub)">Current 4-20mA:</span>
                                <b style="color:var(--accent-yellow)">${tank.ma.toFixed(2)} mA</b>
                            </div>
                        </div>
                    </div>
                `;
                container.innerHTML += card;
            });
        }

        function updateCharts(tanks) {
            const now = new Date();
            const timeLabel = now.getHours().toString().padStart(2, '0') + ':' + 
                              now.getMinutes().toString().padStart(2, '0') + ':' + 
                              now.getSeconds().toString().padStart(2, '0');

            for (let i = 0; i < 3; i++) {
                if (!tanks[i]) continue;

                const chart = charts[i];

                chart.data.labels.push(timeLabel);
                if (chart.data.labels.length > maxDataPoints) chart.data.labels.shift();

                chart.data.datasets[0].data.push(tanks[i].cm);
                if (chart.data.datasets[0].data.length > maxDataPoints) chart.data.datasets[0].data.shift();

                chart.data.datasets[1].data.push(tanks[i].flow);
                if (chart.data.datasets[1].data.length > maxDataPoints) chart.data.datasets[1].data.shift();

                chart.update();
            }
        }

        window.addEventListener("load", () => {
            initCharts();
            initWebSocket();
        });
    </script>
</body>
</html>
)rawliteral";

#endif