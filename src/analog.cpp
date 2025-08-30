#include "display.h"
#include <Arduino.h>

const uint8_t ANALOG_PIN = A0; // Analog input pin 14

void setupAnalogIO() { pinMode(ANALOG_PIN, INPUT); }

int lastAnalogValue = -1;
bool analogMoved = false;
const int analogDeltaDetect =
    1; // Only send MIDI if value changes by more than this

void loopAnalogIO() {

  // Median filter for analog input
  const int medianWindow = 5;
  static int analogSamples[medianWindow] = {0};
  static int sampleIndex = 0;
  analogSamples[sampleIndex] = analogRead(ANALOG_PIN);
  sampleIndex = (sampleIndex + 1) % medianWindow;

  // Copy and sort for median
  int sorted[medianWindow];
  memcpy(sorted, analogSamples, sizeof(sorted));
  for (int i = 0; i < medianWindow - 1; i++) {
    for (int j = i + 1; j < medianWindow; j++) {
      if (sorted[j] < sorted[i]) {
        int temp = sorted[i];
        sorted[i] = sorted[j];
        sorted[j] = temp;
      }
    }
  }
  int rawAnalog = sorted[medianWindow / 2];
  int rawMin = 470;
  int rawMax = 1020; // Assuming 10-bit ADC
  int analogValue = 0;
  int cc = 2; // MIDI CC number for analog input
  if (rawAnalog <= rawMin) {
    analogValue = 0;
  } else if (rawAnalog >= rawMax) {
    analogValue = 127;
  } else {
    analogValue = (rawAnalog - rawMin) * 127 / (rawMax - rawMin);
  }
  // Always send if at min or max, otherwise require minimum delta
  // Only start sending analog MIDI after the dial is moved from its initial
  // value
  if (!analogMoved) {
    if (lastAnalogValue == -1) {
      lastAnalogValue = analogValue;
    } else if (analogValue != lastAnalogValue) {
      analogMoved = true;
    }
  }
  if (analogMoved &&
      (lastAnalogValue == -1 || (lastAnalogValue != 0 && analogValue == 0) ||
       (lastAnalogValue != 127 && analogValue == 127) ||
       abs(analogValue - lastAnalogValue) > analogDeltaDetect)) {
    usbMIDI.sendControlChange(cc, analogValue, 1); // CC#2, value, channel 1
    lastAnalogValue = analogValue;
    // Update OLED display with raw and MIDI values
    updateDisplay(cc, analogValue);
  }
}
