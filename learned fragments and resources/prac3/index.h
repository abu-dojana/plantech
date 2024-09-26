const String PAGE_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Abu Dojana's Web Server</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
        h1 {
            color: #333;
            text-align: center;
        }
        button {
            margin: 10px;
            padding: 10px 20px;
            font-size: 16px;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <h1>Hello I am abu dojana's web server. I am mishkat</h1>

    <br>

    <a href="/HOME"><button> Home </button> </a>

     <button id="ledButton" onclick="toggleLED()">Toggle LED</button>

    <script>
        function toggleLED() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("ledButton").innerHTML = this.responseText;
                }
            };
            xhttp.open("GET", "/toggleLED", true);
            xhttp.send();
        }
    </script>
    
</body>
</html>
)=====";