
const char PAGE_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sensor Dashboard</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
        h1 {
            color: #333;
            text-align: center;
        }
        #sensorData {
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <h1>Sensor Dashboard</h1>
    <div id="sensorData">Loading sensor data...</div>

    <script>
        function updateSensorData() {
            fetch('/sensorData')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('sensorData').innerHTML = `
                        <p>Water Level RAW value: ${data.waterSensorValueRAW}</p>
                        <p>Water level Percentage: ${data.water_level_percentage}%</p>
                        <p>Water level mm: ${data.water_level_mm} mm</p>
                    `;
                })
                .catch(error => console.error('Error:', error));
        }

        // Update sensor data every 1 seconds
        setInterval(updateSensorData, 1000);
        // Initial update
        updateSensorData();
    </script>
</body>
</html>
)=====";