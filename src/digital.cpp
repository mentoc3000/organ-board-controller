#include "display.h"
#include <Arduino.h>
#include <array>

// Define constants for pin modes
constexpr uint8_t DATA_PIN = 30;
constexpr uint8_t CLK_PIN = 31;
constexpr uint8_t SHLD_PIN = 32;
constexpr uint8_t DIGITAL_CHANNEL = 1;
constexpr size_t NUM_TOGGLES = 22;
constexpr uint8_t LED_PIN = 5;

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

void setupDigitalIO() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT_PULLUP);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(SHLD_PIN, LOW);
  for (size_t i = 0; i < NUM_TOGGLES; i++) {
    toggles[i] =
        new (std::nothrow) Toggle(1 + i); // Assign CC numbers starting from 1
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

void loopLED() {
  static unsigned long lastCycleTime = 0;
  constexpr unsigned long cycleInterval = 1000;
  unsigned long now = millis();
  static bool ledState = false;

  if (now - lastCycleTime >= cycleInterval) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    lastCycleTime = now;
  }
}

void loopDigitalIO() {
  loopDigitalIns();
  loopLED();
}
