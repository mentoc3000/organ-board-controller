#include "display.h"
#include <Arduino.h>
#include <array>

// Define constants for pin modes
constexpr uint8_t DATA_PIN = 30;
constexpr uint8_t CLK_PIN = 31;
constexpr uint8_t SHLD_PIN = 32;
constexpr uint8_t DIGITAL_CHANNEL = 1;
constexpr size_t NUM_TOGGLES = 22;
constexpr uint8_t DIGITAL_CC[NUM_TOGGLES] = {
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
};

void cycle_mux() {
  static unsigned long lastCycleTime = 0;
  static bool clkHigh = false;
  unsigned long now = millis();
  if (!clkHigh && now - lastCycleTime >= 1) {
    digitalWrite(CLK_PIN, HIGH);
    clkHigh = true;
    lastCycleTime = now;
  } else if (clkHigh && now - lastCycleTime >= 1) {
    digitalWrite(CLK_PIN, LOW);
    clkHigh = false;
    lastCycleTime = now;
  }
}

class Toggle {
public:
  Toggle(uint8_t cc) : cc(cc), lastState(LOW) {}

  bool update(bool currentState) {
    bool changed = (currentState != lastState);
    if (currentState == HIGH && lastState == LOW) {
      // Toggle was just toggled off
      usbMIDI.sendControlChange(cc, 0, DIGITAL_CHANNEL);
      updateDisplay(cc, 127);
    } else if (currentState == LOW && lastState == HIGH) {
      // Toggle was just toggled on
      usbMIDI.sendControlChange(cc, 127, DIGITAL_CHANNEL);
      updateDisplay(cc, 0);
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

int loopDigitalIns() {
  constexpr unsigned long cycleInterval = 1; // ms
  unsigned long now = millis();

  static unsigned long lastSHLDTime = 0;
  static bool shldHigh = false;
  if (!shldHigh && now - lastSHLDTime >= cycleInterval) {
    digitalWrite(CLK_PIN, LOW);
    digitalWrite(SHLD_PIN, HIGH);
    shldHigh = true;
    lastSHLDTime = now;
  } else if (shldHigh && now - lastSHLDTime >= cycleInterval) {
    for (size_t i = 0; i < NUM_TOGGLES; i++) {
      cycle_mux();
      if (!toggles[i])
        continue; // Safety check for failed allocation
      bool buttonState = digitalRead(DATA_PIN);
      toggles[i]->update(buttonState);
    }
    digitalWrite(CLK_PIN, LOW);
    digitalWrite(SHLD_PIN, LOW);
    shldHigh = false;
    lastSHLDTime = now;
  }

  return 0;
}
