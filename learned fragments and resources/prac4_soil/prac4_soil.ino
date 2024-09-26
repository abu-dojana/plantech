/*
  4. Show the reading of sensor on the webpage
    - create and connect server.
    - soil sensor (in percent)
*/
#include <WiFi.h>
#include <WebServer.h>
#include "index.h" 

// Network credentials
#define WIFI_SSID "abudozana"
#define WIFI_PASS "Darda@55"

// Pin definitions
#define SOIL_MOISTURE_PIN 34  // Analog pin for soil moisture sensor

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
  // Read soil moisture (assuming it's an analog sensor)
  int wetValue_low = 0;
  int dryValue_high = 4095;
  int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  float soilMoisture = map(soilMoistureRaw, wetValue_low, dryValue_high, 100, 0);
  soilMoisture = constrain(soilMoisture, 0, 100);

  // Create JSON response
  String json = "{";
  json += "\"soilMoisture\":" + String(soilMoisture) + ",";
  json += "\"soilMoistureRaw\":" + String(soilMoistureRaw);
  json += "}";

  server.send(200, "application/json", json);
}