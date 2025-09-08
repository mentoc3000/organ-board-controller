#include "display.h"
#include <Arduino.h>
#include <array>

// Define constants for pin modes
constexpr uint8_t DATA_PIN = 30;
constexpr uint8_t CLK_PIN = 31;
constexpr uint8_t SHLD_PIN = 32;
constexpr uint8_t DIGITAL_CHANNEL = 2;
constexpr size_t NUM_TOGGLES = 22;
constexpr uint8_t DIGITAL_CC[NUM_TOGGLES] = {
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
};

class Toggle {
public:
  Toggle(uint8_t cc) : cc(cc), lastState(LOW) {}

  bool update(bool currentState) {
    bool changed = (currentState != lastState);
    if (currentState == HIGH && lastState == LOW) {
      // Toggle was just toggled off
      usbMIDI.sendControlChange(cc, 0, DIGITAL_CHANNEL);
      updateDisplay(cc, DIGITAL_CHANNEL, 0);
    } else if (currentState == LOW && lastState == HIGH) {
      // Toggle was just toggled on
      usbMIDI.sendControlChange(cc, 127, DIGITAL_CHANNEL);
      updateDisplay(cc, DIGITAL_CHANNEL, 127);
    }
    lastState = currentState;
    return changed;
  }

  uint8_t getCC() const { return cc; }
  bool getLastState() const { return lastState; }

private:
  uint8_t cc;
  bool lastState;
};

std::array<Toggle *, NUM_TOGGLES> toggles = {nullptr};

void setupDigitalIns() {
  pinMode(DATA_PIN, INPUT_PULLUP);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, LOW);
  for (size_t i = 0; i < NUM_TOGGLES; i++) {
    toggles[i] = new (std::nothrow) Toggle(DIGITAL_CC[i]);
    if (!toggles[i]) {
      // Handle allocation failure
    }
  }
}

enum MuxState { LOAD, READ, ADVANCE, CLEAR };

int loopDigitalIns() {
  constexpr unsigned long cycleInterval = 1; // ms
  static unsigned long lastTic = 0;
  static bool clkHigh = false;

  unsigned long now = millis();
  static MuxState state = LOAD;
  static size_t readIndex = 0;

  if (state == LOAD) {
    if (now - lastTic >= cycleInterval) {
      digitalWrite(CLK_PIN, LOW);
      digitalWrite(SHLD_PIN, HIGH);
      lastTic = now;
      state = READ;
    }
  }
  if (state == READ) {
    if (toggles[readIndex]) {
      bool buttonState = digitalRead(DATA_PIN);
      toggles[readIndex]->update(buttonState);
    }
    if (readIndex >= NUM_TOGGLES - 1) {
      state = CLEAR;
    } else {
      state = ADVANCE;
    }
  }
  if (state == ADVANCE) {
    if (now - lastTic >= cycleInterval) {
      digitalWrite(CLK_PIN, clkHigh ? HIGH : LOW);
      lastTic = now;
      clkHigh = !clkHigh;
      readIndex++;
      state = clkHigh ? ADVANCE : READ;
    }
  }
  if (state == CLEAR) {
    if (now - lastTic >= cycleInterval) {
      digitalWrite(CLK_PIN, LOW);
      digitalWrite(SHLD_PIN, LOW);
      lastTic = now;
      readIndex = 0;
      state = LOAD;
    }
  }

  return 0;
}
