/*
   If the sensor (water) reading is this then turn on the led.
*/
#include <WiFi.h>
#include <WebServer.h>

#include "index.h"

// Network credentials
#define WIFI_SSID "abudozana"
#define WIFI_PASS "Darda@55"

// Pin definitions

#define WATER_SENSOR_PIN 35

#define LED_PIN 2



// Create instance
WebServer server(80);

// Function prototypes
void connectToWifi();
void handleRoot();
void handleSensorData();

float readWaterLevel();


void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  
  // Connect to Wi-Fi
  connectToWifi();
  

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

  float waterLevel = readWaterLevel();

  
  String json = "{";
  json += "\"waterLevel\":" + String(waterLevel);
  json += "}";
  
  server.send(200, "application/json", json);
}


float readWaterLevel() {
  int waterSensorValueRAW = analogRead(WATER_SENSOR_PIN);
  float waterLevel = map(waterSensorValueRAW, 1500, 2200, 0, 100);
  float waterLevelMm = map(waterSensorValueRAW, 1500, 2200, 0, 400);
  
  if (waterLevelMm <= 100) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  
  return constrain(waterLevel, 0, 100);
}

