#ifndef INDEX_H
#define INDEX_H

const char PAGE_INDEX[] PROGMEM = R"=====(
    <!DOCTYPE html>
<html lang="en">
<head>
    <title>HOME-Plantech</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f0f0f0; }
        .container { max-width: 800px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        h1, h2 { color: #333; }
        .info-box { background-color: #e9f7ef; border: 1px solid #27ae60; border-radius: 5px; padding: 15px; margin-bottom: 20px; }
        .info-box p { margin: 5px 0; }
        #networks { list-style-type: none; padding: 0; }
        #networks li { background-color: #f0f0f0; margin: 10px 0; padding: 10px; border-radius: 5px; cursor: pointer; transition: background-color 0.3s; }
        #networks li:hover { background-color: #e0e0e0; }
        .connect-form { background-color: #fff; padding: 20px; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); margin-bottom: 20px; }
        input[type=text], input[type=password] { width: 100%; padding: 10px; margin: 10px 0; box-sizing: border-box; border: 1px solid #ddd; border-radius: 4px; }
        input[type=submit] { background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; cursor: pointer; width: 100%; border-radius: 4px; transition: background-color 0.3s; }
        input[type=submit]:hover { background-color: #45a049; }
        #status { margin-top: 20px; font-weight: bold; padding: 10px; border-radius: 4px; }
        .success { background-color: #d4edda; color: #155724; }
        .error { background-color: #f8d7da; color: #721c24; }

        /* Navbar container */
        .navbar-nav {
          display: flex;
          justify-content: space-around; /* Spaces out links evenly */
          background-color: #333; /* Dark background for the navbar */
          padding: 10px; /* Adds some padding around the links */
        }

        /* Navbar links */
        .nav-link {
          color: white; /* White text color */
          text-decoration: none; /* Removes the underline from the links */
          padding: 10px 20px; /* Adds padding around the links */
          font-size: 18px; /* Adjusts the font size */
          border-radius: 5px; /* Rounds the corners of each link */
        }

        /* Active link */
        .nav-link.active {
          background-color: #555; /* Highlights the active link */
        }

        /* Hover effect */
        .nav-link:hover {
          background-color: #777; /* Changes background color on hover */
          color: #fff; /* Keeps the text color white */
        }


    </style>

      <!-- Navbar -->
        <div class="navbar-nav">
          <a class="nav-link active" aria-current="page" href="/">Home</a>
          <a class="nav-link" href="/connectivity">Connectivity</a>
          <a class="nav-link" href="/control">Control</a>
        </div>

</head>
<body>
    <h1>DHT11 Sensor Dashboard</h1>
    <div id="DHT11sensorData">Loading sensor data...</div>

    <script>
        function updateDHT11SensorData() {
            fetch('/DHT11sensorData')
                .then(response => response.json())
                .then(data => {
                    if (data.error) {
                        document.getElementById('DHT11sensorData').innerHTML = `
                            <p class="error">${data.error}</p>
                        `;
                    } else {
                        document.getElementById('DHT11sensorData').innerHTML = `
                            <p>Temperature: ${data.readTemperature.toFixed(1)}°C</p>
                            <p>Humidity: ${data.readHumidity.toFixed(1)}%</p>
                            <p>Heat Index: ${data.heat_index.toFixed(1)}°C</p>
                            <p><a href="https://www.weather.gov/ama/heatindex">Learn about Heat Index</a></p>
                        `;
                    }
                })
                .catch(error => {
                    console.error('Error:', error);
                    document.getElementById('DHT11sensorData').innerHTML = `
                        <p class="error">Failed to fetch DHT sensor data. Please Refresh to try again.</p>
                    `;
                });
        }

        // Update sensor data every 2 seconds
        setInterval(updateDHT11SensorData, 2000);
        // Initial update
        updateDHT11SensorData();
    </script>


      <!--water-level-->
          <h1>Resourver Water Level Info</h1>
      <div id="WatersensorData">Loading sensor data...</div>

      <script>
          function updateWaterSensorData() {
              fetch('/WatersensorData')
                  .then(response => response.json())
                  .then(data => {
                      document.getElementById('WatersensorData').innerHTML = `
                          <p>Water Level RAW value: ${data.waterSensorValueRAW}</p>
                          <p>Water level Percentage: ${data.water_level_percentage}%</p>
                          <p>Water level mm: ${data.water_level_mm} mm</p>
                      `;
                  })
                  .catch(error => console.error('Error:', error));
          }

          // Update sensor data every 1 seconds
          setInterval(updateWaterSensorData, 1000);
          // Initial update
          updateWaterSensorData();
      </script>


</body>
</html>


)=====";

#endif  // INDEX_H