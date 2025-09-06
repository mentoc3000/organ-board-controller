#include <Arduino.h>

constexpr uint8_t LED_PIN = 5;

void setupDigitalOuts() { pinMode(LED_PIN, OUTPUT); }

void loopLED() {
  static unsigned long lastCycleTime = 0;
  constexpr unsigned long cycleInterval = 1000;
  unsigned long now = millis();
  static bool ledState = false;

  if (now - lastCycleTime >= cycleInterval) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    lastCycleTime = now;
  }
}

void loopDigitalOuts() { loopLED(); }
