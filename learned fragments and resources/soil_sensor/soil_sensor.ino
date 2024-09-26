//Soil Mosture sensor

// Define the analog pin for the sensor
const int soilPin = 34; // Using GPIO34 (ADC1 Channel 6)

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
}

void loop() {
  // Read the raw analog value from the sensor
  int sensorValue = analogRead(soilPin);

  // Convert the raw value to percentage (optional - requires calibration)
  // Assuming 3800 (dry) to 2000 (wet) for demonstration purposes
  /*
    High Value means dry= 3800
    Low Value means wet=2000
  */

  float moisturePercent = map(sensorValue, 2000, 3800, 100, 0);
  moisturePercent = constrain(moisturePercent, 0, 100);

  /*
    How to use map() function:
    ==========================
      https://www.arduino.cc/reference/en/language/functions/math/map/

      map(sensorValue, 2000, 3800, 100, 0);

      sensorvalue = is the value you want to map.
      2000 = wet value. The lowest.
      3800 = dry value. The hightst.
      100  = 100 %
      0    = 0 % 

      Syntax
      map(value, fromLow, fromHigh, toLow, toHigh)

      Parameters
      value: the number to map.
      fromLow: the lower bound of the value’s current range.
      fromHigh: the upper bound of the value’s current range.
      toLow: the lower bound of the value’s target range.
      toHigh: the upper bound of the value’s target range.

      Returns
      The mapped value. Data type: long.
      
      constrain() function limits the value from 0 to 100.

  */
  

  // Print the raw and percentage values to the Serial Monitor
  Serial.print("Raw Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Soil Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  // Wait for 1 second before the next reading
  delay(1000);
}
