#include <Arduino.h>

// Define constants for pin modes
const uint8_t DATA_PIN = 2;
const uint8_t CLK_PIN = 4;
const uint8_t SHLD_PIN = 3;
const uint8_t ANALOG_PIN = A0; // Analog input pin 14

void cycle_mux() {
  digitalWrite(CLK_PIN, HIGH);
  delay(1);
  digitalWrite(CLK_PIN, LOW);
  delay(1);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DATA_PIN, INPUT_PULLDOWN);
  pinMode(ANALOG_PIN, INPUT);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, LOW);
  delay(1);
}

// State variable for button edge detection
bool lastButtonState = LOW;
int lastAnalogValue = -1;
const int analogDeltaDetect =
    1; // Only send MIDI if value changes by more than this

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

  int rawAnalog = analogRead(ANALOG_PIN);
  int rawMin = 470;
  int rawMax = 1020; // Assuming 10-bit ADC
  int analogValue = 0;
  if (rawAnalog <= rawMin) {
    analogValue = 0;
  } else if (rawAnalog >= rawMax) {
    analogValue = 127;
  } else {
    analogValue = (rawAnalog - rawMin) * 127 / (rawMax - rawMin);
  }
  // Always send if at min or max, otherwise require minimum delta
  if (lastAnalogValue == -1 || analogValue == 0 || analogValue == 127 ||
      abs(analogValue - lastAnalogValue) > analogDeltaDetect) {
    usbMIDI.sendControlChange(2, analogValue, 1); // CC#2, value, channel 1
    lastAnalogValue = analogValue;
  }

  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, LOW);
  delay(1);

  // Always call this to keep USB MIDI running smoothly
  while (usbMIDI.read()) {
    // Empty loop to process incoming MIDI messages if needed
  }
}
