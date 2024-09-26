/*
   Show the reading of sensor on the webpage
	  - soil sensor (in percent)
	  - Water level
	  - DHT11
*/
#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"
#include "index.h"

// Network credentials
#define WIFI_SSID "abudozana"
#define WIFI_PASS "Darda@55"

// Pin definitions
#define SOIL_MOISTURE_PIN 34
#define WATER_SENSOR_PIN 35
#define DHT_SENSOR_PIN 27


// DHT setup
#define DHTTYPE DHT11
DHT dht(DHT_SENSOR_PIN, DHTTYPE);

// Create instance
WebServer server(80);

// Function prototypes
void connectToWifi();
void handleRoot();
void handleSensorData();
float readSoilMoisture();
float readWaterLevel();
void readDHT(float &temperature, float &humidity, float &heatIndex);

void setup() {
  Serial.begin(115200);
  

  
  // Connect to Wi-Fi
  connectToWifi();
  
  // Initialize DHT sensor
  dht.begin();
  
  // Set up server routes
  server.on("/", handleRoot);
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

void handleRoot() {
  server.send(200, "text/html", PAGE_INDEX);
}

void handleSensorData() {
  float soilMoisture = readSoilMoisture();
  float waterLevel = readWaterLevel();
  float temperature, humidity, heatIndex;
  readDHT(temperature, humidity, heatIndex);
  
  String json = "{";
  json += "\"soilMoisture\":" + String(soilMoisture) + ",";
  json += "\"waterLevel\":" + String(waterLevel) + ",";
  json += "\"temperature\":" + String(temperature) + ",";
  json += "\"humidity\":" + String(humidity) + ",";
  json += "\"heatIndex\":" + String(heatIndex);
  json += "}";
  
  server.send(200, "application/json", json);
}

float readSoilMoisture() {
  int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  float soilMoisture = map(soilMoistureRaw, 2000, 3800, 100, 0);
  return constrain(soilMoisture, 0, 100);
}

float readWaterLevel() {
  int waterSensorValueRAW = analogRead(WATER_SENSOR_PIN);
  float waterLevel = map(waterSensorValueRAW, 1500, 2200, 0, 100);
  float waterLevelMm = map(waterSensorValueRAW, 1500, 2200, 0, 400);
  
  return constrain(waterLevel, 0, 100);
}

void readDHT(float &temperature, float &humidity, float &heatIndex) {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  if (isnan(humidity) || isnan(temperature)) {
    temperature = humidity = heatIndex = -1;
  } else {
    heatIndex = dht.computeHeatIndex(temperature, humidity, false);
  }
}