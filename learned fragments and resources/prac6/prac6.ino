/*
  Soil Moisture Sensor with Calibration, Cooldown, and Web Dashboard
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

// Variables for calibration, pump control, and cooldown
bool calibrateActive = false;
int rawValue = 0;
int wetValue_low = 0;
int dryValue_high = 4095;
int maxRawValue = 0;
int minRawValue = 4095;  // ADC max value for 12-bit resolution
unsigned long pumpActiveTime = 0;  // Time for how long the pump should stay on (in milliseconds)
unsigned long pumpStartTime = 0;   // Time when the pump was turned on
unsigned long cooldownStartTime = 0; // Time when the cooldown started
unsigned long cooldownDuration = 0;  // Cooldown period (in milliseconds)
bool isPumpOn = false;
int pumpActivationCount = 0;  // Count the number of activations within a window
bool inCooldown = false;  // Flag to indicate if cooldown is active

// Function prototypes
void connectToWifi();
void index_webpage();
void handleSensorData();
void startCalibrate();
void stopCalibrate();
void processCalibration();
void readSensorData();
void setPumpDuration();
void setCooldownDuration();

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
    pumpActiveTime = server.arg("duration").toInt() * 1000;  // Convert seconds to milliseconds
    server.send(200, "text/plain", "Pump duration updated");
  } else {
    server.send(400, "text/plain", "Missing duration argument");
  }
}

// Function to set the cooldown duration
void setCooldownDuration() {
  if (server.hasArg("duration")) {
    cooldownDuration = server.arg("duration").toInt() * 1000;  // Convert seconds to milliseconds
    server.send(200, "text/plain", "Cooldown duration updated");
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
  
  // New routes to update wetValue_low, dryValue_high, pump duration, and cooldown duration
  server.on("/setWetValueLow", HTTP_POST, setWetValueLow);
  server.on("/setDryValueHigh", HTTP_POST, setDryValueHigh);
  server.on("/setPumpDuration", HTTP_POST, setPumpDuration);
  server.on("/setCooldownDuration", HTTP_POST, setCooldownDuration);

   // New routes for manual pump control
  server.on("/manualPumpOn", HTTP_GET, manualPumpOn);   // Route to turn on pump manually
  server.on("/manualPumpOff", HTTP_GET, manualPumpOff); // Route to turn off pump manually


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
  // Check if cooldown period is active
  if (inCooldown && (millis() - cooldownStartTime >= cooldownDuration)) {
    // Cooldown period is over
    inCooldown = false;
    pumpActivationCount = 0;  // Reset activation count
  }

  // Check if pump needs to be turned off
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

  // Only activate the pump if it's not in cooldown
  if(soilMoisture < 34 && !isPumpOn && !inCooldown) {
    digitalWrite(PUMP_PIN, HIGH);
    digitalWrite(PUMP_PIN_LOW, LOW);
    digitalWrite(ledpin_pump, HIGH);
    pumpStartTime = millis();  // Record the time the pump was activated
    isPumpOn = true;
    
    // Increase the activation count
    pumpActivationCount++;

    // Check if the pump has been activated twice
    if (pumpActivationCount >= 2) {
      // Start the cooldown period
      inCooldown = true;
      cooldownStartTime = millis();
    }
  }

  // Create JSON response
  String json = "{";
  json += "\"soilMoisture\":" + String(soilMoisture) + ",";
  json += "\"soilMoistureRaw\":" + String(rawValue) + ",";
  json += "\"wetValueLow\":" + String(wetValue_low) + ",";
  json += "\"dryValueHigh\":" + String(dryValue_high) + ",";
  json += "\"isPumpOn\":" + String(isPumpOn) + ",";
  json += "\"pumpActivationCount\":" + String(pumpActivationCount) + ",";
  json += "\"inCooldown\":" + String(inCooldown ? "true" : "false");
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



// New function to manually turn the pump on
void manualPumpOn() {
  //if (!isPumpOn && !inCooldown) {
    digitalWrite(PUMP_PIN, HIGH);
    digitalWrite(PUMP_PIN_LOW, LOW);
    digitalWrite(ledpin_pump, HIGH);
    //pumpStartTime = millis();  // Start time for tracking manual pump activation
    //isPumpOn = true;
    server.send(200, "text/plain", "Pump turned on manually");
 // } else if (isPumpOn) {
 //   server.send(400, "text/plain", "Pump is already on");
 // } else if (inCooldown) {
 //   server.send(400, "text/plain", "Pump is in cooldown, cannot be turned on");
 // }
}

// New function to manually turn the pump off
void manualPumpOff() {
  //if (isPumpOn) {
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(PUMP_PIN_LOW, LOW);
    digitalWrite(ledpin_pump, LOW);
    //isPumpOn = false;
    server.send(200, "text/plain", "Pump turned off manually");
 // } else {
 //   server.send(400, "text/plain", "Pump is already off");
// }
}
