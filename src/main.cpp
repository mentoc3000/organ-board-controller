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

// State variable for button edge detection
bool lastButtonState = LOW;

void loop() {
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, HIGH);
  delay(1);

  int n = 15; // Number of cycles to perform
  for (int i = 0; i < n; i++) {
    cycle_mux();
  }

  bool buttonState = digitalRead(DATA_PIN);
  if (buttonState == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
    if (lastButtonState == LOW) {
      // Button was just pressed, send MIDI CC
      usbMIDI.sendControlChange(1, 127, 1); // CC#1, value 127, channel 1
    }
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    if (lastButtonState == HIGH) {
      // Button was just released, send MIDI CC off (optional)
      usbMIDI.sendControlChange(1, 0, 1); // CC#1, value 0, channel 1
    }
  }
  lastButtonState = buttonState;

  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, LOW);
  delay(1);

  // Always call this to keep USB MIDI running smoothly
  while (usbMIDI.read()) {
    // Empty loop to process incoming MIDI messages if needed
  }
}
