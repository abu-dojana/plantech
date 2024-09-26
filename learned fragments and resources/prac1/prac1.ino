/*
    This file contains the normal web server that connects to a router and show a webpage.
    Steps to write code:
    1. Connect to a router and get IP. [check]
    2. Creating a server. [check]
    3. Create a webpage. [check]
*/

// Library for wifi and webserver.
#include <WiFi.h>
#include <WebServer.h>

//Webpage Libraries
#include "index.h" 


//Network credentials (router credentials) for connecting to router
#define ROUTER_SSID "abudozana"
#define ROUTER_PASS "Darda@55"

//Instance for creating web server at port 80
WebServer server(80);



void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);

  // Connect to Wi-Fi
  connectToWifi();

  // Route for root / web page
  server.on("/", SendWebsite);

  // Start server
  server.begin();
  Serial.println("HTTP server started!");


}

void loop() {
  // put your main code here, to run repeatedly:
      server.handleClient();
}


void SendWebsite() {
  server.send(200, "text/html", PAGE_INDEX);
  
}



void connectToWifi(){

    // Connect to Wi-Fi
  WiFi.begin(ROUTER_SSID, ROUTER_PASS);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print ESP32 IP Address
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ROUTER_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}
