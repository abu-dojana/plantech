//include section
#include <WiFi.h>          //wifi include
#include <WebServer.h>     //server include
#include <DNSServer.h>     //DNS include
#include <time.h>          //time for cennectivity.h
#include "connectivity.h"  // Include the header file with WiFi functions and HTML content
#include "index.h"
#include "control.h"
#include "DHT.h"

//wifi variables and objects- start
const char* ap_ssid = "PlanTech";
const char* ap_password = "admin123";

IPAddress apIP(192, 168, 10, 10);
DNSServer dnsServer;
WebServer server(80);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 21600;  // Bangladesh GMT +6
const int daylightOffset_sec = 3600;
//wifi variables and objects- end

//milis() variable
unsigned long previousMillis = 0;
const long interval_water_loop = 3000;

// Pin definitions
#define dht_sensor_pin 27

//Setup DHT
#define DHTTYPE DHT11
DHT dht(dht_sensor_pin, DHTTYPE);


// Pin definitions -water sensor
#define water_sensor_pin 35  // Analog pin for soil moisture sensor
#define ledpin_water 2

// Variables for water level sensor calibration
bool waterCalibrationActive = false;
int waterMinRawValue = 4095;
int waterMaxRawValue = 0;
int waterLevelLowEmpty = 1500;  // Default value, will be updated during calibration
int waterLevelHighFull = 2200;  // Default value, will be updated during calibration



// Pin definitions -soil sensor
#define SOIL_MOISTURE_PIN 34  // Analog pin for soil moisture sensor
#define PUMP_PIN 25
#define PUMP_PIN_LOW 26
#define ledpin_pump 4

// Variables for calibration, pump control, and cooldown
bool calibrateActive = false;
int rawValue = 0;
int wetValue_low = 2200;
int dryValue_high = 3800;
int maxRawValue = 0;
int minRawValue = 4095;               // ADC max value for 12-bit resolution
unsigned long pumpActiveTime = 0;     // Time for how long the pump should stay on (in milliseconds)
unsigned long pumpStartTime = 0;      // Time when the pump was turned on
unsigned long cooldownStartTime = 0;  // Time when the cooldown started
unsigned long cooldownDuration = 0;   // Cooldown period (in milliseconds)
bool isPumpOn = false;
int pumpActivationCount = 0;  // Count the number of activations within a window
bool inCooldown = false;      // Flag to indicate if cooldown is active



void setup() {
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  //wifi- start
  setupAccessPoint();                                        //wifi AP
  setupWebServerConnectivity();                              //connectivity.h server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  //time config for connectivity.h

  Serial.println("Access Point and Web Server started");

  setupWebServerIndex();    //index.h server
  setupWebServerControl();  //control.h server
  //wifi- end

  pinMode(ledpin_water, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  //pinMode(PUMP_PIN_LOW, OUTPUT);
  pinMode(ledpin_pump, OUTPUT);
}

void loop() {
  dnsServer.processNextRequest();  //dns wifi
  server.handleClient();           //request handler

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval_water_loop) {
    previousMillis = currentMillis;
    handleWaterSensorData();  //ensures water reading is running in background
  }

  if (calibrateActive) {
    readSensorData();  // Continuously read sensor data if calibration is active
  }

  managePump();  // Handle the pump state based on the timer
}

//================sensors data functions===============//

//soil sensor
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

  // Automatic pump control based on sensor moisture level
  float soilMoisture = map(rawValue, wetValue_low, dryValue_high, 100, 0);
  soilMoisture = constrain(soilMoisture, 0, 100);

  // If soil is too dry and no cooldown or pump running, activate pump
  if (soilMoisture < 34 && !isPumpOn && !inCooldown) {
    digitalWrite(PUMP_PIN, HIGH);
    digitalWrite(PUMP_PIN_LOW, LOW);
    digitalWrite(ledpin_pump, HIGH);
    pumpStartTime = millis();  // Record pump start time
    isPumpOn = true;
    pumpActivationCount++;

    // If pump has activated twice, initiate cooldown
    if (pumpActivationCount >= 2) {
      inCooldown = true;
      cooldownStartTime = millis();
    }
  }
}

//soil sensor handle
void handleSensorData() {
  readSensorData();

  float soilMoistureRaw = map(rawValue, wetValue_low, dryValue_high, 100, 0);
  float soilMoisture = constrain(soilMoistureRaw, 0, 100);

  // Only activate the pump if it's not in cooldown
  if (soilMoisture < 34 && !isPumpOn && !inCooldown) {
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

  server.send(200, "application/json", json;
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


//dht
void handleDHT11SensorData() {
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


//water level sensor
unsigned long previousMillis_water = 0;
void handleWaterSensorData() {

  // Convert the raw value to percentage (optional - requires calibration)
  // Assuming 3800 (dry) to 2000 (wet) for demonstration purposes
  /*
    0    to 1500 no water.
    2200 to 4095 overflow (full).
    1900 to 2200 => 0% to 100%
  */

  // int water_level_low_max_empty = 1500;
  // int water_level_high_min_full = 2200;

  // Read the raw analog value from the sensor
  int waterSensorValueRAW = analogRead(water_sensor_pin);

  if (waterCalibrationActive) {
    if (waterSensorValueRAW < waterMinRawValue) waterMinRawValue = waterSensorValueRAW;
    if (waterSensorValueRAW > waterMaxRawValue) waterMaxRawValue = waterSensorValueRAW;
  }

  // float water_level_percentage = map(waterSensorValueRAW, water_level_low_max_empty, water_level_high_min_full, 0, 100);
  // water_level_percentage = constrain(water_level_percentage, 0, 100);

  // // sensor length is 400mm
  // float water_level_mm = map(waterSensorValueRAW, water_level_low_max_empty, water_level_high_min_full, 0, 400);
  // water_level_mm = constrain(water_level_mm, 0, 400);

  float water_level_percentage = map(waterSensorValueRAW, waterLevelLowEmpty, waterLevelHighFull, 0, 100);
  water_level_percentage = constrain(water_level_percentage, 0, 100);

  float water_level_mm = map(waterSensorValueRAW, waterLevelLowEmpty, waterLevelHighFull, 0, 400);
  water_level_mm = constrain(water_level_mm, 0, 400);

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis_water >= interval_water_loop) {
    if (water_level_mm <= 100) {
      digitalWrite(ledpin_water, HIGH);
    } else {
      digitalWrite(ledpin_water, LOW);
    }
  }


  // Create JSON response
  String json = "{";
  json += "\"waterSensorValueRAW\":" + String(waterSensorValueRAW) + ",";
  json += "\"water_level_percentage\":" + String(water_level_percentage) + ",";
  json += "\"water_level_mm\":" + String(water_level_mm) + ",";
  json += "\"calibrationActive\":" + String(waterCalibrationActive ? "true" : "false") + ",";
  json += "\"minValue\":" + String(waterMinRawValue) + ",";
  json += "\"maxValue\":" + String(waterMaxRawValue);
  json += "}";
  server.send(200, "application/json", json);
}

//  water level handler functions
void startWaterCalibrate() {
  waterCalibrationActive = true;
  waterMinRawValue = 4095;
  waterMaxRawValue = 0;
  server.send(200, "text/plain", "Water Level Calibration Started");
}

void stopWaterCalibrate() {
  waterCalibrationActive = false;
  server.send(200, "text/plain", "Water Level Calibration Stopped");
}

void processWaterCalibration() {
  waterLevelLowEmpty = waterMinRawValue;
  waterLevelHighFull = waterMaxRawValue;
  server.send(200, "text/plain", "Water Level Calibration Processed");
}

// Function to update the minimum value for the water level sensor
void setminValue() {
  if (server.hasArg("value")) {
    waterMinRawValue = server.arg("value").toInt();  // Convert the received value to an integer
    server.send(200, "text/plain", "Emerged Min value updated");
  } else {
    server.send(400, "text/plain", "Missing value argument");
  }
}

// Function to update the maximum value for the water level sensor
void setmaxValue() {
  if (server.hasArg("value")) {
    waterMaxRawValue = server.arg("value").toInt();  // Convert the received value to an integer
    server.send(200, "text/plain", "Submerged Max value updated");
  } else {
    server.send(400, "text/plain", "Missing value argument");
  }
}




//================================WebPage=========================//
//index.h webpage
void setupWebServerIndex() {
  server.on("/", handleIndex);
  server.on("/DHT11sensorData", HTTP_GET, handleDHT11SensorData);
  server.on("/WatersensorData", HTTP_GET, handleWaterSensorData);
  server.on("/sensorData", HTTP_GET, handleSensorData);
}

//handle for index
void handleIndex() {
  server.send(200, "text/html", PAGE_INDEX);
}

//control.h webpage
void setupWebServerControl() {
  server.on("/control", handleControl);
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
  server.on("/manualPumpOn", HTTP_GET, manualPumpOn);    // Route to turn on pump manually
  server.on("/manualPumpOff", HTTP_GET, manualPumpOff);  // Route to turn off pump manually

  //water level calibration
  server.on("/startWaterCalibrate", HTTP_GET, startWaterCalibrate);
  server.on("/stopWaterCalibrate", HTTP_GET, stopWaterCalibrate);
  server.on("/processWaterCalibration", HTTP_GET, processWaterCalibration);
  server.on("/setminValue", HTTP_POST, setminValue);
  server.on("/setmaxValue", HTTP_POST, setmaxValue);
}

//handle for control
void handleControl() {
  server.send(200, "text/html", getControlHTML);
}


//=======================WiFi Functions========================//

//wifi functions -connectivity.h (Start)- DO NOT TOUCH
void setupAccessPoint() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ap_ssid, ap_password);

  Serial.print("Access Point IP: ");
  Serial.println(WiFi.softAPIP());
}

void setupWebServerConnectivity() {
  server.on("/connectivity", HTTP_GET, handleConnectivity);
  server.on("/scan", HTTP_GET, handleScan);
  server.on("/connect", HTTP_POST, handleConnect);
  server.on("/manual-connect", HTTP_POST, handleManualConnect);
  server.on("/info", HTTP_GET, handleInfo);
  server.onNotFound(handleNotFound);
  server.begin();
}

void handleConnectivity() {
  server.send(200, "text/html", getConnectivityHTML());
}

void handleScan() {
  String json = getScanResultsJSON();
  server.send(200, "application/json", json);
}

void handleConnect() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  if (connectToWiFi(ssid, password)) {
    server.send(200, "text/plain", "Connected to " + ssid + ". IP: " + WiFi.localIP().toString());
  } else {
    server.send(200, "text/plain", "Failed to connect to " + ssid);
  }
}

void handleManualConnect() {
  String ssid = server.arg("manual-ssid");
  String password = server.arg("manual-password");

  if (connectToWiFi(ssid, password)) {
    server.send(200, "text/plain", "Connected to " + ssid + ". IP: " + WiFi.localIP().toString());
  } else {
    server.send(200, "text/plain", "Failed to connect to " + ssid);
  }
}

void handleInfo() {
  String info = "{\n";
  info += "  \"ap_ssid\": \"" + String(ap_ssid) + "\",\n";
  info += "  \"ap_password\": \"" + String(ap_password) + "\",\n";
  info += "  \"ap_ip\": \"" + WiFi.softAPIP().toString() + "\",\n";
  info += "  \"ap_mac\": \"" + WiFi.softAPmacAddress() + "\",\n";
  info += "  \"sta_ssid\": \"" + WiFi.SSID() + "\",\n";
  info += "  \"sta_ip\": \"" + WiFi.localIP().toString() + "\",\n";
  info += "  \"sta_mac\": \"" + WiFi.macAddress() + "\",\n";
  info += "  \"gateway\": \"" + WiFi.gatewayIP().toString() + "\",\n";
  info += "  \"dns\": \"" + WiFi.dnsIP().toString() + "\",\n";

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    info += "  \"datetime\": \"Failed to obtain time\"\n";
  } else {
    char dateTimeStr[64];
    strftime(dateTimeStr, sizeof(dateTimeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
    info += "  \"datetime\": \"" + String(dateTimeStr) + "\"\n";
  }

  info += "}";
  server.send(200, "application/json", info);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}
//wifi functions -connectivity.h (END)- DO NOT TOUCH
