

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }
        h1 { color: #333; }
        .info { background-color: #f0f0f0; padding: 10px; border-radius: 5px; margin-bottom: 20px; }
        form { background-color: #fff; padding: 20px; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
        input[type=text], input[type=password] { width: 100%; padding: 10px; margin: 10px 0; box-sizing: border-box; }
        input[type=submit] { background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; cursor: pointer; width: 100%; }
        input[type=submit]:hover { opacity: 0.8; }
    </style>
</head>
<body>
    <h1>ESP32 Configuration</h1>
    <div class="info">
        <p><strong>Access Point SSID:</strong> {{AP_SSID}}</p>
        <p><strong>Access Point IP:</strong> {{AP_IP}}</p>
        <p><strong>Connected to:</strong> {{STA_SSID}}</p>
        <p><strong>Station IP:</strong> {{STA_IP}}</p>
    </div>
    <form action="/connect" method="post">
        <h2>Connect to WiFi</h2>
        <input type="text" name="ssid" placeholder="SSID" required>
        <input type="password" name="password" placeholder="Password" required>
        <input type="submit" value="Connect">
    </form>
</body>
</html>
)=====";
