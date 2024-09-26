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


// Pin definitions
#define water_sensor_pin 35  // Analog pin for soil moisture sensor
#define ledpin_water 2

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
}

void loop() {
  dnsServer.processNextRequest();  //dns wifi
  server.handleClient();           //request handler

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval_water_loop) {
    handleWaterSensorData();  //ensures water reading is running in background
  }
}

//================sensors data functions===============//
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
void handleWaterSensorData() {

  // Convert the raw value to percentage (optional - requires calibration)
  // Assuming 3800 (dry) to 2000 (wet) for demonstration purposes
  /*
    0    to 1500 no water.
    2200 to 4095 overflow (full).
    1900 to 2200 => 0% to 100%
  */

  int water_level_low_max_empty = 1500;
  int water_level_high_min_full = 2200;

  // Read the raw analog value from the sensor
  int waterSensorValueRAW = analogRead(water_sensor_pin);

  float water_level_percentage = map(waterSensorValueRAW, water_level_low_max_empty, water_level_high_min_full, 0, 100);
  water_level_percentage = constrain(water_level_percentage, 0, 100);

  // sensor length is 400mm
  float water_level_mm = map(waterSensorValueRAW, water_level_low_max_empty, water_level_high_min_full, 0, 400);
  water_level_mm = constrain(water_level_mm, 0, 400);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval_water_loop) {
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
  json += "\"water_level_mm\":" + String(water_level_mm);
  json += "}";

  server.send(200, "application/json", json);
}




//================================WebPage=========================//
//index.h webpage
void setupWebServerIndex() {
  server.on("/", handleIndex);
  server.on("/DHT11sensorData", HTTP_GET, handleDHT11SensorData);
  server.on("/WatersensorData", HTTP_GET, handleWaterSensorData);
}

//handle for index
void handleIndex() {
  server.send(200, "text/html", PAGE_INDEX);
}

//control.h webpage
void setupWebServerControl() {
  server.on("/control", handleControl);
}

//handle for control
void handleControl() {
  server.send(200, "text/html", getControlHTML());
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
