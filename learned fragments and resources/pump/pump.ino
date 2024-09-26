#define PUMP_PIN 25
#define PUMP_PIN_LOW 13
void setup() {
  // put your setup code here, to run once:
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(PUMP_PIN_LOW, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(PUMP_PIN_LOW, HIGH);

    delay(1000);

    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(PUMP_PIN_LOW, LOW);

    delay(1000);
}
