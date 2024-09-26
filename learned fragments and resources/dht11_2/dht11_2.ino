#include "DHT.h"

// Pin Number Variables
const int DHT_PIN = 27;

// Setup DHT
#define DHTTYPE DHT11 
DHT dht(DHT_PIN, DHTTYPE);

// Variables to manage the delay
unsigned long previousMillis = 0;
const long interval = 2000; // Interval at which to read the sensor (in milliseconds)

void setup() {
  // Serial Monitor
  Serial.begin(115200);
  Serial.println("DHT11 test!");

  // Initialize the DHT sensor
  dht.begin();
}

void loop() {
  // Get the current time
  unsigned long currentMillis = millis();

  // Check if the interval has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Call the DHT sensor reading function
    dht_sensor();
  }
}

void dht_sensor() {
  /* Reading temperature or humidity takes about 250 milliseconds!
     Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  */

  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Read humidity as percent
  float h = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hix = dht.computeHeatIndex(t, h, false);

  // Output the readings to the Serial Monitor
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% "));

  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  Serial.print(F("Heat index (Feels like): "));
  Serial.print(hix);
  Serial.print(F("°C "));

  Serial.println();
}
