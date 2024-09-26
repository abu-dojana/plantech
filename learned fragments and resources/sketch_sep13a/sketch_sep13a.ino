#include <ESP32Servo.h>

Servo myservo;  // Create a servo object

void setup() {
  myservo.attach(18);  // Attach the servo to pin 9
}

void loop() {
  // Gradually increase the speed in the clockwise direction
  for (int i = 0; i <= 360; i++) {
    myservo.write(i);
    delay(5);  // Adjust this delay for slower or faster motion
  }

  delay(5000);  // Pause for 1 second

  // Gradually decrease the speed in the counterclockwise direction
  for (int i = 360; i >= 0; i--) {
    myservo.write(i);
    delay(10);  // Adjust this delay for slower or faster motion
  }

  delay(1000);  // Pause for 1 second
}