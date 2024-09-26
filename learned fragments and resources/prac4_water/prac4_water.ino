/*
  4. Show the reading of sensor on the webpage
    - create and connect server.
    - water sensor (in percent)
*/
#include <WiFi.h>
#include <WebServer.h>
#include "index.h" 

// Network credentials
#define WIFI_SSID "abudozana"
#define WIFI_PASS "Darda@55"

// Pin definitions
#define water_sensor_pin 34  // Analog pin for soil moisture sensor
#define ledpin 2

// Create instances
WebServer server(80);

// Function prototypes
void connectToWifi();
void index_webpage();
void handleSensorData();

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  connectToWifi();

  // Set up server routes
  server.on("/", index_webpage);
  server.on("/sensorData", HTTP_GET, handleSensorData);

  // Start server
  server.begin();
  Serial.println("HTTP server started!");

  pinMode(ledpin, OUTPUT);
}

void loop() {
  server.handleClient();
}

void connectToWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void index_webpage() {
  server.send(200, "text/html", PAGE_INDEX);
}

void handleSensorData() {

   // Read the raw analog value from the sensor
  int waterSensorValueRAW = analogRead(water_sensor_pin);

  // Convert the raw value to percentage (optional - requires calibration)
  // Assuming 3800 (dry) to 2000 (wet) for demonstration purposes
  /*
    0    to 1300 no water.
    2200 to 4095 overflow (full).
    1900 to 2200 => 0% to 100%
  */

  float water_level_percentage = map(waterSensorValueRAW, 1500, 2200, 0, 100);
  water_level_percentage = constrain(water_level_percentage, 0, 100);
   
  // sensor length is 400mm
  float water_level_mm = map(waterSensorValueRAW, 1500, 2200, 0, 400);
  water_level_mm = constrain(water_level_mm, 0, 400);

  if (water_level_mm <= 100 ){
    digitalWrite(ledpin, HIGH);
  }else {
    digitalWrite(ledpin, LOW);
  }

  // Create JSON response
  String json = "{";
  json += "\"waterSensorValueRAW\":" + String(waterSensorValueRAW) + ",";
  json += "\"water_level_percentage\":" + String(water_level_percentage) + ",";
  json += "\"water_level_mm\":" + String(water_level_mm);
  json += "}";

  server.send(200, "application/json", json);
}