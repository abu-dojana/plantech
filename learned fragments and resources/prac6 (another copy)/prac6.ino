/*
  Soil Moisture Sensor with Calibration and Web Dashboard
*/

#include <WiFi.h>
#include <WebServer.h>
#include "index.h"

// Network credentials
#define WIFI_SSID "abudozana"
#define WIFI_PASS "Darda@55"

// Pin definitions
#define SOIL_MOISTURE_PIN 34  // Analog pin for soil moisture sensor
#define PUMP_PIN 18
#define PUMP_PIN_LOW 19
#define ledpin_pump 4

// Create instances
WebServer server(80);

// Variables for calibration and pump control
bool calibrateActive = false;
int rawValue = 0;
int wetValue_low = 0;
int dryValue_high = 4095;
int maxRawValue = 0;
int minRawValue = 4095; // ADC max value for 12-bit resolution
unsigned long pumpActiveTime = 0;  // Time for how long the pump should stay on
unsigned long pumpStartTime = 0;   // Time when the pump was turned on
bool isPumpOn = false;

// Function prototypes
void connectToWifi();
void index_webpage();
void handleSensorData();
void startCalibrate();
void stopCalibrate();
void processCalibration();
void readSensorData();
void setPumpDuration();

// Function to update wetValue_low
void setWetValueLow() {
  if (server.hasArg("value")) {
    wetValue_low = server.arg("value").toInt();
    server.send(200, "text/plain", "wetValue_low updated");
  } else {
    server.send(400, "text/plain", "Missing value argument");
  }
}

// Function to update dryValue_high
void setDryValueHigh() {
  if (server.hasArg("value")) {
    dryValue_high = server.arg("value").toInt();
    server.send(200, "text/plain", "dryValue_high updated");
  } else {
    server.send(400, "text/plain", "Missing value argument");
  }
}

// Function to set the pump duration
void setPumpDuration() {
  if (server.hasArg("duration")) {
    pumpActiveTime = server.arg("duration").toInt();  // Convert seconds to milliseconds
    server.send(200, "text/plain", "Pump duration updated");
  } else {
    server.send(400, "text/plain", "Missing duration argument");
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  connectToWifi();

  // Set up server routes
  server.on("/", index_webpage);
  server.on("/sensorData", HTTP_GET, handleSensorData);
  server.on("/start_calibrate", HTTP_GET, startCalibrate);
  server.on("/stop_calibrate", HTTP_GET, stopCalibrate);
  server.on("/process_calibration", HTTP_GET, processCalibration);
  
  // New routes to update wetValue_low, dryValue_high, and pump duration
  server.on("/setWetValueLow", HTTP_POST, setWetValueLow);
  server.on("/setDryValueHigh", HTTP_POST, setDryValueHigh);
  server.on("/setPumpDuration", HTTP_POST, setPumpDuration);

  // Start server
  server.begin();
  Serial.println("HTTP server started!");

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(PUMP_PIN_LOW, OUTPUT);
  pinMode(ledpin_pump, OUTPUT);
}

void loop() {
  server.handleClient();
  if (calibrateActive) {
    readSensorData();  // Continuously read sensor data if calibration is active
  }
  managePump();  // Handle the pump state based on the timer
}

void managePump() {
  if (isPumpOn && (millis() - pumpStartTime >= pumpActiveTime)) {
    // Time to turn the pump off
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(PUMP_PIN_LOW, LOW);
    digitalWrite(ledpin_pump, LOW);
    isPumpOn = false;
  }
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
  readSensorData();

  float soilMoistureRaw = map(rawValue, wetValue_low, dryValue_high, 100, 0);
  float soilMoisture = constrain(soilMoistureRaw, 0, 100);

  if(soilMoisture < 34 && !isPumpOn) {
    //digitalWrite(PUMP_PIN, HIGH);
    //digitalWrite(PUMP_PIN_LOW, LOW);
    //digitalWrite(ledpin_pump, HIGH);
    pumpStartTime = millis();  // Record the time the pump was activated
    isPumpOn = true;
  }

  // Create JSON response
  String json = "{";
  json += "\"soilMoisture\":" + String(soilMoisture) + ",";
  json += "\"soilMoistureRaw\":" + String(rawValue) + ",";
  json += "\"wetValueLow\":" + String(wetValue_low) + ",";
  json += "\"dryValueHigh\":" + String(dryValue_high);
  if (calibrateActive) {
    json += ",\"calibrationStatus\":\"Active\"";
    json += ",\"minValue\":" + String(minRawValue);
    json += ",\"maxValue\":" + String(maxRawValue);
  }
  json += "}";

  server.send(200, "application/json", json);
}

void startCalibrate() {
  calibrateActive = true;
  maxRawValue = 0;
  minRawValue = 4095;
  server.send(200, "text/plain", "Calibration Started");
}

void stopCalibrate() {
  calibrateActive = false;
  server.send(200, "text/plain", "Calibration Stopped");
}

void processCalibration() {
  wetValue_low = minRawValue;
  dryValue_high = maxRawValue;
  server.send(200, "text/plain", "Calibration Processed");
}

void readSensorData() {
  rawValue = analogRead(SOIL_MOISTURE_PIN);  // Read raw sensor value

  // Update min and max values during calibration
  if (calibrateActive) {
    if (rawValue > maxRawValue) {
      maxRawValue = rawValue;
    }
    if (rawValue < minRawValue) {
      minRawValue = rawValue;
    }
  }
}
