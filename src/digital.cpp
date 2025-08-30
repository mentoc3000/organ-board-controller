#include "display.h"
#include <Arduino.h>

// Define constants for pin modes
const uint8_t DATA_PIN = 2;
const uint8_t CLK_PIN = 4;
const uint8_t SHLD_PIN = 3;
const uint8_t DIGITAL_CHANNEL = 1;
const uint8_t NUM_BUTTONS = 1; // Update this if more buttons are added

void cycle_mux() {
  digitalWrite(CLK_PIN, HIGH);
  delay(1);
  digitalWrite(CLK_PIN, LOW);
  delay(1);
}

// Button class
class Button {
public:
  Button(uint8_t cc) : cc(cc), lastState(LOW) {}

  void update(bool currentState) {
    if (currentState == HIGH && lastState == LOW) {
      // Button was just toggled on
      usbMIDI.sendControlChange(cc, 127, DIGITAL_CHANNEL);
      updateDisplay(cc, 127);
    } else if (currentState == LOW && lastState == HIGH) {
      // Button was just toggled off
      usbMIDI.sendControlChange(cc, 0, DIGITAL_CHANNEL);
      updateDisplay(cc, 0);
    }
    lastState = currentState;
  }

private:
  uint8_t cc;
  bool lastState;
};

Button buttons[NUM_BUTTONS];

void setupDigitalIO() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DATA_PIN, INPUT_PULLDOWN);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, LOW);
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    buttons[i] = Button(1 + i); // Assign CC numbers starting from 1
  }
}

void loopDigitalIO() {
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, HIGH);
  delay(1);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    cycle_mux();
    bool buttonState = digitalRead(DATA_PIN);
    buttons[i].update(buttonState);
  }

  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, LOW);
  delay(1);
}
