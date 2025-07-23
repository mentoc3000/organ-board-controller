/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>

void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize pin 2 as input with pull-down resistor
  pinMode(2, INPUT_PULLDOWN);
}

void loop()
{
  // read the state of pin 2
  int inputState = digitalRead(2);
  if (inputState == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(10); // small delay to debounce
}
