#include <Arduino.h>

// Define constants for pin modes
const uint8_t CLK_PIN = 2;  // Example pin for clock input
const uint8_t SHLD_PIN = 3; // Example pin for shift input
const uint8_t DATA_PIN = 4; // Example pin for data input

// define button_signal funciton
void button_signal() {
  // This function can be used to handle button signals
  // For example, it can read the state of a button and perform actions
  if (digitalRead(DATA_PIN) == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(200);                     // small delay to debounce
  digitalWrite(LED_BUILTIN, LOW); // turn off the LED after reading
  delay(200);                     // wait for a second before next action
}

void setup() {
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize pin 2 as input with pull-down resistor
  pinMode(DATA_PIN, INPUT_PULLDOWN);
  digitalWrite(CLK_PIN, LOW);  // Set clock pin low
  digitalWrite(SHLD_PIN, LOW); // Set shift pin low
}

void loop() {
  // Start signal
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW
  delay(1000);                     // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED on by making the
  delay(1000);                     // wait for a second

  digitalWrite(CLK_PIN, LOW);   // Set clock pin low
  digitalWrite(SHLD_PIN, HIGH); // Set shift pin low
  delay(10);                    // Small delay for stability

  button_signal();             // Call the button signal function
  digitalWrite(CLK_PIN, HIGH); // Set clock pin high
  delay(10);                   // Small delay for stability
  digitalWrite(CLK_PIN, LOW);  // Set shift pin low
  button_signal();             // Call the button signal function
  digitalWrite(CLK_PIN, HIGH); // Set clock pin high
  delay(10);                   // Small delay for stability
  digitalWrite(CLK_PIN, LOW);  // Set shift pin low
  button_signal();             // Call the button signal function

  digitalWrite(CLK_PIN, LOW);  // Set clock pin low
  digitalWrite(SHLD_PIN, LOW); // Set shift pin low

  delay(1000); // small delay to debounce
}
