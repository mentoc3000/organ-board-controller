#include "analog.h"
#include "digital.h"
#include "display.h"
#include <Arduino.h>

void setup() {
  setupDigitalIO();
  setupAnalogIO();
  setupDisplay();
  delay(1);
}

void loop() {
  loopDigitalIO();
  loopAnalogIO();
  // Always call this to keep USB MIDI running smoothly
  while (usbMIDI.read()) {
    // Empty loop to process incoming MIDI messages if needed
  }
}
