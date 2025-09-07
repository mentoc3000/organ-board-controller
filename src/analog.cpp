#include "display.h"
#include <Arduino.h>
#include <algorithm>
#include <array>
#include <tuple>

constexpr uint8_t ANALOG_CHANNEL = 1;
constexpr size_t NUM_KNOBS = 14;
constexpr int RAW_MIN_DEFAULT = 4;
constexpr int RAW_MAX_DEFAULT = 1020;
constexpr int ANALOG_DELTA_DETECT = 1;

struct KnobConfig {
  uint8_t pin;
  uint8_t cc;
  int rawMin;
  int rawMax;
};

const std::array<KnobConfig, NUM_KNOBS> KNOB_CONFIGS = {
    KnobConfig{A0, 9, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A1, 8, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A6, 3, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A7, 6, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A8, 5, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A9, 7, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A10, 0, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A11, 1, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A12, 2, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A13, 4, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A14, 13, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A15, 12, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A16, 11, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT},
    KnobConfig{A17, 10, RAW_MIN_DEFAULT, RAW_MAX_DEFAULT}};

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
  for (size_t i = 0; i < NUM_KNOBS; i++) {
    const KnobConfig &cfg = KNOB_CONFIGS[i];
    pinMode(cfg.pin, INPUT);
    knobs[i] = new (std::nothrow) Knob(cfg.pin, cfg.cc, cfg.rawMin, cfg.rawMax);
    if (!knobs[i]) {
      // Handle allocation failure
    }
  }
}

void loopAnalogIO() {
  for (size_t i = 0; i < NUM_KNOBS; i++) {
    Knob *knob = knobs[i];
    if (!knob)
      continue; // Safety check for failed allocation
    int rawValue = analogRead(knob->getPin());
    int ccValue = knob->update(rawValue);
    if (ccValue != -1) {
      updateDisplay(knob->getCC(), ANALOG_CHANNEL, ccValue);
    }
  }
}
