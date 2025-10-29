#include "display.h"
#include <Arduino.h>
#include <array>

// Define constants for pin modes
constexpr uint8_t DATA_PIN = 30;
constexpr uint8_t CLK_PIN = 31;
constexpr uint8_t SHLD_PIN = 32;
constexpr uint8_t DIGITAL_CHANNEL = 6;
constexpr size_t NUM_TOGGLES = 22;
constexpr uint8_t DIGITAL_CC[NUM_TOGGLES] = {
    10, 11, 12, 13, 1, 2,  3,  4,  18, 19, 20,
    21, 9,  8,  7,  0, 14, 15, 16, 17, 5,  6,
};

class Toggle {
public:
  Toggle(uint8_t cc) : cc(cc), state(LOW) {}

  bool update(int newState) {
    bool changed = (newState != state);
    if (newState == HIGH && state == LOW) {
      // Toggle was just toggled off
      usbMIDI.sendControlChange(cc, 0, DIGITAL_CHANNEL);
      // displaySend(cc, DIGITAL_CHANNEL, 0);
    } else if (newState == LOW && state == HIGH) {
      // Toggle was just toggled on
      usbMIDI.sendControlChange(cc, 127, DIGITAL_CHANNEL);
      // displaySend(cc, DIGITAL_CHANNEL, 127);
    }
    state = newState;
    return changed;
  }

  uint8_t getCC() const { return cc; }
  int getState() const { return state; }

private:
  uint8_t cc;
  int state;
};

std::array<Toggle *, NUM_TOGGLES> toggles = {nullptr};

void setupDigitalOuts() {
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

int loopDigitalOuts() {
  constexpr unsigned long cycleInterval = 1; // ms
  static unsigned long lastTic = 0;
  static bool clkHigh = false;
  static unsigned long iteration = 0;

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
      int buttonState = digitalRead(DATA_PIN);
      toggles[readIndex]->update(buttonState);
    }
    if (readIndex >= NUM_TOGGLES - 1) {
      state = CLEAR;
    } else {
      state = ADVANCE;
    }
    readIndex++;
  }
  if (state == ADVANCE) {
    if (now - lastTic >= cycleInterval) {
      digitalWrite(CLK_PIN, clkHigh ? HIGH : LOW);
      lastTic = now;
      clkHigh = !clkHigh;
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
      iteration++;
    }
  }

  return iteration;
}
