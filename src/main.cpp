#include "analog.h"
#include "digitalIn.h"
#include "digitalOut.h"
#include "display.h"
#include <Arduino.h>

void setup() {
  setupDigitalIns();
  setupAnalogIO();
  setupDisplay();
  delay(1);
}

void loop() {
  loopDigitalIns();
  loopDigitalOuts();
  loopAnalogIO();

  // Always call this to keep USB MIDI running smoothly
  while (usbMIDI.read()) {
    if (usbMIDI.getType() == usbMIDI.Clock) {
      onMidiClock();
    }
    // ...existing code...
  }
}
