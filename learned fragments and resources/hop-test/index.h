#ifndef INDEX_H
#define INDEX_H

String getIndexHTML() {
  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>HOME-Plantech</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f0f0f0; }
        .container { max-width: 800px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        h1, h2 { color: #333; }
        .info-box { background-color: #e9f7ef; border: 1px solid #27ae60; border-radius: 5px; padding: 15px; margin-bottom: 20px; }
        .info-box p { margin: 5px 0; }
        .navbar-nav { display: flex; justify-content: space-around; background-color: #333; padding: 10px; }
        .nav-link { color: white; text-decoration: none; padding: 10px 20px; font-size: 18px; border-radius: 5px; }
        .nav-link.active { background-color: #555; }
        .nav-link:hover { background-color: #777; color: #fff; }
    </style>
</head>
<body>
    <div class="navbar-nav">
        <a class="nav-link active" aria-current="page" href="/">Home</a>
        <a class="nav-link" href="/connectivity">Connectivity</a>
        <a class="nav-link" href="/control">Control</a>
    </div>

    <div class="container">
        <h1>PlanTech Dashboard</h1>
        
        <div class="info-box">
            <h2>Soil Moisture</h2>
            <p>Moisture: <span id="soilMoisture"></span>%</p>
            <p>Raw Value: <span id="soilMoistureRaw"></span></p>
            <p>Pump Threshold: <span id="pumpThreshold"></span>%</p>
        </div>

        <div class="info-box">
            <h2>Water Level</h2>
            <p>Level: <span id="waterLevelPercentage"></span>%</p>
            <p>Height: <span id="waterLevelMM"></span> mm</p>
            <p>Raw Value: <span id="waterLevelRaw"></span></p>
        </div>

        <div class="info-box">
            <h2>Environmental Data</h2>
            <p>Temperature: <span id="temperature"></span> °C</p>
            <p>Humidity: <span id="humidity"></span>%</p>
            <p>Heat Index: <span id="heatIndex"></span> °C</p>
        </div>

        <div class="info-box">
            <h2>System Info</h2>
            <p>Pump Activations: <span id="pumpActivationCount"></span></p>
            <p>Uptime: <span id="uptime"></span></p>
            <p>Date & Time: <span id="dateTime"></span></p>
        </div>
    </div>

    <script>
        function updateData() {
            fetch('/indexData')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('soilMoisture').textContent = data.soilMoisture;
                    document.getElementById('soilMoistureRaw').textContent = data.soilMoistureRaw;
                    document.getElementById('pumpThreshold').textContent = data.pumpThreshold;
                    document.getElementById('waterLevelPercentage').textContent = data.waterLevelPercentage;
                    document.getElementById('waterLevelMM').textContent = data.waterLevelMM;
                    document.getElementById('waterLevelRaw').textContent = data.waterLevelRaw;
                    document.getElementById('temperature').textContent = data.temperature;
                    document.getElementById('humidity').textContent = data.humidity;
                    document.getElementById('heatIndex').textContent = data.heatIndex;
                    document.getElementById('pumpActivationCount').textContent = data.pumpActivationCount;
                    document.getElementById('uptime').textContent = formatUptime(data.uptime);
                    document.getElementById('dateTime').textContent = data.dateTime;
                });
        }

        function formatUptime(ms) {
            let seconds = Math.floor(ms / 1000);
            let minutes = Math.floor(seconds / 60);
            let hours = Math.floor(minutes / 60);
            let days = Math.floor(hours / 24);

            return `${days}d ${hours%24}h ${minutes%60}m ${seconds%60}s`;
        }

        setInterval(updateData, 5000);
        updateData();
    </script>
</body>
</html>
  )";
  return html;
}

#endif // INDEX_H