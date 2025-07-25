#include <Arduino.h>

// Define constants for pin modes
const uint8_t DATA_PIN = 2;
const uint8_t CLK_PIN = 4;
const uint8_t SHLD_PIN = 3;

void cycle_mux() {
  digitalWrite(CLK_PIN, HIGH);
  delay(1);
  digitalWrite(CLK_PIN, LOW);
  delay(1);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DATA_PIN, INPUT_PULLDOWN);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, LOW);
  delay(1);
}

void loop() {
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, HIGH);
  delay(1);

  int n = 15; // Number of cycles to perform
  for (int i = 0; i < n; i++) {
    cycle_mux();
  }

  if (digitalRead(DATA_PIN) == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, LOW);
  delay(1);
}
