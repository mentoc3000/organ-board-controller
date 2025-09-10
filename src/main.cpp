#include "analog.h"
#include "digitalIn.h"
#include "digitalOut.h"
#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

bool calibrationMode = false;

void setup() {
  setupDigitalIns();
  setupAnalogIO();
  setupDisplay();
  delay(1);

  // load toggle states and check for calibration trigger
  for (int i = 0; i < 5; i++) {
    loopAnalogIO();
  }

  calibrationMode = checkCalibrationTrigger();

  if (calibrationMode) {
    clearAnalogCalibration();
  }
}

void loop() {
  if (calibrationMode) {
    loopAnalogCalibration();
    return;
  }

  loopDigitalIns();
  loopDigitalOuts();
  loopAnalogIO();

  // Always call this to keep USB MIDI running smoothly
  while (usbMIDI.read()) {
    if (usbMIDI.getType() == usbMIDI.Clock) {
      onMidiClock();
    }
  }
}
