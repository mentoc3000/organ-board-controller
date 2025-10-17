#include "display.h"
#include "storage.h"
#include <Arduino.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <tuple>

constexpr uint8_t ANALOG_CHANNEL = 1;
constexpr size_t NUM_KNOBS = 14;
constexpr int ANALOG_DELTA_DETECT = 1;

struct KnobConfig {
  uint8_t pin;
  uint8_t cc;
  int rawMin;
  int rawMax;
};

const std::array<KnobConfig, NUM_KNOBS> KNOB_CONFIGS = {
    KnobConfig{A0, 9},   KnobConfig{A1, 8},   KnobConfig{A6, 3},
    KnobConfig{A7, 6},   KnobConfig{A8, 5},   KnobConfig{A9, 7},
    KnobConfig{A10, 0},  KnobConfig{A11, 1},  KnobConfig{A12, 2},
    KnobConfig{A13, 4},  KnobConfig{A14, 13}, KnobConfig{A15, 12},
    KnobConfig{A16, 11}, KnobConfig{A17, 10}};

class Knob {
public:
  static constexpr int medianWindow = 5;

  Knob(uint8_t pin, uint8_t cc, int rawMin, int rawMax)
      : pin(pin), cc(cc), lastAnalogValue(-1), analogMoved(false),
        rawMin(rawMin), rawMax(rawMax) {
    for (int i = 0; i < medianWindow; i++)
      samples[i] = 0;
  }

  int update(int value) {
    // Median filter for analog input
    samples[sampleIndex] = value;
    sampleIndex = (sampleIndex + 1) % medianWindow;
    float scale = static_cast<float>(rawMax - rawMin) / 127.0f;

    int sorted[medianWindow];
    memcpy(sorted, samples, sizeof(sorted));
    std::sort(sorted, sorted + medianWindow);
    int raw = sorted[medianWindow / 2];
    float rawClipped = 0.0;
    if (raw <= rawMin) {
      rawClipped = 0.0f;
    } else if (raw >= rawMax) {
      rawClipped = static_cast<float>(rawMax - rawMin);
    } else {
      rawClipped = static_cast<float>(raw - rawMin);
    }
    int analogValue = std::round(rawClipped / scale);

    // Only start sending analog MIDI after the dial is moved from its initial
    // value
    if (!analogMoved) {
      if (lastAnalogValue == -1) {
        lastAnalogValue = analogValue;
      } else if (analogValue != lastAnalogValue) {
        analogMoved = true;
      }
    }
    if (analogMoved &&            // the value has changed AND
        (lastAnalogValue == -1 || // this is the first change OR
         (lastAnalogValue != 0 && analogValue == 0) ||     // the min was hit OR
         (lastAnalogValue != 127 && analogValue == 127) || // the max was hit OR
         abs(rawClipped - lastAnalogValue * scale) >
             rawDeltaDetect)) // the change was large enough
    {
      usbMIDI.sendControlChange(cc, analogValue, ANALOG_CHANNEL);
      lastAnalogValue = analogValue;
      return analogValue;
    } else {
      return -1;
    }
  }

  uint8_t getPin() const { return pin; }
  uint8_t getCC() const { return cc; }

private:
  uint8_t pin;
  uint8_t cc;
  int lastAnalogValue;
  bool analogMoved;
  int samples[medianWindow];
  int sampleIndex = 0;
  int rawMin;
  int rawMax;
  static constexpr int rawDeltaDetect = 6;
};

std::array<Knob *, NUM_KNOBS> knobs = {nullptr};

void setupAnalogIO() {
  int rawMin, rawMax;
  for (size_t i = 0; i < NUM_KNOBS; i++) {
    const KnobConfig &cfg = KNOB_CONFIGS[i];
    pinMode(cfg.pin, INPUT);
    loadCalibrationData(cfg.pin, rawMin, rawMax);
    knobs[i] = new (std::nothrow) Knob(cfg.pin, cfg.cc, rawMin, rawMax);
    if (!knobs[i]) {
      // Handle allocation failure
    }
  }

  for (int i = 0; i < knobs[0]->medianWindow; i++) {
    for (Knob *knob : knobs) {
      if (!knob)
        continue; // Safety check for failed allocation
      int rawValue = analogRead(knob->getPin());
      knob->update(rawValue); // Initialize the knob state
    }
  }
}

bool checkCalibrationTrigger() {
  const int high = 900;
  const int low = 100;

  int cc3_val = analogRead(KNOB_CONFIGS[2].pin); // CC 3
  int cc4_val = analogRead(KNOB_CONFIGS[9].pin); // CC 4
  int cc5_val = analogRead(KNOB_CONFIGS[4].pin); // CC 5
  int cc6_val = analogRead(KNOB_CONFIGS[3].pin); // CC 6

  // Show the values on the display so you can see them
  delay(3000); // Give time to read the display

  return cc3_val > high && cc4_val < low && cc5_val < low && cc6_val > high;
}

void loopAnalogIO() {
  for (Knob *knob : knobs) {
    if (!knob)
      continue; // Safety check for failed allocation
    int rawValue = analogRead(knob->getPin());
    int ccValue = knob->update(rawValue);
    if (ccValue != -1) {
      displaySend(knob->getCC(), ANALOG_CHANNEL, ccValue);
    }
  }
}

void clearAnalogCalibration() {
  for (const KnobConfig &cfg : KNOB_CONFIGS) {
    saveCalibrationData(cfg.pin, 300, 600);
  }
}

void loopAnalogCalibration() {
  int rawMin, rawMax, newMin, newMax;
  for (KnobConfig cfg : KNOB_CONFIGS) {
    int raw = analogRead(cfg.pin);
    loadCalibrationData(cfg.pin, rawMin, rawMax);
    newMin = std::min(rawMin, raw);
    newMax = std::max(rawMax, raw);
    if (newMin < rawMin || newMax > rawMax) {
      displayCalibratingKnob(cfg.cc);
      saveCalibrationData(cfg.pin, newMin, newMax);
    }
  }
  delay(200);
}
