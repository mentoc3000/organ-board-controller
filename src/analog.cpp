#include "display.h"
#include "storage.h"
#include <Arduino.h>
#include <algorithm>
#include <array>
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

    // TODO: handle inital case where there are fewer than medianWindow samples

    int sorted[medianWindow];
    memcpy(sorted, samples, sizeof(sorted));
    std::sort(sorted, sorted + medianWindow);
    int rawAnalog = sorted[medianWindow / 2];
    int analogValue = 0;
    if (rawAnalog <= rawMin) {
      analogValue = 0;
    } else if (rawAnalog >= rawMax) {
      analogValue = 127;
    } else {
      analogValue = (rawAnalog - rawMin) * 127 / (rawMax - rawMin);
    }

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
         abs(analogValue - lastAnalogValue) >
             analogDeltaDetect)) // the change was large enough
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
  uint8_t getLastAnalogValue() const { return lastAnalogValue; }

private:
  uint8_t pin;
  uint8_t cc;
  int lastAnalogValue;
  bool analogMoved;
  static constexpr int medianWindow = 5;
  int samples[medianWindow];
  int sampleIndex = 0;
  int rawMin;
  int rawMax;
  static constexpr int analogDeltaDetect = 1;
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
}

bool checkCalibrationTrigger() {
  const int high = 120;
  const int low = 10;
  displaySend(knobs[2]->getCC(), ANALOG_CHANNEL,
              knobs[2]->getLastAnalogValue());
  return knobs[2]->getLastAnalogValue() > high && // CC 3
         knobs[9]->getLastAnalogValue() < low &&  // CC 4
         knobs[4]->getLastAnalogValue() < low &&  // CC 5
         knobs[3]->getLastAnalogValue() > high;   // CC 6
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
    saveCalibrationData(cfg.pin, 512, 512);
  }
}

void loopAnalogCalibration() {
  int rawMin, rawMax, newMin, newMax;
  for (KnobConfig cfg : KNOB_CONFIGS) {
    int rawValue = analogRead(cfg.pin);
    loadCalibrationData(cfg.pin, rawMin, rawMax);
    newMin = std::min(rawMin, rawValue);
    newMax = std::max(rawMax, rawValue);
    if (newMin < rawMin || newMax > rawMax) {
      saveCalibrationData(cfg.pin, newMin, newMax);
      displayCalibration(cfg.cc, newMin, newMax);
    }
  }
  delay(200);
}
