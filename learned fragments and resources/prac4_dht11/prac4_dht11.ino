#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"
#include "index.h" 

// Network credentials
#define WIFI_SSID "abudozana"
#define WIFI_PASS "Darda@55"

// Pin definitions
#define dht_sensor_pin 27

//Setup DHT
#define DHTTYPE DHT11 
DHT dht(dht_sensor_pin, DHTTYPE);

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

  // Initialize DHT sensor
  dht.begin();

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
  float readTemperature = dht.readTemperature();
  float readHumidity = dht.readHumidity();

  String json = "{";

  if (isnan(readHumidity) || isnan(readTemperature)) {
    json += "\"error\": \"Failed to read from DHT sensor!\"";
  } else {
    float heat_index = dht.computeHeatIndex(readTemperature, readHumidity, false);
    
    json += "\"readTemperature\":" + String(readTemperature) + ",";
    json += "\"readHumidity\":" + String(readHumidity) + ",";
    json += "\"heat_index\":" + String(heat_index);
  }

  json += "}";

  server.send(200, "application/json", json);
}