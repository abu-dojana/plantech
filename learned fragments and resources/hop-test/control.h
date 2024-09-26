#include "esp32-hal-gpio.h"
#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include <DHT.h>

#define PUMP_PIN 26

extern WebServer server;
extern int wetValue_low;
extern int dryValue_high;
extern int soilMoistureThreshold;
extern unsigned long pumpDuration;
extern int pumpActivationCount;
extern Servo myservo;

String getControlHTML() {
  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>CONTROL-Plantech</title>
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
</head>
<body>
    <div class="navbar-nav">
        <a class="nav-link" href="/">Home</a>
        <a class="nav-link" href="/connectivity">Connectivity</a>
        <a class="nav-link active" href="/control">Control</a>
    </div>
    
    <h1>Sensor Calibration and Control</h1>
    
    <div id="soilMoistureCalibration">
        <h2>Soil Moisture Sensor Calibration</h2>
        <button onclick='startCalibrate()'>Start Calibrate</button>
        <button onclick='stopCalibrate()'>Stop Calibrate</button>
        <button onclick='processCalibration()'>Process Calibration</button>
        <div id="calibrationStatus"></div>
        
        <h3>Set Values Manually</h3>
        <input type="number" id="wetValueLow" placeholder='Wet Value (Low)'>
        <button onclick='setWetValueLow()'>Set Wet Value</button>
        <input type="number" id="dryValueHigh" placeholder='Dry Value (High)'>
        <button onclick='setDryValueHigh()'>Set Dry Value</button>
    </div>
    
    <div id="waterLevelCalibration">
        <h2>Water Level Sensor Calibration</h2>
        <!-- Add water level calibration controls here if needed -->
    </div>
    
    <div id="pumpControl">
        <h2>Pump Control</h2>
        <button onclick='activatePump()'>Activate Pump</button>
        <input type="number" id="pumpDuration" placeholder='Pump Duration (ms)'>
        <button onclick='setPumpDuration()'>Set Pump Duration</button>
        <input type="number" id="soilMoistureThreshold" placeholder='Soil Moisture Threshold (%)'>
        <button onclick='setSoilMoistureThreshold()'>Set Threshold</button>
        <button onclick='resetPumpActivationCount()'>Reset Pump Activation Count</button>
    </div>
    
    <div id="servoControl">
        <h2>Servo Motor Control</h2>
        <input type="range" id="servoAngle" min="0" max="360" value="0">
        <button onclick='setServoAngle()'>Set Servo Angle</button>
    </div>

    <script>
        let calibrateActive = false;
        
        function startCalibrate() {
            calibrateActive = true;
            updateCalibrationStatus("Calibration Started");
            fetch('/startCalibrate').then(response => response.text()).then(data => console.log(data));
        }
        
        function stopCalibrate() {
            calibrateActive = false;
            updateCalibrationStatus("Calibration Stopped");
            fetch('/stopCalibrate').then(response => response.text()).then(data => console.log(data));
        }
        
        function processCalibration() {
            fetch('/processCalibration').then(response => response.text()).then(data => {
                updateCalibrationStatus("Calibration Processed: " + data);
            });
        }
        
        function updateCalibrationStatus(status) {
            document.getElementById('calibrationStatus').innerText = status;
        }
        
        function setWetValueLow() {
            const value = document.getElementById('wetValueLow').value;
            fetch('/setWetValueLow', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `value=${value}`
            }).then(response => response.text()).then(data => alert(data));
        }
        
        function setDryValueHigh() {
            const value = document.getElementById('dryValueHigh').value;
            fetch('/setDryValueHigh', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `value=${value}`
            }).then(response => response.text()).then(data => alert(data));
        }
        
        function activatePump() {
            fetch('/activatePump').then(response => response.text()).then(data => alert(data));
        }
        
        function setPumpDuration() {
            const duration = document.getElementById('pumpDuration').value;
            fetch('/setPumpDuration', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `duration=${duration}`
            }).then(response => response.text()).then(data => alert(data));
        }
        
        function setSoilMoistureThreshold() {
            const threshold = document.getElementById('soilMoistureThreshold').value;
            fetch('/setSoilMoistureThreshold', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `threshold=${threshold}`
            }).then(response => response.text()).then(data => alert(data));
        }
        
        function resetPumpActivationCount() {
            fetch('/resetPumpActivationCount').then(response => response.text()).then(data => alert(data));
        }
        
        function setServoAngle() {
            const angle = document.getElementById('servoAngle').value;
            fetch('/setServoAngle', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `angle=${angle}`
            }).then(response => response.text()).then(data => alert(data));
        }
    </script>
</body>
</html>
  )";
  return html;
}



void handleControl() {
    server.send(200, "text/html", getControlHTML());
}

void handleSetWetValueLow() {
    if (server.hasArg("value")) {
        wetValue_low = server.arg("value").toInt();
        server.send(200, "text/plain", "Wet Value (Low) updated");
    } else {
        server.send(400, "text/plain", "Missing value argument");
    }
}

void handleSetDryValueHigh() {
    if (server.hasArg("value")) {
        dryValue_high = server.arg("value").toInt();
        server.send(200, "text/plain", "Dry Value (High) updated");
    } else {
        server.send(400, "text/plain", "Missing value argument");
    }
}

void handleActivatePump() {
    // Implement pump activation logic here
    // This should trigger the pump for the set duration
    //activatePump();
    digitalWrite(PUMP_PIN, HIGH);
    server.send(200, "text/plain", "Pump activated manually");
}



void handleSetPumpDuration() {
    if (server.hasArg("duration")) {
        pumpDuration = server.arg("duration").toInt();
        server.send(200, "text/plain", "Pump duration updated");
    } else {
        server.send(400, "text/plain", "Missing duration argument");
    }
}

void handleSetSoilMoistureThreshold() {
    if (server.hasArg("threshold")) {
        soilMoistureThreshold = server.arg("threshold").toInt();
        server.send(200, "text/plain", "Soil moisture threshold updated");
    } else {
        server.send(400, "text/plain", "Missing threshold argument");
    }
}

void handleResetPumpActivationCount() {
    pumpActivationCount = 0;
    server.send(200, "text/plain", "Pump activation count reset");
}

void handleSetServoAngle() {
    if (server.hasArg("angle")) {
        int angle = server.arg("angle").toInt();
        myservo.write(angle);
        server.send(200, "text/plain", "Servo angle set to " + String(angle));
    } else {
        server.send(400, "text/plain", "Missing angle argument");
    }
}

// Variables for calibration
bool calibrateActive = false;
int maxRawValue = 0;
int minRawValue = 4095;

void handleStartCalibrate() {
    calibrateActive = true;
    maxRawValue = 0;
    minRawValue = 4095;
    server.send(200, "text/plain", "Calibration Started");
}

void handleStopCalibrate() {
    calibrateActive = false;
    server.send(200, "text/plain", "Calibration Stopped");
}

void handleProcessCalibration() {
    wetValue_low = minRawValue;
    dryValue_high = maxRawValue;
    String result = "Calibration Processed. Wet: " + String(wetValue_low) + ", Dry: " + String(dryValue_high);
    server.send(200, "text/plain", result);
}

// This function should be called in the main loop when calibrateActive is true
void calibrateSoilMoisture() {
    if (calibrateActive) {
       // int rawValue = analogRead(SOIL_MOISTURE_PIN);
      //  if (rawValue > maxRawValue) maxRawValue = rawValue;
      //  if (rawValue < minRawValue) minRawValue = rawValue;
    }
}

#endif // CONTROL_H