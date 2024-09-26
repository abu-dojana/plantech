#include <WiFi.h>
#include <WebServer.h>
#include "WebPage.h"  // Include the header file with the HTML content

const char* ap_ssid = "PlanTech";
const char* ap_password = "admin";

const char* sta_ssid = "abudozana";
const char* sta_password = "Darda@55";

// Static IP configuration
IPAddress local_IP(192, 168, 1, 420);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  wifiAccessPoint();
  wifiStation();
  
  server.on("/", handleRoot);
  server.on("/connect", HTTP_POST, handleConnect);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void wifiAccessPoint() {
  Serial.println("Setting AP (Access Point)...");
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void wifiStation() {
  Serial.println("Configuring Station mode...");
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Connecting to ");
  Serial.println(sta_ssid);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void handleRoot() {
  String html = MAIN_page;
  html.replace("{{AP_SSID}}", String(ap_ssid));
  html.replace("{{STA_SSID}}", WiFi.SSID());
  html.replace("{{STA_IP}}", WiFi.localIP().toString());
  html.replace("{{AP_IP}}", WiFi.softAPIP().toString());
  server.send(200, "text/html", html);
}

void handleConnect() {
  String newSsid = server.arg("ssid");
  String newPassword = server.arg("password");
  
  if (newSsid.length() > 0 && newPassword.length() > 0) {
    WiFi.disconnect();
    WiFi.begin(newSsid.c_str(), newPassword.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      server.send(200, "text/plain", "Connected to " + newSsid);
    } else {
      server.send(200, "text/plain", "Failed to connect to " + newSsid);
    }
  } else {
    server.send(400, "text/plain", "Invalid SSID or password");
  }
}