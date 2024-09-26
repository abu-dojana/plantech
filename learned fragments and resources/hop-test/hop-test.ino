//include section 
#include <WiFi.h> //wifi include
#include <WebServer.h> //server include
#include <DNSServer.h> //DNS include
#include <time.h> //time for cennectivity.h
#include "connectivity.h"  // Include the header file with WiFi functions and HTML content
#include "index.h"
#include "control.h"
#include <DHT.h>
#include <ESP32Servo.h>

//wifi variables and objects- start
const char* ap_ssid = "PlanTech";
const char* ap_password = "admin123";

IPAddress apIP(192, 168, 10, 10);
DNSServer dnsServer;
WebServer server(80);

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 21600;  // Bangladesh GMT +6
const int   daylightOffset_sec = 0;
//wifi variables and objects- end

// Pin definitions
#define SOIL_MOISTURE_PIN 34
#define WATER_LEVEL_PIN 35
#define DHT_PIN 27
#define PUMP_PIN 26
#define PUMP_LED_PIN 25
#define LOW_WATER_LED_PIN 33
#define SERVO_PIN 32

// Sensor objects
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);
Servo myservo;

// Variables for soil moisture sensor
int soilMoistureRaw = 0;
int wetValue_low = 0;
int dryValue_high = 4095;
float soilMoisturePercent = 0;
int soilMoistureThreshold = 34;  // Default threshold for pump activation

// Variables for water level sensor
int waterLevelRaw = 0;
float waterLevelPercent = 0;
float waterLevelMM = 0;

// Variables for pump control
bool pumpActive = false;
unsigned long pumpActivationTime = 0;
unsigned long pumpDuration = 2000;  // Default pump duration (2 seconds)
int pumpActivationCount = 0;

// Variables for system info
unsigned long startTime;
int wateringCount = 0;

void setup() {
  Serial.begin(115200);
  
  //wifi- start
  setupAccessPoint(); //wifi AP
  setupWebServerConnectivity(); //connectivity.h server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //time config for connectivity.h
  
  Serial.println("Access Point and Web Server started");
  //wifi- end

  setupWebServerIndex(); //index.h server
  setupWebServerControl(); //control.h server

  dht.begin();
  myservo.attach(SERVO_PIN);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(PUMP_LED_PIN, OUTPUT);
  pinMode(LOW_WATER_LED_PIN, OUTPUT);

  startTime = millis();
}



void loop() {
  dnsServer.processNextRequest(); //dns wifi
  server.handleClient(); //request handler
  
  readSensors();
  controlPump();

  if (calibrateActive == true){
    calibrateSoilMoisture();
  }
}

//index.h webpage
void setupWebServerIndex() {
    server.on("/", handleIndex);
    server.on("/indexData", HTTP_GET, handleIndexData);
}

void handleIndex() {
  server.send(200, "text/html", getIndexHTML());
}

void handleIndexData() {
  String json = "{";
  json += "\"soilMoisture\":" + String(soilMoisturePercent) + ",";
  json += "\"soilMoistureRaw\":" + String(soilMoistureRaw) + ",";
  json += "\"soilMoistureThreshold\":" + String(soilMoistureThreshold) + ",";
  json += "\"waterLevelPercent\":" + String(waterLevelPercent) + ",";
  json += "\"waterLevelMM\":" + String(waterLevelMM) + ",";
  json += "\"waterLevelRaw\":" + String(waterLevelRaw) + ",";
  json += "\"temperature\":" + String(dht.readTemperature()) + ",";
  json += "\"humidity\":" + String(dht.readHumidity()) + ",";
  json += "\"heatIndex\":" + String(dht.computeHeatIndex(dht.readTemperature(), dht.readHumidity(), false)) + ",";
  json += "\"pumpActivationCount\":" + String(pumpActivationCount) + ",";
  json += "\"uptime\":" + String((millis() - startTime) / 1000) + ",";
  json += "\"wateringCount\":" + String(wateringCount) + ",";
  json += "\"currentTime\":\"" + getCurrentTime() + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

String getCurrentTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "Failed to obtain time";
  }
  char timeString[30];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeString);
}

//control.h webpage
// void setupWebServerControl() {
//     server.on("/control", handleControl);
// }

void setupWebServerControl() {
    server.on("/control", HTTP_GET, handleControl);
    server.on("/setWetValueLow", HTTP_POST, handleSetWetValueLow);
    server.on("/setDryValueHigh", HTTP_POST, handleSetDryValueHigh);
    server.on("/activatePump", HTTP_GET, handleActivatePump);
    server.on("/setPumpDuration", HTTP_POST, handleSetPumpDuration);
    server.on("/setSoilMoistureThreshold", HTTP_POST, handleSetSoilMoistureThreshold);
    server.on("/resetPumpActivationCount", HTTP_GET, handleResetPumpActivationCount);
    server.on("/setServoAngle", HTTP_POST, handleSetServoAngle);
    server.on("/startCalibrate", HTTP_GET, handleStartCalibrate);
    server.on("/stopCalibrate", HTTP_GET, handleStopCalibrate);
    server.on("/processCalibration", HTTP_GET, handleProcessCalibration);
}

// void handleControl() {
//   server.send(200, "text/html", getControlHTML());
  
// }

// void activatePump(){
//   digitalWrite(PUMP_PIN, HIGH);
//   digitalWrite(PUMP_LED_PIN, HIGH);
//   pumpActive = true;
//   pumpStartTime = millis();
//   pumpActivationCount++;
//   wateringCount++;
// }

//wifi functions -connectivity.h (Start)
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
  if(!getLocalTime(&timeinfo)){
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
//wifi functions -connectivity.h (END)

void readSensors() {
  // Read soil moisture sensor
  soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  soilMoisturePercent = map(soilMoistureRaw, wetValue_low, dryValue_high, 100, 0);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

  // Read water level sensor
  waterLevelRaw = analogRead(WATER_LEVEL_PIN);
  waterLevelPercent = map(waterLevelRaw, 1500, 2200, 0, 100);
  waterLevelPercent = constrain(waterLevelPercent, 0, 100);
  waterLevelMM = map(waterLevelRaw, 1500, 2200, 0, 400);
  waterLevelMM = constrain(waterLevelMM, 0, 400);
}

void controlPump() {
  if (waterLevelPercent < 15) {
    digitalWrite(LOW_WATER_LED_PIN, HIGH);  // Turn on the low water level LED
    pumpActive = false;
    digitalWrite(PUMP_PIN, LOW);  // Turn off the pump
    digitalWrite(PUMP_LED_PIN, LOW);  // Turn off the pump LED
  } else {
    digitalWrite(LOW_WATER_LED_PIN, LOW);  // Turn off the low water level LED
    
    if (soilMoisturePercent < soilMoistureThreshold && !pumpActive) {
      // Activate the pump
      pumpActive = true;
      digitalWrite(PUMP_PIN, HIGH);
      digitalWrite(PUMP_LED_PIN, HIGH);
      pumpActivationTime = millis();
      pumpActivationCount++;
    }

    if (pumpActive && millis() - pumpActivationTime >= pumpDuration) {
      // Deactivate the pump after the specified duration
      pumpActive = false;
      digitalWrite(PUMP_PIN, LOW);
      digitalWrite(PUMP_LED_PIN, LOW);
      wateringCount++;
    }
  }
}
