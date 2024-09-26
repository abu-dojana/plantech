#include "DHT.h"

//Pin Number Variables
const int  DHT_PIN = 27;

//Setup DHT
#define DHTTYPE DHT11 
DHT dht(DHT_PIN, DHTTYPE);

//function formilation
void dht_sensor();

void setup() {
  // put your setup code here, to run once:

  //Serial Monitor
  Serial.begin(115200);
  Serial.println("DHT11 test!");


  dht.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  dht_sensor();
}



void dht_sensor(){
  /* Reading temperature or humidity takes about 250 milliseconds!
     Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  */
  delay(2000);

  // Read temperature as Celsius (the default)
  float t= dht.readTemperature(); 

  //Read humidity as percent
  float h= dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hix = dht.computeHeatIndex(t, h, false);


  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% "));

  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  Serial.print(F("Heat index (Feels like): "));
  Serial.print(hix);
  Serial.print(F("°C "));

  Serial.println("");
}
