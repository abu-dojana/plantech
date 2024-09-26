#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <Arduino.h>
#include <WiFi.h>

String getConnectivityHTML() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>CONNECTIVITY-Plantech</title>
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
    <div class="container">
        <h1>ESP32 WiFi Manager</h1>
        
        <div id="info-box" class="info-box">
            <h2>System Information</h2>
            <!-- This will be populated by JavaScript -->
        </div>

        <div id="status"></div>
        
        <div class="connect-form">
            <h2>Manual Connection</h2>
            <form id="manual-connect-form">
                <input type="text" id="manual-ssid" name="manual-ssid" placeholder="SSID" required>
                <input type="password" id="manual-password" name="manual-password" placeholder="Password" required>
                <input type="submit" value="Connect Manually">
            </form>
        </div>

        <h2>Available Networks</h2>
        <ul id="networks"></ul>
        <form id="connect-form" class="connect-form" style="display: none;">
            <input type="hidden" id="ssid" name="ssid">
            <input type="password" id="password" name="password" placeholder="Password" required>
            <input type="submit" value="Connect">
        </form>
    </div>

    <script>
        function updateInfo() {
            fetch('/info')
                .then(response => response.json())
                .then(info => {
                    const infoBox = document.getElementById('info-box');
                    infoBox.innerHTML = `
                        <h2>System Information</h2>
                        <p><strong>Date & Time:</strong> ${info.datetime}</p>
                        <p><strong>AP SSID:</strong> ${info.ap_ssid}</p>
                        <p><strong>AP Password:</strong> ${info.ap_password}</p>
                        <p><strong>AP IP:</strong> ${info.ap_ip}</p>
                        <p><strong>AP MAC:</strong> ${info.ap_mac}</p>
                        <p><strong>Connected to:</strong> ${info.sta_ssid}</p>
                        <p><strong>Station IP:</strong> ${info.sta_ip}</p>
                        <p><strong>Station MAC:</strong> ${info.sta_mac}</p>
                        <p><strong>Gateway:</strong> ${info.gateway}</p>
                        <p><strong>DNS:</strong> ${info.dns}</p>
                    `;
                });
        }

        function scanNetworks() {
            fetch('/scan')
                .then(response => response.json())
                .then(networks => {
                    const networkList = document.getElementById('networks');
                    networkList.innerHTML = '';
                    networks.forEach(network => {
                        const li = document.createElement('li');
                        li.textContent = `${network.ssid} (${network.rssi} dBm)`;
                        li.onclick = () => selectNetwork(network.ssid);
                        networkList.appendChild(li);
                    });
                });
        }

        function selectNetwork(ssid) {
            document.getElementById('ssid').value = ssid;
            document.getElementById('connect-form').style.display = 'block';
        }

        function connect(url, formData) {
            fetch(url, { method: 'POST', body: formData })
                .then(response => response.text())
                .then(result => {
                    const status = document.getElementById('status');
                    status.textContent = result;
                    status.className = result.includes('Failed') ? 'error' : 'success';
                    updateInfo();
                });
        }

        document.getElementById('connect-form').onsubmit = function(e) {
            e.preventDefault();
            connect('/connect', new FormData(this));
        };

        document.getElementById('manual-connect-form').onsubmit = function(e) {
            e.preventDefault();
            connect('/manual-connect', new FormData(this));
        };

        updateInfo();
        scanNetworks();
        setInterval(updateInfo, 5000);
        setInterval(scanNetworks, 10000);
    </script>
</body>
</html>
  )";
  return html;
}

String getScanResultsJSON() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
  }
  json += "]";
  return json;
}

bool connectToWiFi(const String& ssid, const String& password) {
  WiFi.begin(ssid.c_str(), password.c_str());

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  return WiFi.status() == WL_CONNECTED;
}

#endif  // CONNECTIVITY_H
