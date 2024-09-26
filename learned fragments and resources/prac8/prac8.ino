//include section 
#include <WiFi.h> //wifi include
#include <WebServer.h> //server include
#include <DNSServer.h> //DNS include
#include "connectivity.h"  // Include the header file with WiFi functions and HTML content
#include <time.h> //time for cennectivity.h


//wifi variables and objects- start
const char* ap_ssid = "PlanTech";
const char* ap_password = "admin123";

IPAddress apIP(192, 168, 10, 10);
DNSServer dnsServer;
WebServer server(80);

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
//wifi variables and objects- end


void setup() {
  Serial.begin(115200);
  
  //wifi- start
  setupAccessPoint(); //wifi AP
  setupWebServer(); //server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //time config for connectivity.h
  
  Serial.println("Access Point and Web Server started");
  //wifi- end

}

void loop() {
  dnsServer.processNextRequest(); //dns wifi
  server.handleClient(); //request handler
}







//wifi functions -connectivity.h
void setupAccessPoint() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ap_ssid, ap_password);
  
  Serial.print("Access Point IP: ");
  Serial.println(WiFi.softAPIP());
}

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/scan", HTTP_GET, handleScan);
  server.on("/connect", HTTP_POST, handleConnect);
  server.on("/manual-connect", HTTP_POST, handleManualConnect);
  server.on("/info", HTTP_GET, handleInfo);
  server.onNotFound(handleNotFound);
  server.begin();
}

void handleRoot() {
  server.send(200, "text/html", getHTML());
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