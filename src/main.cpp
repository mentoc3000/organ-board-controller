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

  // calibrationMode = checkCalibrationTrigger();

  // if (calibrationMode) {
  //   displayCalibration();
  //   clearAnalogCalibration();
  // }
}

void loop() {
  // Always call this to keep USB MIDI running smoothly
  while (usbMIDI.read()) {
    if (usbMIDI.getType() == usbMIDI.Clock) {
      onMidiClock();
    }
  }

  if (calibrationMode) {
    loopAnalogCalibration();
    return;
  }

  loopDigitalIns();
  loopDigitalOuts();
  loopAnalogIO();
}
