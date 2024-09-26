const char PAGE_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>DHT11 Sensor Dashboard</title>
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
        .error {
            color: red;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <h1>DHT11 Sensor Dashboard</h1>
    <div id="sensorData">Loading sensor data...</div>

    <script>
        function updateSensorData() {
            fetch('/sensorData')
                .then(response => response.json())
                .then(data => {
                    if (data.error) {
                        document.getElementById('sensorData').innerHTML = `
                            <p class="error">${data.error}</p>
                        `;
                    } else {
                        document.getElementById('sensorData').innerHTML = `
                            <p>Temperature: ${data.readTemperature.toFixed(1)}°C</p>
                            <p>Humidity: ${data.readHumidity.toFixed(1)}%</p>
                            <p>Heat Index: ${data.heat_index.toFixed(1)}°C</p>
                        `;
                    }
                })
                .catch(error => {
                    console.error('Error:', error);
                    document.getElementById('sensorData').innerHTML = `
                        <p class="error">Failed to fetch sensor data. Please try again later.</p>
                    `;
                });
        }

        // Update sensor data every 2 seconds
        setInterval(updateSensorData, 2000);
        // Initial update
        updateSensorData();
    </script>
</body>
</html>
)=====";