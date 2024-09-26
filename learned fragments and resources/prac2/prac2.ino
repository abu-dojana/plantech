/*
  2. create another webpage and add buttons to change from one page to another (index.h to home.h).  
*/


// Library for wifi and webserver.
#include <WiFi.h>
#include <WebServer.h>

//Webpage Libraries
#include "index.h" 
#include "home.h"

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

  //load home page
  server.on("/HOME", SendHOME);

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


void SendHOME() {
  server.send(200, "text/html", PAGE_HOME);
  
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
