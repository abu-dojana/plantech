const char PAGE_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Soil Moisture Dashboard & Calibration</title>
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
    <h1>Soil Moisture Dashboard & Calibration</h1>
    <div id="sensorData">Loading sensor data...</div>
    <div id="calibrateStatus">Calibration Status: Idle</div>

    <button onclick="startCalibrate()">Start Calibrate</button>
    <button onclick="stopCalibrate()">Stop Calibrate</button>
    <button onclick="processCalibration()">Process Calibration</button>

    <h2>Set Values</h2>
    <label for="wetValueLow">Wet Value (Low): </label>
    <input type="number" id="wetValueLow" placeholder="Enter wet value">
    <button onclick="updateWetValueLow()">Set Wet Value</button>
    
    <br>

    <label for="dryValueHigh">Dry Value (High): </label>
    <input type="number" id="dryValueHigh" placeholder="Enter dry value">
    <button onclick="updateDryValueHigh()">Set Dry Value</button>

    <script>
        function updateSensorData() {
            fetch('/sensorData')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('sensorData').innerHTML = `
                        <p>Soil Moisture: ${data.soilMoisture}%</p>
                        <p>RAW value: ${data.soilMoistureRaw}</p>
                        <p>Pump Status: ${data.isPumpOn ? 'Active' : 'Inactive'}</p>
                        <p>Pump Cooldown: ${data.inCooldown ? 'True' : 'False'}</p>
                        <p>Pump Activation Count: ${data.pumpActivationCount}</p>
                        <p>Wet Value (Low): ${data.wetValueLow}</p>
                        <p>Dry Value (High): ${data.dryValueHigh}</p>
                        ${data.calibrationStatus ? `<p>Min: ${data.minValue}, Max: ${data.maxValue}</p>` : ''}
                    `;
                })
                .catch(error => console.error('Error:', error));
        }

        function startCalibrate() {
            fetch('/start_calibrate').then(response => response.text()).then(data => {
                document.getElementById('calibrateStatus').innerText = data;
            });
        }

        function stopCalibrate() {
            fetch('/stop_calibrate').then(response => response.text()).then(data => {
                document.getElementById('calibrateStatus').innerText = data;
            });
        }

        function processCalibration() {
            fetch('/process_calibration').then(response => response.text()).then(data => {
                document.getElementById('calibrateStatus').innerText = data;
            });
        }

        function updateWetValueLow() {
            const value = document.getElementById('wetValueLow').value;
            fetch('/setWetValueLow', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `value=${value}`
            })
            .then(response => response.text())
            .then(data => {
                alert(data);
            })
            .catch(error => console.error('Error:', error));
        }

        function updateDryValueHigh() {
            const value = document.getElementById('dryValueHigh').value;
            fetch('/setDryValueHigh', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `value=${value}`
            })
            .then(response => response.text())
            .then(data => {
                alert(data);
            })
            .catch(error => console.error('Error:', error));
        }

        setInterval(updateSensorData, 1000);  // Fetch sensor data every second
        updateSensorData();  // Initial update
    </script>

      <h2>Set Pump Duration</h2>
      <label for="pumpDuration">Pump Duration (seconds): </label>
      <input type="number" id="pumpDuration" placeholder="Enter pump duration in seconds">
      <button onclick="updatePumpDuration()">Set Pump Duration</button>
    

      <script>
          function updatePumpDuration() {
              const duration = document.getElementById('pumpDuration').value;
              fetch('/setPumpDuration', {
                  method: 'POST',
                  headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                  body: `duration=${duration}`
              })
              .then(response => response.text())
              .then(data => {
                  alert(data);
              })
              .catch(error => console.error('Error:', error));
          }
      </script>


       <h2>Set Pump Cooldown Duration</h2>
<label for="cooldownDuration">Cooldown Duration (seconds): </label>
<input type="number" id="cooldownDuration" placeholder="Enter cooldown duration in seconds">
<button onclick="updateCooldownDuration()">Set Cooldown Duration</button>

<script>
    function updateCooldownDuration() {
        const duration = document.getElementById('cooldownDuration').value;
        fetch('/setCooldownDuration', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: `duration=${duration}`
        })
        .then(response => response.text())
        .then(data => {
            alert(data);
        })
        .catch(error => console.error('Error:', error));
    }
</script>

<h2>Manual Pump Control</h2>
<button onclick="manualPumpOn()">Turn Pump ON</button>
<button onclick="manualPumpOff()">Turn Pump OFF</button>

<script>
    function manualPumpOn() {
        fetch('/manualPumpOn')
            .then(response => response.text())
            .then(data => {
                alert(data);  // Show the response in an alert box
            })
            .catch(error => console.error('Error:', error));
    }

    function manualPumpOff() {
        fetch('/manualPumpOff')
            .then(response => response.text())
            .then(data => {
                alert(data);  // Show the response in an alert box
            })
            .catch(error => console.error('Error:', error));
    }
</script>






</body>
</html>

)=====";
