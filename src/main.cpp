#include <Arduino.h>

// Define constants for pin modes
const uint8_t DATA_PIN = 2;  // Example pin for data input
const uint8_t CLK1_PIN = 4;  // Example pin for clock input
const uint8_t SHLD1_PIN = 3; // Example pin for shift input
const uint8_t CLK2_PIN = 6;  // Example pin for second clock input
const uint8_t SHLD2_PIN = 5; // Example pin for second shift input

// define button_signal funciton
void button_signal() {
  if (digitalRead(DATA_PIN) == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void cycle_mux(uint8_t mux) {
  if (mux == 1) {
    digitalWrite(CLK1_PIN, HIGH); // Set clock pin high
    delay(10);                    // Small delay for stability
    digitalWrite(CLK1_PIN, LOW);  // Set shift pin low
  } else if (mux == 2) {
    digitalWrite(CLK2_PIN, HIGH); // Set clock pin high
    delay(10);                    // Small delay for stability
    digitalWrite(CLK2_PIN, LOW);  // Set shift pin low
  }
  delay(10); // Small delay for stability
}

void setup() {
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize pin 2 as input with pull-down resistor
  pinMode(DATA_PIN, INPUT_PULLDOWN);
  digitalWrite(CLK1_PIN, LOW);  // Set clock pin low
  digitalWrite(SHLD1_PIN, LOW); // Set shift pin low
  delay(10);                    // Small delay for stability
}

void loop() {
  digitalWrite(CLK1_PIN, LOW);   // Set clock pin low
  digitalWrite(SHLD1_PIN, HIGH); // Set shift pin low
  delay(10);                     // Small delay for stability

  cycle_mux(1);    // Cycle through the first multiplexer
  cycle_mux(1);    // Cycle through the first multiplexer
  button_signal(); // Call the button signal function

  digitalWrite(CLK1_PIN, LOW);  // Set clock pin low
  digitalWrite(SHLD1_PIN, LOW); // Set shift pin low

  delay(100); // small delay to debounce
}
