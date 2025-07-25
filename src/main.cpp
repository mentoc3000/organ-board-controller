#include <Arduino.h>

// Define constants for pin modes
const uint8_t DATA_PIN = 2; // Example pin for data input
const uint8_t CLK_PIN = 4;  // Example pin for clock input
const uint8_t SHLD_PIN = 3; // Example pin for shift input

void cycle_mux() {
  digitalWrite(CLK_PIN, HIGH); // Set clock pin high
  delay(1);                    // Small delay for stability
  digitalWrite(CLK_PIN, LOW);  // Set shift pin low
  delay(1);                    // Small delay for stability
}

void setup() {
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize pin 2 as input with pull-down resistor
  pinMode(DATA_PIN, INPUT_PULLDOWN);
  digitalWrite(CLK_PIN, LOW);  // Set clock pin low
  digitalWrite(SHLD_PIN, LOW); // Set shift pin low
  delay(1);                    // Small delay for stability
}

void loop() {
  digitalWrite(CLK_PIN, LOW);   // Set clock pin low
  digitalWrite(SHLD_PIN, HIGH); // Set shift pin low
  delay(1);                     // Small delay for stability

  int n = 15; // Number of cycles to perform
  for (int i = 0; i < n; i++) {
    cycle_mux(); // Cycle through the first multiplexer
  }

  if (digitalRead(DATA_PIN) == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  digitalWrite(CLK_PIN, LOW);  // Set clock pin low
  digitalWrite(SHLD_PIN, LOW); // Set shift pin low
  delay(1);                    // small delay to debounce
}
