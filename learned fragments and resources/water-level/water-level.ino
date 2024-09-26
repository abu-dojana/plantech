//Water Level Sensor

// Define the analog pin for the sensor
const int water_sensor_pin = 34; // Using GPIO34 (ADC1 Channel 6)

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
}

void loop() {
  // Read the raw analog value from the sensor
  int sensorValue = analogRead(water_sensor_pin);

  // Convert the raw value to percentage (optional - requires calibration)
  // Assuming 3800 (dry) to 2000 (wet) for demonstration purposes
  /*
    0    to 1300 no water.
    2200 to 4095 overflow (full).
    1900 to 2200 => 0% to 100%
  */

  float level_percentage = map(sensorValue, 1500, 2200, 0, 100);
  level_percentage = constrain(level_percentage, 0, 100);
  
  // sensor length is 400mm
  float level_mm = map(sensorValue, 1500, 2200, 0, 400);
  level_mm = constrain(level_mm, 0, 400);

  /*
    How to use map() function:
    ==========================
      https://www.arduino.cc/reference/en/language/functions/math/map/
  */
  

  // Print the raw and percentage values to the Serial Monitor
  Serial.print("Raw Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Water level Percent: ");
  Serial.print(level_percentage);
  Serial.print("%");
  Serial.print(" | Water level: ");
  Serial.print(level_mm);
  Serial.println(" mm");

  // Wait for 1 second before the next reading
  delay(1000);
}
