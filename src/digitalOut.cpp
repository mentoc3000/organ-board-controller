#include <Arduino.h>

constexpr uint8_t LED_PIN = 5;
// MIDI Clock: 24 clocks per quarter note (beat)
static uint8_t midiClockCount = 0;
static bool ledState = false;
static unsigned long lastBeatTime = 0;
constexpr unsigned long ledOnDuration = 100; // ms LED stays on per beat

void setupDigitalOuts() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

// Call this from main loop when a MIDI Clock message is received
void onMidiClock() {
  midiClockCount++;
  if (midiClockCount >= 24) { // One beat
    midiClockCount = 0;
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    lastBeatTime = millis();
  }
}

void loopDigitalOuts() {
  if (ledState && (millis() - lastBeatTime > ledOnDuration)) {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
  }
}
