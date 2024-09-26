/*
  3. Create a normal webpage that will turn a LED on and off using button on the webpage.
    i. another led will be toggle from home. 
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



//led pin 
const int ledPin=2;
int ledState = LOW;

const int ledPin2=5;
int ledState2 = LOW;



void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  connectToWifi();

    //ledpin conf
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin, ledState);


  //ledpin2 conf
  pinMode(ledPin2,OUTPUT);
  digitalWrite(ledPin2, ledState2);

  // Route for root / web page
  server.on("/", SendWebsite);

  //load home page
  server.on("/HOME", SendHOME);

  // Handle LED toggle
  server.on("/toggleLED", HTTP_GET, handleLEDToggle);
  server.on("/toggleLED2", HTTP_GET, handleLEDToggle2);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started!");

}

void loop() {
  server.handleClient();
}

void connectToWifi(){

    // Connect to Wi-Fi
  WiFi.begin(ROUTER_SSID, ROUTER_PASS);
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
void SendWebsite() {
  server.send(200, "text/html", PAGE_INDEX);
}

void SendHOME() {
  server.send(200, "text/html", PAGE_HOME);
}


void handleLEDToggle() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
  String response = ledState ? "LED is ON" : "LED is OFF";
  server.send(200, "text/plain", response);
}

void handleLEDToggle2() {
  ledState2 = !ledState2;
  digitalWrite(ledPin2, ledState2);
  String response = ledState2 ? "LED is ON" : "LED is OFF";
  server.send(200, "text/plain", response);
}



